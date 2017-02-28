// Copyright (c) 2006-2016 Audiokinetic Inc. / All Rights Reserved

#pragma once

#include "MovieSceneAkTrack.h"
#include "IMovieScenePlayer.h"
#include "MovieSceneAkAudioEventTrack.generated.h"

// AKAUDIOEVENTTRACK_CACHE_AKCOMPONENTS needs to be defined as 1 due to the fact that Transform tracks
// reset the position of their associated AActors to the origin during their update cycle. Once this 
// behavior is resolved, the associated code can be removed. This code corrects the improperly calculated
// occlusion values.
#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 15
#define AKAUDIOEVENTTRACK_CACHE_AKCOMPONENTS 0
#else
#define AKAUDIOEVENTTRACK_CACHE_AKCOMPONENTS 1
#endif // ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION >= 15


/**
 * Handles manipulation of float properties in a movie scene
 */
UCLASS(MinimalAPI)
class UMovieSceneAkAudioEventTrack : public UMovieSceneAkTrack
{
	GENERATED_BODY()

public:

	UMovieSceneAkAudioEventTrack() 
		: bFireEventsWhenForwards(true)
		, bFireEventsWhenBackwards(true)
#if AKAUDIOEVENTTRACK_CACHE_AKCOMPONENTS
		, PreviousPlayerStatus(EMovieScenePlayerStatus::Stopped)
#endif
	{
#if WITH_EDITORONLY_DATA && AK_SUPPORTS_LEVEL_SEQUENCER
		SetColorTint(FColor(0, 156, 255, 65));
#endif
	}

#if AK_SUPPORTS_LEVEL_SEQUENCER
	/** begin UMovieSceneTrack interface */
	virtual TSharedPtr<IMovieSceneTrackInstance> CreateInstance() override;
	virtual UMovieSceneSection* CreateNewSection() override;
	virtual bool SupportsMultipleRows() const override { return true; }

	virtual FName GetTrackName() const override;

#if WITH_EDITORONLY_DATA
	virtual FText GetDisplayName() const override;
#endif
	/** end UMovieSceneTrack interface */

	AKAUDIO_API bool AddNewEvent(float Time, UAkAudioEvent* Event, const FString& EventName = FString());

	void Update(EMovieSceneUpdateData& UpdateData, const TArray<TWeakObjectPtr<UObject>>& RuntimeObjects, IMovieScenePlayer& Player, FMovieSceneSequenceInstance& SequenceInstance);
	void ClearInstance();
#endif // AK_SUPPORTS_LEVEL_SEQUENCER

protected:

	/** If events should be fired when passed playing the sequence forwards. */
	UPROPERTY(EditAnywhere, Category = TrackEvent)
	uint32 bFireEventsWhenForwards : 1;

	/** If events should be fired when passed playing the sequence backwards. */
	UPROPERTY(EditAnywhere, Category = TrackEvent)
	uint32 bFireEventsWhenBackwards : 1;

#if AKAUDIOEVENTTRACK_CACHE_AKCOMPONENTS
	TSet<TWeakObjectPtr<UAkComponent>> AkComponents;
	EMovieScenePlayerStatus::Type PreviousPlayerStatus;
#endif // AKAUDIOEVENTTRACK_CACHE_AKCOMPONENTS
};
