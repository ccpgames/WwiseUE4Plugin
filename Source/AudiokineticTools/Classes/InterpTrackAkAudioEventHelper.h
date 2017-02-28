// Copyright (c) 2006-2012 Audiokinetic Inc. / All Rights Reserved

#pragma once

#include "AkAudioEvent.h"
#include "Matinee/InterpTrack.h"

#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 15
#include "IMatinee.h"
#endif

#include "InterpTrackHelper.h"
#include "InterpTrackAkAudioEventHelper.generated.h"

UCLASS()
class UInterpTrackAkAudioEventHelper : public UInterpTrackHelper
{
	GENERATED_UCLASS_BODY()

	void OnAkEventSet(UAkAudioEvent * in_SelectedAkEvent, const FString& in_AkEventName, IMatineeBase *InterpEd, UInterpTrack * ActiveTrack);

	// Begin UInterpTrackHelper Interface
	virtual	bool PreCreateKeyframe( UInterpTrack *Track, float KeyTime ) const override;
	virtual void  PostCreateKeyframe( UInterpTrack *Track, int32 KeyIndex ) const override;
	// End UInterpTrackHelper Interface

private:
	UAkAudioEvent * SelectedAkEvent;
	FString SelectedAkEventName;
};
