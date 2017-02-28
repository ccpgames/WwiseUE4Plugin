// Copyright (c) 2006-2016 Audiokinetic Inc. / All Rights Reserved

#include "AkAudioDevice.h"
#include "AkAudioClasses.h"

#if AK_SUPPORTS_LEVEL_SEQUENCER

#include "IMovieScenePlayer.h"
#include "MovieScene.h"
#include "MovieSceneAkAudioEventSection.h"
#include "MovieSceneAkAudioEventTrack.h"

namespace AkAudioEventTrackHelper
{
	void PostEventCallback(AkCallbackType in_eType, AkCallbackInfo* in_pCallbackInfo)
	{
		if (in_pCallbackInfo != nullptr && in_eType == AK_EndOfEvent)
		{
			auto Section = (UMovieSceneAkAudioEventSection*)in_pCallbackInfo->pCookie;
			Section->ClearPlayingIDs();
		}
	}

	void PostEvent(UMovieSceneAkAudioEventSection* Section, FAkAudioDevice* AudioDevice, AActor* Actor)
	{
		ensure(Section);

		if (AudioDevice)
		{
			enum { StopWhenOwnerIsDestroyed = false, };
			auto PlayingID = AudioDevice->PostEvent(Section->GetEventName(), Actor, AK_EndOfEvent, &PostEventCallback, Section, StopWhenOwnerIsDestroyed);
			if (PlayingID != AK_INVALID_PLAYING_ID)
			{
				Section->AddPlayingID(PlayingID);
			}
		}
	}
}


class UMovieSceneAkAudioEventTrackInstance : public UMovieSceneAkTrackInstance<UMovieSceneAkAudioEventTrack>
{
public:
	UMovieSceneAkAudioEventTrackInstance(UMovieSceneAkAudioEventTrack& InAkTrack)
		: UMovieSceneAkTrackInstance(InAkTrack)
	{}

	virtual void RestoreState(const TArray<TWeakObjectPtr<UObject>>& RuntimeObjects, IMovieScenePlayer& Player, FMovieSceneSequenceInstance& SequenceInstance) override
	{
		AkTrack->ClearInstance();
	}

	virtual void ClearInstance(IMovieScenePlayer& Player, FMovieSceneSequenceInstance& SequenceInstance) override
	{
		AkTrack->ClearInstance();
	}

	virtual EMovieSceneUpdatePass HasUpdatePasses() override { return MSUP_PostUpdate; }

	virtual bool RequiresUpdateForSubSceneDeactivate() override { return true; }
};


TSharedPtr<IMovieSceneTrackInstance> UMovieSceneAkAudioEventTrack::CreateInstance()
{
	return MakeShareable(new UMovieSceneAkAudioEventTrackInstance(*this));
}

UMovieSceneSection* UMovieSceneAkAudioEventTrack::CreateNewSection()
{
	return NewObject<UMovieSceneSection>(this, UMovieSceneAkAudioEventSection::StaticClass(), NAME_None, RF_Transactional);
}

bool UMovieSceneAkAudioEventTrack::AddNewEvent(float Time, UAkAudioEvent* Event, const FString& EventName)
{
	if (Event == nullptr && EventName.IsEmpty())
		return false;

	auto NewSection = CastChecked<UMovieSceneAkAudioEventSection>(CreateNewSection());
	ensure(NewSection);

	NewSection->SetEvent(Event, EventName);

	const auto Duration = NewSection->GetAudioDuration();
	NewSection->InitialPlacement(GetAllSections(), Time, Time + Duration.GetUpperBoundValue(), SupportsMultipleRows());
	AddSection(*NewSection);

	return true;
}

