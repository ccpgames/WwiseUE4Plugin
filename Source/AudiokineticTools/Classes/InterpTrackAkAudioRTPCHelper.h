// Copyright (c) 2006-2012 Audiokinetic Inc. / All Rights Reserved

#pragma once

#include "Matinee/InterpTrack.h"

#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 15
#include "IMatinee.h"
#endif

#include "InterpTrackHelper.h"
#include "InterpTrackAkAudioRTPCHelper.generated.h"

UCLASS()
class UInterpTrackAkAudioRTPCHelper : public UInterpTrackHelper
{
	GENERATED_UCLASS_BODY()

	// Begin UInterpTrackHelper Interface
	virtual	bool PreCreateKeyframe( UInterpTrack *Track, float KeyTime ) const override;
	virtual void  PostCreateKeyframe( UInterpTrack *Track, int32 KeyIndex ) const override;
	// End UInterpTrackHelper Interface
};
