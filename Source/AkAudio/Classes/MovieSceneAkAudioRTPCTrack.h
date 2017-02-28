// Copyright (c) 2006-2016 Audiokinetic Inc. / All Rights Reserved

#pragma once

#include "MovieSceneAkTrack.h"

#include "MovieSceneAkAudioRTPCTrack.generated.h"


/**
 * Handles manipulation of float properties in a movie scene
 */
UCLASS(MinimalAPI)
class UMovieSceneAkAudioRTPCTrack : public UMovieSceneAkTrack
{
	GENERATED_BODY()

public:

	UMovieSceneAkAudioRTPCTrack()
	{
#if WITH_EDITORONLY_DATA && AK_SUPPORTS_LEVEL_SEQUENCER
		SetColorTint(FColor(58, 111, 143, 65));
#endif
	}

#if AK_SUPPORTS_LEVEL_SEQUENCER
	void Update(EMovieSceneUpdateData& UpdateData, const TArray<TWeakObjectPtr<UObject>>& RuntimeObjects, IMovieScenePlayer& Player, FMovieSceneSequenceInstance& SequenceInstance);

	/** begin UMovieSceneTrack interface */
	virtual TSharedPtr<IMovieSceneTrackInstance> CreateInstance() override;
	virtual UMovieSceneSection* CreateNewSection() override;

	virtual FName GetTrackName() const override;

#if WITH_EDITORONLY_DATA
	virtual FText GetDisplayName() const override;
#endif
	/** end UMovieSceneTrack interface */
#endif // AK_SUPPORTS_LEVEL_SEQUENCER
};
