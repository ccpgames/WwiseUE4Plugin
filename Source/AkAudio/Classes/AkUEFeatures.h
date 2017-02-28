// Defines which features of the Wwise-Unreal integration are supported in which version of UE.

#pragma once

#include "Runtime/Launch/Resources/Version.h"


// Features added in UE 4.14
#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 14
#define AK_SUPPORTS_LEVEL_SEQUENCER	1	// Level sequencer tracks for AkEvent and RTPC
#else
#define AK_SUPPORTS_LEVEL_SEQUENCER	0
#endif

// Features added in UE 4.15
#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 15
#define AK_MATINEE_TO_LEVEL_SEQUENCE_MODULE_MODIFICATIONS 1
#define AK_SUPPORTS_EVENT_DRIVEN_LOADING 1
#else
#define AK_MATINEE_TO_LEVEL_SEQUENCE_MODULE_MODIFICATIONS 0
#define AK_SUPPORTS_EVENT_DRIVEN_LOADING 0
#endif
