// InputMgr.cpp
// Copyright (C) 2010 Audiokinetic Inc
/// \file 
/// Defines the methods declared in InputMgr.h

#include "stdafx.h"

#include <algorithm>			// For Sort()
#include "InputMgr.h"
#include "Platform.h"
#include "UniversalInput.h"
#include <ncurses.h>

int g_lastCh = 0;

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

	m_pUInput = new UniversalInput;
	m_pUInput->AddDevice( 1 , UGDeviceType_GAMEPAD );

	return true;

cleanup:
	Release();
	return false;
}


UniversalInput* InputMgr::UniversalInputAdapter() const
{
	return m_pUInput;
}


void InputMgr::Release()
{
	if ( m_pUInput )
	{
		delete m_pUInput;
		m_pUInput = NULL;
	}
}

void InputMgr::Update()
{
	UGStickState stickState[] = {{0.f, 0.f},{0.f,0.f} };
	UGBtnState btnState = 0;

	switch ( g_lastCh )
	{
	case KEY_UP:
	case 'w':
		btnState = UG_DPAD_UP;
		break;
	case KEY_DOWN:
	case 's':
		btnState = UG_DPAD_DOWN;
		break;
	case KEY_LEFT:
	case 'a':
		btnState = UG_DPAD_LEFT;
		break;
	case KEY_RIGHT:
	case 'd':
		btnState = UG_DPAD_RIGHT;
		break;
	case KEY_ENTER:
	case 10:
		btnState = UG_BUTTON1;
		break;
	case KEY_BACKSPACE:
		btnState = UG_BUTTON2;
		break;
	default:
		printf("%d ", g_lastCh );
	}

	g_lastCh = 0;

	m_pUInput->SetState(1, true, btnState, stickState);
}

