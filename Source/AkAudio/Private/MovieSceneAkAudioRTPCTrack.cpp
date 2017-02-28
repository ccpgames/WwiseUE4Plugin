// Copyright (c) 2006-2016 Audiokinetic Inc. / All Rights Reserved

#include "AkAudioDevice.h"
#include "AkAudioClasses.h"

#if AK_SUPPORTS_LEVEL_SEQUENCER

#include "IMovieScenePlayer.h"
#include "MovieSceneCommonHelpers.h"

#include "MovieSceneAkAudioRTPCSection.h"
#include "MovieSceneAkAudioRTPCTrack.h"


TSharedPtr<IMovieSceneTrackInstance> UMovieSceneAkAudioRTPCTrack::CreateInstance()
{
	return MakeShareable(new UMovieSceneAkTrackInstance<UMovieSceneAkAudioRTPCTrack>(*this));
}

UMovieSceneSection* UMovieSceneAkAudioRTPCTrack::CreateNewSection()
{
	return NewObject<UMovieSceneSection>(this, UMovieSceneAkAudioRTPCSection::StaticClass(), NAME_None, RF_Transactional);
}

void UMovieSceneAkAudioRTPCTrack::Update(EMovieSceneUpdateData& UpdateData, const TArray<TWeakObjectPtr<UObject>>& RuntimeObjects, IMovieScenePlayer& Player, FMovieSceneSequenceInstance& SequenceInstance)
{
	auto AudioDevice = FAkAudioDevice::Get();
	if (!AudioDevice)
	{
		return;
	}

	auto Section = CastChecked<UMovieSceneAkAudioRTPCSection>(MovieSceneHelpers::FindNearestSectionAtTime(Sections, UpdateData.Position));
	if (!Section || !Section->IsActive())
	{
		return;
	}

	auto& RTPCName = Section->GetRTPCName();
	if (!RTPCName.Len())
	{
		return;
	}

	auto RTPCNameString = *RTPCName;
	const float Value = Section->Eval(UpdateData.Position);

	for (auto ObjectPtr : RuntimeObjects)
	{
		auto Object = ObjectPtr.Get();
		if (Object)
		{
			auto Actor = CastChecked<AActor>(Object);
			if (Actor)
			{
				AudioDevice->SetRTPCValue(RTPCNameString, Value, 0, Actor);
			}
		}
	}

	if (IsAMasterTrack())
	{
		AudioDevice->SetRTPCValue(RTPCNameString, Value, 0, nullptr);
	}
}

#if WITH_EDITORONLY_DATA
FText UMovieSceneAkAudioRTPCTrack::GetDisplayName() const
{
	return NSLOCTEXT("MovieSceneAkAudioRTPCTrack", "TrackName", "AkAudioRTPC");
}
#endif

FName UMovieSceneAkAudioRTPCTrack::GetTrackName() const
{
	const auto Section = CastChecked<UMovieSceneAkAudioRTPCSection>(MovieSceneHelpers::FindNearestSectionAtTime(Sections, 0));
	return (Section != nullptr) ? FName(*Section->GetRTPCName()) : FName(NAME_None);
}

#endif // AK_SUPPORTS_LEVEL_SEQUENCER