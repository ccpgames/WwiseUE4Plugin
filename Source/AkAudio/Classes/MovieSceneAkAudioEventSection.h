// Copyright (c) 2006-2016 Audiokinetic Inc. / All Rights Reserved

#pragma once

#include "MovieSceneSection.h"

#include "MovieSceneAkAudioEventSection.generated.h"


class FAkAudioDevice;


/**
* A single floating point section
*/
UCLASS(MinimalAPI)
class UMovieSceneAkAudioEventSection : public UMovieSceneSection
{
	GENERATED_BODY()

public:

	FString GetEventName() const { return (Event == nullptr) ? EventName : Event->GetName(); }

	void SetEvent(UAkAudioEvent* AudioEvent, const FString& Name) { Event = AudioEvent; EventName = Name; }
	bool IsValid() const { return Event != nullptr || !EventName.IsEmpty(); }

	bool IsPlaying() const { return PlayingIDs.Num() > 0; }

	void AddPlayingID(AkPlayingID PlayingID) { PlayingIDs.Add(PlayingID); }
	void ClearPlayingIDs() { PlayingIDs.Empty(); }

	/** returns the minimum and maximum durations for the specified Event or EventName */
	FFloatRange GetAudioDuration();

	void StopAllPlayingEvents(FAkAudioDevice* AudioDevice);
	void StopAllPlayingEvents(FAkAudioDevice* AudioDevice, float Time);

private:

	TArray<AkPlayingID> PlayingIDs;

	/** The AkAudioEvent represented by this section */
	UPROPERTY(EditAnywhere, Category = "AkAudioEvent")
	UAkAudioEvent* Event;

	/** The name of the AkAudioEvent represented by this section */
	UPROPERTY(EditAnywhere, AdvancedDisplay, Category = "AkAudioEvent")
	FString EventName;
};
