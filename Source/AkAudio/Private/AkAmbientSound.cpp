// Copyright (c) 2006-2012 Audiokinetic Inc. / All Rights Reserved

/*=============================================================================
	AkAmbientSound.cpp:
=============================================================================*/

#include "AkAudioDevice.h"
#include "AkAudioClasses.h"
#include "Net/UnrealNetwork.h"

/*------------------------------------------------------------------------------------
	AAkAmbientSound
------------------------------------------------------------------------------------*/

AAkAmbientSound::AAkAmbientSound(const class FObjectInitializer& ObjectInitializer) :
Super(ObjectInitializer)
{
	// Property initialization
	StopWhenOwnerIsDestroyed = true;
	CurrentlyPlaying = false;
	
	AkComponent = ObjectInitializer.CreateDefaultSubobject<UAkComponent>(this, TEXT("AkAudioComponent0"));
	
	AkComponent->StopWhenOwnerDestroyed = StopWhenOwnerIsDestroyed;

	RootComponent = AkComponent;

	AkComponent->AttenuationScalingFactor = 1.f;

	//bNoDelete = true;
	bHidden = true;
}

void AAkAmbientSound::PostLoad()
{
	Super::PostLoad();
#if WITH_EDITOR
	if( AkAudioEvent_DEPRECATED )
	{
		AkComponent->AkAudioEvent = AkAudioEvent_DEPRECATED;
	}
#endif
}

void AAkAmbientSound::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	AkComponent->UpdateAkReverbVolumeList(AkComponent->GetComponentLocation());
}

#if WITH_EDITOR
void AAkAmbientSound::CheckForErrors()
{
	Super::CheckForErrors();
}

void AAkAmbientSound::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	if( AkComponent )
	{
		// Reset audio component.
		if( IsCurrentlyPlaying() )
		{
			StartPlaying();
		}
	}
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void AAkAmbientSound::StartAmbientSound()
{
	StartPlaying();
}

void AAkAmbientSound::StopAmbientSound()
{
	StopPlaying();
}

void AAkAmbientSound::StartPlaying()
{
	if( !IsCurrentlyPlaying() )
	{
		FAkAudioDevice * AkAudioDevice = FAkAudioDevice::Get();
		if( AkAudioDevice )
		{
			AkAudioDevice->SetAttenuationScalingFactor(this, AkComponent->AttenuationScalingFactor);
			FString EventName = AkComponent->EventName;
			if (AkComponent->AkAudioEvent != NULL)
			{
				EventName = AkComponent->AkAudioEvent->GetName();
			}
			AkAudioDevice->PostEvent(EventName, this, 0, NULL, NULL, StopWhenOwnerIsDestroyed );
		}
	}
}

void AAkAmbientSound::StopPlaying()
{
	if( IsCurrentlyPlaying() )
	{
		// State of CurrentlyPlaying gets updated in UAkComponent::Stop() through the EndOfEvent callback.
		AkComponent->Stop();
	}
}

bool AAkAmbientSound::IsCurrentlyPlaying()
{
	bool ret = false;
	if (AkComponent)
	{
		ret = AkComponent->NumActiveEvents.GetValue() != 0;
	}

	return ret;
}
