//////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006 Audiokinetic Inc. / All Rights Reserved
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include "SoundInputBase.h"
#include <AK/Tools/Common/AkPlatformFuncs.h>
#include <AK/Tools/Common/AkLock.h>
#include <queue>

// The window size of the sample input

#ifdef AK_LOW_LATENCY
	#define AUDIO_INPUT_NUM_BUFFERS 8
	#define AUDIO_INPUT_BUFFER_SIZE 256
	#define NUM_TARGET_FREE_BUFFERS 5
#else
	#define AUDIO_INPUT_NUM_BUFFERS 8
	#define AUDIO_INPUT_BUFFER_SIZE 1024
	#define NUM_TARGET_FREE_BUFFERS (AUDIO_INPUT_NUM_BUFFERS/2)
#endif

class SoundInput : public SoundInputBase
{
public:
	SoundInput();
	virtual ~SoundInput();
	
	static SoundInput& Instance()
	{
		return ms_Instance;
	}

	virtual bool InputOn( unsigned int in_DevNumber = 0 );		// Start input recording
	virtual bool InputOff();

	virtual void Execute( AkAudioBuffer* io_pBufferOut );
	virtual void GetFormatCallback( AkAudioFormat&  io_AudioFormat );

private:
	static SoundInput ms_Instance;	
};
