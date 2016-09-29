//////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2011 Audiokinetic Inc. / All Rights Reserved
//
//////////////////////////////////////////////////////////////////////

// AkLinuxSoundEngine.h

/// \file 
/// Main Sound Engine interface, specific to Linux.

#pragma once

#include <AK/SoundEngine/Common/AkTypes.h>
#include <AK/Tools/Common/AkPlatformFuncs.h>

/// Platform specific initialization settings
/// \sa AK::SoundEngine::Init
/// \sa AK::SoundEngine::GetDefaultPlatformInitSettings
/// - \ref soundengine_initialization_advanced_soundengine_using_memory_threshold
struct AkPlatformInitSettings
{
	// Threading model.
    AkThreadProperties  threadLEngine;			///< Lower engine threading properties
	AkThreadProperties  threadBankManager;		///< Bank manager threading properties (its default priority is AK_THREAD_PRIORITY_NORMAL)
	AkThreadProperties  threadMonitor;			///< Monitor threading properties (its default priority is AK_THREAD_PRIORITY_ABOVENORMAL). This parameter is not used in Release build.
	
    // Memory.
	AkReal32            fLEngineDefaultPoolRatioThreshold;	///< 0.0f to 1.0f value: The percentage of occupied memory where the sound engine should enter in Low memory mode. \ref soundengine_initialization_advanced_soundengine_using_memory_threshold
	AkUInt32            uLEngineDefaultPoolSize;///< Lower Engine default memory pool size
	
	//Voices.
	AkUInt32			uSampleRate;			///< Sampling Rate. Default 48000 Hz
	AkUInt16            uNumRefillsInVoice;		///< Number of refill buffers in voice buffer. 2 == double-buffered, defaults to 4.
};

///< Used with \ref AK::SoundEngine::AddSecondaryOutput to specify the type of secondary output.
enum AkAudioOutputType
{
	AkOutput_None = 0,		///< Used for uninitialized type, do not use.
	AkOutput_Dummy,			///< Dummy output, simply eats the audio stream and outputs nothing.
	AkOutput_Main,			///< Main output.  This cannot be used with AddSecondaryOutput, but can be used to query information about the main output (GetSpeakerConfiguration for example).	
	AkOutput_MergeToMain,	///< This output will mix back its content to the main output, after the master mix.
	AkOutput_NumBuiltInOutputs,		///< Do not use.
	AkOutput_Plugin			///< Specify if using Audio Device Plugin Sink.
};