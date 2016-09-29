/***********************************************************************
The content of this file includes source code for the sound engine
portion of the AUDIOKINETIC Wwise Technology and constitutes "Level
Two Source Code" as defined in the Source Code Addendum attached
with this file.  Any use of the Level Two Source Code shall be
subject to the terms and conditions outlined in the Source Code
Addendum and the End User License Agreement for Wwise(R).

Version: <VERSION>  Build: <BUILDNUMBER>
Copyright (c) <COPYRIGHTYEAR> Audiokinetic Inc.
***********************************************************************/

#include "AkSinkXAudio.h"
#include <AK/SoundEngine/Common/AkSimd.h>
#include "SinkPluginParams.h"
#include <AK/Tools/Common/AkBankReadHelpers.h>


// Constructor/destructor.
SinkPluginParams::SinkPluginParams()
{
}

SinkPluginParams::~SinkPluginParams()
{
}

// Copy constructor.
SinkPluginParams::SinkPluginParams(const SinkPluginParams & in_rCopy)
{
	m_params = in_rCopy.m_params;
}

// Create duplicate.
AK::IAkPluginParam * SinkPluginParams::Clone(AK::IAkPluginMemAlloc * in_pAllocator)
{
	AKASSERT(in_pAllocator != NULL);
	return AK_PLUGIN_NEW(in_pAllocator, SinkPluginParams(*this));
}

// Init/Term.
AKRESULT SinkPluginParams::Init(AK::IAkPluginMemAlloc *	in_pAllocator,
	const void *			in_pParamsBlock,
	AkUInt32				in_ulBlockSize)
{
	if (in_ulBlockSize == 0)
	{
		// Init default parameters.
		m_params.fDirectGain = 1.f;
		return AK_Success;
	}

	return SetParamsBlock(in_pParamsBlock, in_ulBlockSize);
}

AKRESULT SinkPluginParams::Term(AK::IAkPluginMemAlloc * in_pAllocator)
{
	AKASSERT(in_pAllocator != NULL);

	AK_PLUGIN_DELETE(in_pAllocator, this);
	return AK_Success;
}

// Blob set.
AKRESULT SinkPluginParams::SetParamsBlock(const void * in_pParamsBlock,
	AkUInt32 in_ulBlockSize)
{
	AKRESULT eResult = AK_Success;

	AkUInt8 * pParamsBlock = (AkUInt8 *)in_pParamsBlock;

	m_params.fDirectGain = READBANKDATA(AkReal32, pParamsBlock, in_ulBlockSize);

	CHECKBANKDATASIZE(in_ulBlockSize, eResult);

	return eResult;
}

// Update one parameter.
AKRESULT SinkPluginParams::SetParam(AkPluginParamID in_ParamID,
	const void * in_pValue,
	AkUInt32 in_ulParamSize)
{
	AKASSERT(in_pValue != NULL);
	if (in_pValue == NULL)
	{
		return AK_InvalidParameter;
	}
	AKRESULT eResult = AK_Success;

	switch (in_ParamID)
	{
	case SINK_DIRECT_GAIN_PROP:
		m_params.fDirectGain = (*(AkReal32*)(in_pValue));
		break;

	default:
		AKASSERT(!"Invalid parameter.");
		eResult = AK_InvalidParameter;
	}

	return eResult;
}
