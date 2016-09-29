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

//////////////////////////////////////////////////////////////////////
//
// SinkPluginParams.h
//
//////////////////////////////////////////////////////////////////////

#pragma once

#include <AK/SoundEngine/Common/IAkPlugin.h>

// Parameters IDs for the Wwise or RTPC.
enum SinkPluginParamIDs
{
	SINK_DIRECT_GAIN_PROP = 0,
	SINK_SAMPLE_MAX_PARAMS	// Keep last
};

//-----------------------------------------------------------------------------
// Structures.
//-----------------------------------------------------------------------------

// Structure of Sink Effect Stub parameters (on bus)
struct SinkPluginParamStruct
{
	float fDirectGain;
};

//-----------------------------------------------------------------------------
// Name: class SinkPluginParams
// Desc: Shared parameters implementation.
//-----------------------------------------------------------------------------
class SinkPluginParams : public AK::IAkPluginParam
{
public:
	// Constructor/destructor.
	SinkPluginParams();
	~SinkPluginParams();
	SinkPluginParams(const SinkPluginParams & in_rCopy);

	// Create duplicate.
	IAkPluginParam * Clone(AK::IAkPluginMemAlloc * in_pAllocator);

	// Init/Term.
	AKRESULT Init(AK::IAkPluginMemAlloc *	in_pAllocator,
		const void *			in_pParamsBlock,
		AkUInt32				in_ulBlockSize
		);
	AKRESULT Term(AK::IAkPluginMemAlloc * in_pAllocator);

	// Blob set.
	AKRESULT SetParamsBlock(const void * in_pParamsBlock,
		AkUInt32 in_ulBlockSize
		);

	// Update one parameter.
	AKRESULT SetParam(AkPluginParamID in_ParamID,
		const void * in_pValue,
		AkUInt32 in_ulParamSize
		);

	const SinkPluginParamStruct& GetCurrentParams() {
		return m_params;
	}

private:
	SinkPluginParamStruct m_params;	// Parameter structure.
};

