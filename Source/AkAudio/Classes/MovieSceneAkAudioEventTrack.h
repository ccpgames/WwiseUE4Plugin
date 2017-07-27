// Copyright (c) 2006-2016 Audiokinetic Inc. / All Rights Reserved

#pragma once

#include "MovieSceneAkTrack.h"
#include "IMovieScenePlayer.h"
#include "MovieSceneAkAudioEventTrack.generated.h"


UCLASS(MinimalAPI)
class UMovieSceneAkAudioEventTrack : public UMovieSceneAkTrack
{
	GENERATED_BODY()

public:

	UMovieSceneAkAudioEventTrack() 
	{
#if WITH_EDITORONLY_DATA && AK_SUPPORTS_LEVEL_SEQUENCER
		SetColorTint(FColor(0, 156, 255, 65));
#endif
	}

#if AK_SUPPORTS_LEVEL_SEQUENCER
	virtual UMovieSceneSection* CreateNewSection() override;
	virtual bool SupportsMultipleRows() const override { return true; }

	virtual FName GetTrackName() const override;

#if WITH_EDITORONLY_DATA
	virtual FText GetDisplayName() const override;
#endif

	AKAUDIO_API bool AddNewEvent(float Time, UAkAudioEvent* Event, const FString& EventName = FString());

#if AK_SUPPORTS_LEVEL_SEQUENCER_TEMPLATES
protected:
	virtual FMovieSceneEvalTemplatePtr CreateTemplateForSection(const UMovieSceneSection& InSection) const override;
#else
public:
	virtual TSharedPtr<IMovieSceneTrackInstance> CreateInstance() override;
	void Update(EMovieSceneUpdateData& UpdateData, const TArray<TWeakObjectPtr<UObject>>& RuntimeObjects, IMovieScenePlayer& Player, FMovieSceneSequenceInstance& SequenceInstance);
	void ClearInstance();

#if AKAUDIOEVENTTRACK_CACHE_AKCOMPONENTS
	TSet<TWeakObjectPtr<UAkComponent>> AkComponents;
	EMovieScenePlayerStatus::Type PreviousPlayerStatus = EMovieScenePlayerStatus::Stopped;
#endif // AKAUDIOEVENTTRACK_CACHE_AKCOMPONENTS
#endif // AK_SUPPORTS_LEVEL_SEQUENCER_TEMPLATES
#endif // AK_SUPPORTS_LEVEL_SEQUENCER
};
