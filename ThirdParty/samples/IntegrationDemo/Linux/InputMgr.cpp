// InputMgr.cpp
// Copyright (C) 2010 Audiokinetic Inc
/// \file 
/// Defines the methods declared in InputMgr.h

#include "stdafx.h"

#include <algorithm>			// For Sort()
#include "InputMgr.h"
#include "Platform.h"
#include "UniversalInput.h"

#include <SDL2/SDL.h>

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
	// Basic Gesture handling (implemented in 10 minutes in a "just to get it working" fashion.
	static bool bDidMotion = false;
	SDL_Scancode lastCh = SDL_SCANCODE_UNKNOWN;

	UGStickState stickState[] = {{0.f, 0.f},{0.f,0.f} };
	UGBtnState btnState = 0;

	SDL_Event e;
	while (SDL_PollEvent(&e))
	{
		switch (e.type) {
		case SDL_KEYDOWN:
			switch (e.key.keysym.scancode)
			{
			case SDL_SCANCODE_UP:
			case SDL_SCANCODE_W:
			case SDL_SCANCODE_DOWN:
			case SDL_SCANCODE_S:
			case SDL_SCANCODE_LEFT:
			case SDL_SCANCODE_A:
			case SDL_SCANCODE_RIGHT:
			case SDL_SCANCODE_D:
			case SDL_SCANCODE_RETURN:
			case SDL_SCANCODE_BACKSPACE:
				lastCh = e.key.keysym.scancode;
				break;
			default:
				break;
			}
			break;

		case SDL_FINGERMOTION:
			{
				bool bHandleX = abs(e.tfinger.dx) > abs(e.tfinger.dy);

				if (bHandleX)
				{
					if (abs(e.tfinger.dx) > 100)
					{
						// User is largely swiping on sides, consider an escape.
						lastCh = SDL_SCANCODE_BACKSPACE;
						printf("Swiping Back\n");
						bDidMotion = true;
					}
					else if (e.tfinger.dx > 0)
					{
						lastCh = SDL_SCANCODE_RIGHT;
						bDidMotion = true;
					}
					else if (e.tfinger.dx < 0)
					{
						lastCh = SDL_SCANCODE_LEFT;
						bDidMotion = true;
					}
				}
				else //y
				{
					if (e.tfinger.dy > 0)
					{
						lastCh = SDL_SCANCODE_DOWN;
						bDidMotion = true;
					}
					else if (e.tfinger.dy < 0)
					{
						lastCh = SDL_SCANCODE_UP;
						bDidMotion = true;
					}
				}
			}
			break;

		case SDL_FINGERDOWN:
			bDidMotion = false;
			break;

		case SDL_MULTIGESTURE:
			printf("SDL_MULTIGESTURE\n");
			break;

		case SDL_FINGERUP:
			if (!bDidMotion)
			{
				// condider a touch.
				lastCh = SDL_SCANCODE_RETURN;
			}
			break;

		case SDL_QUIT:
			//done = true;
			return;
			break;

		default:
			break;
		}
	}

	switch ( lastCh )
	{
	case SDL_SCANCODE_UP:
	case SDL_SCANCODE_W:
		btnState = UG_DPAD_UP;
		break;
	case SDL_SCANCODE_DOWN:
	case SDL_SCANCODE_S:
		btnState = UG_DPAD_DOWN;
		break;
	case SDL_SCANCODE_LEFT:
	case SDL_SCANCODE_A:
		btnState = UG_DPAD_LEFT;
		break;
	case SDL_SCANCODE_RIGHT:
	case SDL_SCANCODE_D:
		btnState = UG_DPAD_RIGHT;
		break;
	case SDL_SCANCODE_RETURN:
		btnState = UG_BUTTON1;
		break;
	case SDL_SCANCODE_BACKSPACE:
		btnState = UG_BUTTON2;
		break;
	default:
		break;
	}

	m_pUInput->SetState(1, true, btnState, stickState);
}

