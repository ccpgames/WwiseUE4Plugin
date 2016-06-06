// InputMgr.cpp
// Copyright (C) 2010 Audiokinetic Inc 
/// \file 
/// Defines the methods declared in InputMgr.h

#include "stdafx.h"

#include "InputMgr.h"
#include "Platform.h"
#include <math.h>

#include <sampleutil/system.h>
#include "SampleApplication.h"

extern PS4SampleApplication g_application;

InputMgr::InputMgr()
{
}

InputMgr::~InputMgr()
{
}

bool InputMgr::Init(
	void* in_pParam,
	AkOSChar* in_szErrorBuffer,
	unsigned int in_unErrorBufferCharCount
)
{	
	m_pUInput = new UniversalInput;
	
	
	for (int i = 0; i < SCE_USER_SERVICE_MAX_LOGIN_USERS; i++)
	{
		m_LoggedUserId[i] = SCE_USER_SERVICE_USER_ID_INVALID;
	}

	AKVERIFY( scePadInit() == SCE_OK);
	AKVERIFY( sceUserServiceInitialize(NULL) == SCE_OK);
	SceUserServiceLoginUserIdList list;
	AKVERIFY( sceUserServiceGetLoginUserIdList(&list) ==SCE_OK );

	for (int i = 0; i < SCE_SAMPLE_UTIL_MAX_LOGIN_USERS; ++i) 
	{
		if(list.userId[i] != SCE_USER_SERVICE_USER_ID_INVALID)
		{
			AddDevice(i, list.userId[i]);
		}
	}

	return true;
}

UniversalInput* InputMgr::UniversalInputAdapter() const
{
	return m_pUInput;
}

void InputMgr::Update()
{
	if ( m_pUInput == NULL )
		return;

	UpdateConnectedDevices();
	UpdatePadData();

}

void InputMgr::UpdateConnectedDevices()
{
	SceUserServiceEvent userEvent;
	
	sceUserServiceGetEvent(&userEvent);

	bool bFound;
	while (sceUserServiceGetEvent(&userEvent) == SCE_OK)
	{
		if (userEvent.eventType == SCE_USER_SERVICE_EVENT_TYPE_LOGIN)
		{
			AKASSERT(userEvent.userId != SCE_USER_SERVICE_USER_ID_INVALID);
			// Login processing
			for(int i=0; i<SCE_SAMPLE_UTIL_MAX_LOGIN_USERS; i++)
			{
				if ( m_LoggedUserId[i] == userEvent.userId )
				{
					AddDevice(i, userEvent.userId);
					bFound = true;
				}
			}
			AKASSERT(bFound);
		} 
		else if (userEvent.eventType == SCE_USER_SERVICE_EVENT_TYPE_LOGOUT)
		{
			// Logout processing
			AKASSERT(userEvent.userId != SCE_USER_SERVICE_USER_ID_INVALID);
			for(int i=0; i<SCE_SAMPLE_UTIL_MAX_LOGIN_USERS; i++)
			{
				if ( m_LoggedUserId[i] == userEvent.userId )
				{
					RemoveDevice(i);
					bFound = false;
				}
			}
		}
	}
}

void InputMgr::UpdatePadData()
{
	int32_t handle;

	for(int i=0; i<SCE_SAMPLE_UTIL_MAX_LOGIN_USERS; i++)
	{
		if ( m_LoggedUserId[i] != SCE_USER_SERVICE_USER_ID_INVALID )
		{
			handle = (int64_t)(m_pUInput->GetGamepad(i+1)->GetExtraParam());

			scePadRead(handle, &m_UserPadData[i], 1);
			TranslateInput(&m_UserPadData[i], i);
		}
	}
}

void InputMgr::Release()
{
	AKVERIFY( sceUserServiceTerminate() == SCE_OK );
	if ( m_pUInput )
	{
		delete m_pUInput;
		m_pUInput = NULL;
	}
}

void InputMgr::TranslateInput(ScePadData* in_PadData, int in_deviceIndex)
{
	if ( m_pUInput == NULL )
		return;

	UGBtnState iState = 0;
	UGStickState joysticks[2];
	memset(joysticks, 0, sizeof(joysticks));

	if ( in_PadData->buttons & ssi::kButtonLeft )
	{
		iState |= UG_DPAD_LEFT;
	}

	if ( in_PadData->buttons & ssi::kButtonRight )
	{
		iState |= UG_DPAD_RIGHT;
	}

	if ( in_PadData->buttons & ssi::kButtonUp )
	{
		iState |= UG_DPAD_UP;
	}

	if ( in_PadData->buttons & ssi::kButtonDown )
	{
		iState |= UG_DPAD_DOWN;
	}

	if ( in_PadData->buttons & ssi::kButtonCross )
	{
		iState |= UG_BUTTON1;
	}
	if (in_PadData->buttons & ssi::kButtonCircle )
	{
		iState |= UG_BUTTON2;
	}
	if ( in_PadData->buttons & ssi::kButtonSquare )
	{
		iState |= UG_BUTTON3;
	}
	if ( in_PadData->buttons & ssi::kButtonTriangle )
	{
		iState |= UG_BUTTON4;
	}
	if (in_PadData->buttons & ssi::kButtonL1 )
	{
		iState |= UG_BUTTON5;
	}
	if ( in_PadData->buttons & ssi::kButtonR1 )
	{
		iState |= UG_BUTTON6;
	}
	if ( in_PadData->buttons & ssi::kButtonStart )
	{
		iState |= UG_BUTTON7;
	}
	if ( in_PadData->buttons & ssi::kButtonR3 )
	{
		iState |= UG_BUTTON8;
	}

	joysticks[UG_STICKLEFT].x = (in_PadData->leftStick.x - 128.0) / 255.0;
	joysticks[UG_STICKLEFT].y = -(in_PadData->leftStick.y - 128.0) / 255.0;

	joysticks[UG_STICKRIGHT].x = (in_PadData->rightStick.x - 128.0) / 255.0;
	joysticks[UG_STICKRIGHT].y = -(in_PadData->rightStick.y - 128.0) / 255.0;

	m_pUInput->SetState( in_deviceIndex+1, true, iState, joysticks );
	
}

int InputMgr::AddDevice( int index, SceUserServiceUserId in_guidInstance )
{
	AKASSERT(m_LoggedUserId[index] == SCE_USER_SERVICE_USER_ID_INVALID);
	m_LoggedUserId[index] = in_guidInstance; 

	int handle = scePadOpen(in_guidInstance, SCE_PAD_PORT_TYPE_STANDARD, 0, NULL);

	if ( ! m_pUInput->AddDevice( index+1, UGDeviceType_GAMEPAD, (void *)(int64_t)handle ) )
	{
		return 0;
	}

	return handle;
}

bool InputMgr::RemoveDevice( int index )
{
	AKVERIFY( scePadClose((int64_t)(m_pUInput->GetGamepad(index+1)->GetExtraParam())) == SCE_OK );

	AKASSERT(m_LoggedUserId[index] != SCE_USER_SERVICE_USER_ID_INVALID);
	m_LoggedUserId[index] = SCE_USER_SERVICE_USER_ID_INVALID;

	if ( ! m_pUInput->RemoveDevice( index+1 ) )
	{
		return false;
	}

	return true;
}

AkUInt32 InputMgr::GetUserId(AkUInt8 in_iPlayerIndex) const
{
	return m_LoggedUserId[in_iPlayerIndex-1];
}
