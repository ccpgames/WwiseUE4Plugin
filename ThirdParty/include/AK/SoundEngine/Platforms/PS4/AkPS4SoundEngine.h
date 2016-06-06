//////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006 Audiokinetic Inc. / All Rights Reserved
//
//////////////////////////////////////////////////////////////////////

/// \file 
/// Main Sound Engine interface, PS4 specific.

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

    // Memory.
    AkUInt32            uLEngineDefaultPoolSize;///< Lower Engine default memory pool size
	AkReal32            fLEngineDefaultPoolRatioThreshold;	///< 0.0f to 1.0f value: The percentage of occupied memory where the sound engine should enter in Low memory mode. \ref soundengine_initialization_advanced_soundengine_using_memory_threshold
	
	// (SCE_AJM_JOB_INITIALIZE_SIZE*MAX_INIT_SOUND_PER_FRAME) + (SCE_AJM_JOB_RUN_SPLIT_SIZE(4)*MAX_BANK_SRC + (SCE_AJM_JOB_RUN_SPLIT_SIZE(5)*MAX_FILE_SRC
	AkUInt32            uLEngineAcpBatchBufferSize; ///< Lower Engine default memory pool size
	bool				bHwCodecLowLatencyMode; ///< Use low latency mode for ATRAC9  (default is false).  If true, decoding jobs are submitted at the beginning of the Wwise update and it will be necessary to wait for the result.

	// Voices.
	AkUInt16            uNumRefillsInVoice;		///< Number of refill buffers in voice buffer. 2 == double-buffered, defaults to 4.	
	
	AkThreadProperties  threadMonitor;			///< Monitor threading properties (its default priority is AK_THREAD_PRIORITY_ABOVENORMAL). This parameter is not used in Release build.
};

///< API used for audio output
///< Use with AkInitSettings to select the API used for audio output.
///< \sa AK::SoundEngine::Init
enum AkAudioAPI
{
	AkAPI_Default = 1 << 0,		///< Default audio subsystem
	AkAPI_Dummy = 1 << 2,		///< Dummy output, simply eats the audio stream and outputs nothing.
};

///< Used with \ref AK::SoundEngine::AddSecondaryOutput to specify the type of secondary output.
enum AkAudioOutputType
{
	AkOutput_Dummy = 1 << 2,		///< Dummy output, simply eats the audio stream and outputs nothing.
	AkOutput_MergeToMain = 1 << 3,	///< This output will mix back its content to the main output, after the master mix.
	AkOutput_Main = 1 << 4,			///< Main output.  This cannot be used with AddSecondaryOutput, but can be used to query information about the main output (GetSpeakerConfiguration for example).	
	AkOutput_Voice = 1 << 5,		///< Use the PS4 voice channel.
	AkOutput_Personal = 1 << 6,		///< Use the Personal channel (headset).
	AkOutput_PAD = 1 << 7,			///< Use the controller speaker channel.
	AkOutput_BGM = 1 << 8,			///< Output to background music port.	
	AkOutput_Aux = 1 << 9,			///< Output to the auxiliary port of the PS4.
	AkOutput_NumOutputs = 1 << 10,	///< Do not use.
	AkOutput_OptionNotRecordable = 1 << 11, ///< This is an optional flag to tell that this output should not be recorded by the internal DVR.  OR-it with the other output values.
};

namespace AK
{
	/// Returns the current PS4 output port handle being used by the Wwise SoundEngine for main output.
	/// This should be called only once the SoundEngine has been successfully initialized, otherwise
	/// the function will return an invalid value (-1).
	///
	/// \return the current PS4 main output port handle or -1.

	extern int GetPS4OutputHandle();
};
