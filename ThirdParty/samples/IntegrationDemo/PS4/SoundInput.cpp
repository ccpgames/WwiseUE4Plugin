//////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2006 Audiokinetic Inc. / All Rights Reserved
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include <AK/Plugin/AkAudioInputSourceFactory.h>
#include <math.h>

#include "SoundInput.h"
#include "SoundInputMgr.h"

SoundInput SoundInput::ms_Instance;

//---------------------------------------------------------------------------
SoundInput::SoundInput()
{
}

//---------------------------------------------------------------------------
SoundInput::~SoundInput() 
{
}

//---------------------------------------------------------------------------
bool SoundInput::InputOn( unsigned int )
{
	return false;
}

//---------------------------------------------------------------------------
bool SoundInput::InputOff()
{
	return true;
}


//---------------------------------------------------------------------------
void SoundInput::GetFormatCallback(
        AkAudioFormat&  io_AudioFormat  // Already filled format, modify it if required.
	    )
{
    io_AudioFormat.SetAll(
            48000,// Sample rate
            AK_SPEAKER_SETUP_MONO,
            16,						// Bits per samples
            2,						// 2 bytes per samples
            AK_INT,					// feeding integers(signed)
            AK_INTERLEAVED
            );
}


void SoundInput::Execute(
        AkAudioBuffer*	io_pBufferOut  // Buffer to fill
	    )
{
    io_pBufferOut->eState = AK_Fail;
    io_pBufferOut->uValidFrames = 0;
}