void UMovieSceneAkAudioEventTrack::Update(EMovieSceneUpdateData& UpdateData, const TArray<TWeakObjectPtr<UObject>>& RuntimeObjects, IMovieScenePlayer& Player, FMovieSceneSequenceInstance& SequenceInstance)
{
	if (Sections.Num() == 0)
	{
		return;
	}

	const bool Backwards = UpdateData.Position < UpdateData.LastPosition;
	if (Backwards ? !bFireEventsWhenBackwards : !bFireEventsWhenForwards)
	{
		return;
	}

	auto AudioDevice = FAkAudioDevice::Get();

	const auto PlaybackStatus = Player.GetPlaybackStatus();

	switch (PlaybackStatus)
	{
	case EMovieScenePlayerStatus::Stopped:
		for (auto Section : Sections)
		{
			auto EventSection = CastChecked<UMovieSceneAkAudioEventSection>(Section);
			ensure(EventSection);
			EventSection->StopAllPlayingEvents(AudioDevice);
		}

#if AKAUDIOEVENTTRACK_CACHE_AKCOMPONENTS
		AkComponents.Empty();
#endif // AKAUDIOEVENTTRACK_CACHE_AKCOMPONENTS
		break;

	case EMovieScenePlayerStatus::Playing:
		if (UpdateData.Position >= UpdateData.LastPosition)
		{
			for (auto Section : Sections)
			{
				auto EventSection = CastChecked<UMovieSceneAkAudioEventSection>(Section);
				ensure(EventSection);

				if (!EventSection->IsValid() || !EventSection->IsActive())
				{
					continue;
				}

				auto StartTime = EventSection->GetStartTime();
				if (!EventSection->IsPlaying())
				{
					if (AudioDevice && UpdateData.LastPosition <= StartTime && UpdateData.Position > StartTime)
					{
						for (auto ObjectPtr : RuntimeObjects)
						{
							auto Object = ObjectPtr.Get();
							if (!Object)
							{
								continue;
							}

							auto Actor = CastChecked<AActor>(Object);
							if (!IsValid(Actor))
							{
								continue;
							}

							auto AkComponent = AudioDevice->GetAkComponent(Actor->GetRootComponent(), FName(), NULL, EAttachLocation::KeepRelativeOffset);
							if (!AkComponent)
							{
								continue;
							}

							AkComponent->CalculateOcclusionValues(false);

							auto PlayingID = AkComponent->PostAkEventByNameWithCallback(EventSection->GetEventName(), AK_EndOfEvent, &AkAudioEventTrackHelper::PostEventCallback, EventSection);
							if (PlayingID != AK_INVALID_PLAYING_ID)
							{
								EventSection->AddPlayingID(PlayingID);

#if AKAUDIOEVENTTRACK_CACHE_AKCOMPONENTS
								AkComponents.Add(AkComponent);
#endif // AKAUDIOEVENTTRACK_CACHE_AKCOMPONENTS
							}
						}

						if (IsAMasterTrack())
						{
							AkAudioEventTrackHelper::PostEvent(EventSection, AudioDevice, nullptr);
						}
					}
				}
				else if (UpdateData.Position > EventSection->GetEndTime())
				{
					EventSection->StopAllPlayingEvents(AudioDevice, UpdateData.Position);
				}
			}
		}
		break;
	}

#if AKAUDIOEVENTTRACK_CACHE_AKCOMPONENTS
	if (PreviousPlayerStatus == EMovieScenePlayerStatus::Playing)
	{
		AkComponents.Remove(nullptr);

		for (auto AkComponentPtr : AkComponents)
		{
			auto AkComponent = AkComponentPtr.Get();
			AkComponent->CalculateOcclusionValues(false);
		}
	}

	PreviousPlayerStatus = PlaybackStatus;
#endif // AKAUDIOEVENTTRACK_CACHE_AKCOMPONENTS
}

void UMovieSceneAkAudioEventTrack::ClearInstance()
{
	auto AudioDevice = FAkAudioDevice::Get();
	for (auto Section : Sections)
	{
		auto EventSection = CastChecked<UMovieSceneAkAudioEventSection>(Section);
		ensure(EventSection);
		EventSection->StopAllPlayingEvents(AudioDevice);
	}

#if AKAUDIOEVENTTRACK_CACHE_AKCOMPONENTS
	AkComponents.Empty();
#endif // AKAUDIOEVENTTRACK_CACHE_AKCOMPONENTS
}

#if WITH_EDITORONLY_DATA
FText UMovieSceneAkAudioEventTrack::GetDisplayName() const
{
	return NSLOCTEXT("MovieSceneAkAudioEventTrack", "TrackName", "AkAudioEvents");
}
#endif

FName UMovieSceneAkAudioEventTrack::GetTrackName() const
{
	static FName TrackName("AkAudioEvents");
	return TrackName;
}

#endif // AK_SUPPORTS_LEVEL_SEQUENCER