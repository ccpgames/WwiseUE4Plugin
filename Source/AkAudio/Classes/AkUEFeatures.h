// Defines which features of the Wwise-Unreal integration are supported in which version of UE.

#pragma once

#include <AK/AkWwiseSDKVersion.h>
#include "Runtime/Launch/Resources/Version.h"

#if AK_WWISESDK_VERSION_MAJOR >= 2017 
#define AK_SPATIAL_AUDIO_AVAILABLE	1
#else
#define AK_SPATIAL_AUDIO_AVAILABLE	0
#endif

#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 12
#define	UE_4_12_OR_LATER 1
#else
#define	UE_4_12_OR_LATER 0
#endif

#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 13
#define	UE_4_13_OR_LATER 1
#else
#define	UE_4_13_OR_LATER 0
#endif

// Features added in UE 4.14
#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 14
#define	UE_4_14_OR_LATER 1
#define AK_SUPPORTS_LEVEL_SEQUENCER	1	// Level sequencer tracks for AkEvent and RTPC
#else
#define	UE_4_14_OR_LATER 0
#define AK_SUPPORTS_LEVEL_SEQUENCER	0
#endif

// Features added in UE 4.15
#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 15
#define	UE_4_15_OR_LATER 1
#define AK_MATINEE_TO_LEVEL_SEQUENCE_MODULE_MODIFICATIONS 1
#define AK_SUPPORTS_EVENT_DRIVEN_LOADING 1
#define AK_SUPPORTS_LEVEL_SEQUENCER_TEMPLATES 1
#else
#define	UE_4_15_OR_LATER 0
#define AK_MATINEE_TO_LEVEL_SEQUENCE_MODULE_MODIFICATIONS 0
#define AK_SUPPORTS_EVENT_DRIVEN_LOADING 0
#define AK_SUPPORTS_LEVEL_SEQUENCER_TEMPLATES 0
#endif

#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 16
#define	UE_4_16_OR_LATER 1
#define AK_FIOSYSTEM_AVAILABLE 0
#else
#define	UE_4_16_OR_LATER 0
#define AK_FIOSYSTEM_AVAILABLE 1
#endif

// In UE4.14, AKAUDIOEVENTTRACK_CACHE_AKCOMPONENTS needs to be defined as non-zero due to the fact 
// that Transform tracks reset the position of their associated AActors to the origin during their 
// update cycle. The code within these sections corrects the improperly calculated occlusion 
// values.
//
// This has been resolved in UE4.15 with the addition of the FMovieSceneEvalTemplate class and its 
// associated handling of updating objects.
#define AKAUDIOEVENTTRACK_CACHE_AKCOMPONENTS (ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION == 14)
