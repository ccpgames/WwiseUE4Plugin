// Platform.h
// Copyright (C) 2010 Audiokinetic Inc
/// \file 
/// Contains platform-specific helpers for the Vita platform.

#pragma once

#define DEMO_DEFAULT_POOL_SIZE 2*1024*1024
#define DEMO_LENGINE_DEFAULT_POOL_SIZE 2*1024*1024

extern const AkOSChar* g_szIntegrationDemoBanksPath;
#define SOUND_BANK_PATH g_szIntegrationDemoBanksPath

// Defines for button name strings
#define UG_BUTTON1_NAME  "Cross"
#define UG_BUTTON2_NAME  "Circle"
#define UG_BUTTON3_NAME  "Square"
#define UG_BUTTON4_NAME  "Triangle"
#define UG_BUTTON5_NAME  "L"
#define UG_BUTTON6_NAME  "R"
#define UG_BUTTON7_NAME  "START"
#define UG_BUTTON8_NAME  "SELECT"
#define UG_BUTTON9_NAME  ""
#define UG_BUTTON10_NAME ""
#define UG_BUTTON11_NAME ""
#define UG_RIGHT_STICK	 "Right Analog Stick"
#define UG_LEFT_STICK	 "Left Analog Stick"
#define OS_BUTTON_NAME   ""
#define DIRECTIONAL_TYPE "Directional Buttons"
#define HELP_BUTTON   UG_BUTTON7_NAME
#define ACTION_BUTTON UG_BUTTON1_NAME
#define BACK_BUTTON   UG_BUTTON2_NAME

#define INTEGRATIONDEMO_SCREEN_CHAR_WIDTH	60

// Assert hook for the sound engine
void IntegrationDemoAssertHook( 
	const char * in_pszExpression,
    const char * in_pszFileName,
    int in_lineNumber
	);

#define INTEGRATIONDEMO_ASSERT_HOOK IntegrationDemoAssertHook

#define __AK_OSCHAR_SNPRINTF snprintf

#if AK_VITA_HW
#define CODECTYPE_STANDARD	AKCODECID_VAG
#else
#define CODECTYPE_STANDARD	AKCODECID_ADPCM
#endif

#define GET_PLAYER_ID(_index) IntegrationDemo::Instance().GetInputMgr()->GetUserId(_index+1)
#define CONTROLLER_OUTPUT(_index) AkOutput_PAD

#define INTEGRATIONDEMO_BGM