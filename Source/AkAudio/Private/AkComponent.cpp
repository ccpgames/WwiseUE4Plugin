// Copyright (c) 2006-2012 Audiokinetic Inc. / All Rights Reserved

/*=============================================================================
	AkComponent.cpp:
=============================================================================*/

#include "AkAudioDevice.h"
#include "AkInclude.h"
#include "AkAudioClasses.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Texture2D.h"
#include "Components/BillboardComponent.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "AkComponentCallbackManager.h"

/*------------------------------------------------------------------------------------
	UAkComponent
------------------------------------------------------------------------------------*/


UAkComponent::UAkComponent(const class FObjectInitializer& ObjectInitializer) :
Super(ObjectInitializer)
{
	// Property initialization
 	StopWhenOwnerDestroyed = true;
	bUseReverbVolumes = true;
	OcclusionRefreshInterval = 0.2f;
	LastOcclusionRefresh = -1;

	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickGroup = TG_DuringPhysics;
	PrimaryComponentTick.bAllowTickOnDedicatedServer = false;
	bTickInEditor = true;

	bAutoActivate = true;
	bNeverNeedsRenderUpdate = true;
	bWantsOnUpdateTransform = true;

#if WITH_EDITORONLY_DATA
	bVisualizeComponent = true;
#endif

	AttenuationScalingFactor = 1.0f;
	bAutoDestroy = false;
	bStarted = false;
}

int32 UAkComponent::PostAssociatedAkEvent()
{
	return PostAkEvent(AkAudioEvent, EventName);
}

int32 UAkComponent::PostAkEvent( class UAkAudioEvent * AkEvent, const FString& in_EventName )
{
	return PostAkEventByName(GET_AK_EVENT_NAME(AkEvent, in_EventName));
}

int32 UAkComponent::PostAkEventByName(const FString& in_EventName)
{
	return PostAkEventByNameWithCallback(in_EventName);
}

bool UAkComponent::VerifyEventName(const FString& in_EventName) const
{
	const bool IsEventNameEmpty = in_EventName.IsEmpty();
	if (IsEventNameEmpty)
	{
		FString OwnerName = FString(TEXT(""));
		FString ObjectName = GetFName().ToString();

		const auto owner = GetOwner();
		if (owner)
			OwnerName = owner->GetName();

		UE_LOG(LogAkAudio, Warning, TEXT("[%s.%s] AkComponent: Attempted to post an empty AkEvent name."), *OwnerName, *ObjectName);
	}

	return !IsEventNameEmpty;
}

bool UAkComponent::AllowAudioPlayback() const
{
	UWorld* CurrentWorld = GetWorld();
	return (CurrentWorld && CurrentWorld->AllowAudioPlayback() && !IsBeingDestroyed());
}

AkPlayingID UAkComponent::PostAkEventByNameWithCallback(const FString& in_EventName, AkUInt32 in_uFlags /*= 0*/, AkCallbackFunc in_pfnUserCallback /*= NULL*/, void * in_pUserCookie /*= NULL*/)
{
	AkPlayingID playingID = AK_INVALID_PLAYING_ID;

	auto AudioDevice = FAkAudioDevice::Get();
	if (AudioDevice)
	{
		playingID = AudioDevice->PostEvent(in_EventName, this, in_uFlags, in_pfnUserCallback, in_pUserCookie);
		if (playingID != AK_INVALID_PLAYING_ID)
			bStarted = true;
	}

	return playingID;
}

void UAkComponent::Stop()
{
	if (  FAkAudioDevice::Get() )
	{
		AK::SoundEngine::StopAll(GetAkGameObjectID());
	}
}

void UAkComponent::SetRTPCValue( FString RTPC, float Value, int32 InterpolationTimeMs = 0)
{
	if ( FAkAudioDevice::Get() )
	{
		auto szRTPC = TCHAR_TO_AK(*RTPC);
		AK::SoundEngine::SetRTPCValue( szRTPC, Value, GetAkGameObjectID(), InterpolationTimeMs );
	}
}

void UAkComponent::PostTrigger( FString Trigger )
{
	if ( FAkAudioDevice::Get() )
	{
		auto szTrigger = TCHAR_TO_AK(*Trigger);
		AK::SoundEngine::PostTrigger( szTrigger, GetAkGameObjectID());
	}
}

void UAkComponent::SetSwitch( FString SwitchGroup, FString SwitchState )
{
	if ( FAkAudioDevice::Get() )
	{
		auto szSwitchGroup = TCHAR_TO_AK(*SwitchGroup);
		auto szSwitchState = TCHAR_TO_AK(*SwitchState);

		AK::SoundEngine::SetSwitch( szSwitchGroup, szSwitchState, GetAkGameObjectID());
	}
}

void UAkComponent::SetStopWhenOwnerDestroyed( bool bStopWhenOwnerDestroyed )
{
	StopWhenOwnerDestroyed = bStopWhenOwnerDestroyed;
}

void UAkComponent::SetActiveListeners_Implementation( int32 ListenerMask )
{
	if ( FAkAudioDevice::Get() )
	{
		AK::SoundEngine::SetActiveListeners( (AkGameObjectID) this, ListenerMask );
	}
}

void UAkComponent::UseReverbVolumes(bool inUseReverbVolumes)
{
	bUseReverbVolumes = inUseReverbVolumes;
}

float UAkComponent::GetAttenuationRadius_Implementation() const
{ 
	if( AkAudioEvent )
	{
		return AttenuationScalingFactor * AkAudioEvent->MaxAttenuationRadius;
	}
	
	return 0.f;
}

void UAkComponent::SetOutputBusVolume(float BusVolume)
{
	FAkAudioDevice * AudioDevice = FAkAudioDevice::Get();
	if (AudioDevice)
	{
		AudioDevice->SetGameObjectOutputBusVolume(this, BusVolume);
	}
}

void UAkComponent::OnRegister()
{
	RegisterGameObject(); // Done before parent so that OnUpdateTransform follows registration and updates position correctly.
	if (OcclusionRefreshInterval > 0.0f)
	{
		UWorld* CurrentWorld = GetWorld();
		if (CurrentWorld)
		{
			LastOcclusionRefresh = CurrentWorld->GetTimeSeconds() + FMath::RandRange(0.0f, OcclusionRefreshInterval);
		}
	}

	Super::OnRegister();

#if WITH_EDITORONLY_DATA
	UpdateSpriteTexture();
#endif
}

#if WITH_EDITORONLY_DATA
void UAkComponent::UpdateSpriteTexture()
{
	if (SpriteComponent)
	{
		SpriteComponent->SetSprite(LoadObject<UTexture2D>(NULL, TEXT("/Wwise/S_AkComponent.S_AkComponent")));
	}
}
#endif


void UAkComponent::OnUnregister()
{
	// Route OnUnregister event.
	Super::OnUnregister();

	// Don't stop audio and clean up component if owner has been destroyed (default behaviour). This function gets
	// called from AActor::ClearComponents when an actor gets destroyed which is not usually what we want for one-
	// shot sounds.
	AActor* Owner = GetOwner();
	UWorld* CurrentWorld = GetWorld();
	if( !Owner || !CurrentWorld || StopWhenOwnerDestroyed || CurrentWorld->bIsTearingDown || (Owner->GetClass() == APlayerController::StaticClass() && CurrentWorld->WorldType == EWorldType::PIE))
	{
		Stop();
	}
}

void UAkComponent::FinishDestroy( void )
{
	UnregisterGameObject();

	Super::FinishDestroy();
}

void UAkComponent::OnComponentDestroyed( bool bDestroyingHierarchy )
{
	Super::OnComponentDestroyed(bDestroyingHierarchy);

	UnregisterGameObject();
}

void UAkComponent::ShutdownAfterError( void )
{
	UnregisterGameObject();

	Super::ShutdownAfterError();
}

void UAkComponent::ApplyAkReverbVolumeList(float DeltaTime)
{
	if( CurrentAkReverbVolumes.Num() > 0 )
	{
		// Fade control
		for( int32 Idx = 0; Idx < CurrentAkReverbVolumes.Num(); Idx++ )
		{
			if( CurrentAkReverbVolumes[Idx].CurrentControlValue != CurrentAkReverbVolumes[Idx].TargetControlValue || CurrentAkReverbVolumes[Idx].bIsFadingOut )
			{
				float Increment = ComputeFadeIncrement(DeltaTime, CurrentAkReverbVolumes[Idx].FadeRate, CurrentAkReverbVolumes[Idx].TargetControlValue);
				if( CurrentAkReverbVolumes[Idx].bIsFadingOut )
				{
					CurrentAkReverbVolumes[Idx].CurrentControlValue -= Increment;
					if( CurrentAkReverbVolumes[Idx].CurrentControlValue <= 0.f )
					{
						CurrentAkReverbVolumes.RemoveAt(Idx);
					}
				}
				else
				{
					CurrentAkReverbVolumes[Idx].CurrentControlValue += Increment;
					if( CurrentAkReverbVolumes[Idx].CurrentControlValue > CurrentAkReverbVolumes[Idx].TargetControlValue )
					{
						CurrentAkReverbVolumes[Idx].CurrentControlValue = CurrentAkReverbVolumes[Idx].TargetControlValue;
					}
				}
			}
		}

		// Sort the list of active AkReverbVolumes by descending priority, if necessary
		if(CurrentAkReverbVolumes.Num() > 1 )
		{
			CurrentAkReverbVolumes.Sort([](const AkReverbVolumeFadeControl& A, const AkReverbVolumeFadeControl& B)
			{
				// Ensure the fading out buffers are sent to the end of the array
				return (A.bIsFadingOut == B.bIsFadingOut) ? (A.Priority > B.Priority) : (A.bIsFadingOut < B.bIsFadingOut);
			});
		}
	}

	TArray<AkAuxSendValue> AkReverbVolumes;
	AkAuxSendValue	TmpSendValue;
	AkReverbVolumes.Empty();

	// Build a list to set as AuxBusses
	FAkAudioDevice * AkAudioDevice = FAkAudioDevice::Get();
	if( AkAudioDevice )
	{
		for( int32 Idx = 0; Idx < CurrentAkReverbVolumes.Num() && Idx < AkAudioDevice->GetMaxAuxBus(); Idx++ )
		{
			TmpSendValue.auxBusID = CurrentAkReverbVolumes[Idx].AuxBusId;
			TmpSendValue.fControlValue = CurrentAkReverbVolumes[Idx].CurrentControlValue;
			AkReverbVolumes.Add(TmpSendValue);
		}

		AkAudioDevice->SetAuxSends(GetAkGameObjectID(), AkReverbVolumes);
	}

}

void UAkComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	if ( AK::SoundEngine::IsInitialized() )
	{
		Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

		FAkAudioDevice * AkAudioDevice = FAkAudioDevice::Get();
		if( AkAudioDevice )
		{
			// Update AkReverbVolume fade in/out
			if( bUseReverbVolumes && AkAudioDevice->GetMaxAuxBus() > 0 )
			{
				ApplyAkReverbVolumeList(DeltaTime);
			}
		}

		// Check Occlusion/Obstruction, if enabled
		if( OcclusionRefreshInterval > 0.f || ClearingOcclusion )
		{
			SetOcclusion(DeltaTime);
		}
		else
		{
			ClearOcclusionValues();
		}

		if( !HasActiveEvents() && bAutoDestroy && bStarted)
		{
			DestroyComponent();
		}
	}
}


void UAkComponent::Activate(bool bReset)
{
	Super::Activate( bReset );

	UpdateGameObjectPosition();

	// If spawned inside AkReverbVolume(s), we do not want the fade in effect to kick in.
	UpdateAkReverbVolumeList(GetComponentLocation());
	for( int32 Idx = 0; Idx < CurrentAkReverbVolumes.Num(); Idx++ )
	{
		CurrentAkReverbVolumes[Idx].CurrentControlValue = CurrentAkReverbVolumes[Idx].TargetControlValue;
	}

	FAkAudioDevice * AudioDevice = FAkAudioDevice::Get();
	if (AudioDevice)
	{
		AudioDevice->SetAttenuationScalingFactor(this, AttenuationScalingFactor);
	}
}

void UAkComponent::SetAttenuationScalingFactor_Implementation(float Value)
{
	AttenuationScalingFactor = Value;
	FAkAudioDevice * AudioDevice = FAkAudioDevice::Get();
	if (AudioDevice)
	{
		AudioDevice->SetAttenuationScalingFactor(this, AttenuationScalingFactor);
	}
}


#if ENGINE_MAJOR_VERSION == 4 && ENGINE_MINOR_VERSION <= 11
void UAkComponent::OnUpdateTransform(bool bSkipPhysicsMove, ETeleportType Teleport)
{
	Super::OnUpdateTransform(bSkipPhysicsMove, Teleport);
#else
void UAkComponent::OnUpdateTransform(EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport)
{
	Super::OnUpdateTransform(UpdateTransformFlags, Teleport);
#endif

	UpdateGameObjectPosition();
}

bool UAkComponent::HasActiveEvents() const
{
	auto CallbackManager = FAkComponentCallbackManager::GetInstance();
	return (CallbackManager != nullptr) && CallbackManager->HasActiveEvents(GetAkGameObjectID());
}

AkGameObjectID UAkComponent::GetAkGameObjectID() const
{
	return (AkGameObjectID)this;
}

void UAkComponent::RegisterGameObject()
{
	FAkAudioDevice* AkAudioDevice = FAkAudioDevice::Get();
	if ( AkAudioDevice )
	{
		AkAudioDevice->RegisterComponent(this);
	}
}

void UAkComponent::UnregisterGameObject()
{
	FAkAudioDevice* AkAudioDevice = FAkAudioDevice::Get();
	if (AkAudioDevice)
		AkAudioDevice->UnregisterComponent(this);
}

int32 UAkComponent::FindNewAkReverbVolumeInCurrentlist(uint32 AuxBusId)
{
	return CurrentAkReverbVolumes.IndexOfByPredicate([=](const AkReverbVolumeFadeControl& Candidate)
	{
		return AuxBusId == Candidate.AuxBusId;
	});
}

static int32 FindCurrentAkReverbVolumeInNewlist(TArray<AAkReverbVolume*> FoundVolumes, AkAuxBusID AuxBusId)
{
	return FoundVolumes.IndexOfByPredicate([=](const AAkReverbVolume* const Candidate)
	{
		return AuxBusId == Candidate->GetAuxBusId();
	});
}

void FindAkReverbVolumesAtLocation(FVector Loc, TArray<AAkReverbVolume*>& FoundVolumes, const UWorld* in_World);
void UAkComponent::UpdateAkReverbVolumeList( FVector Loc )
{
	TArray<AAkReverbVolume*> FoundVolumes;
	FindAkReverbVolumesAtLocation(Loc, FoundVolumes, GetWorld());

	// Add the new volumes to the current list
	for( int32 Idx = 0; Idx < FoundVolumes.Num(); Idx++ )
	{
		AkAuxBusID	CurrentAuxBusId = FoundVolumes[Idx]->GetAuxBusId();
		int32 FoundIdx = FindNewAkReverbVolumeInCurrentlist( CurrentAuxBusId );
		if( FoundIdx == INDEX_NONE )
		{
			// The volume was not found, add it to the list
			CurrentAkReverbVolumes.Add(AkReverbVolumeFadeControl(CurrentAuxBusId, 0.f, FoundVolumes[Idx]->SendLevel, FoundVolumes[Idx]->FadeRate, false, FoundVolumes[Idx]->Priority));
		}
		else
		{
			// The volume was found. We still have to check if it is currently fading out, in case we are
			// getting back in a volume we just exited.
			if( CurrentAkReverbVolumes[FoundIdx].bIsFadingOut == true )
			{
				CurrentAkReverbVolumes[FoundIdx].bIsFadingOut = false;
			}
		}
	}

	// Fade out the current volumes not found in the new list
	for( int32 Idx = 0; Idx < CurrentAkReverbVolumes.Num(); Idx++ )
	{
		if( FindCurrentAkReverbVolumeInNewlist(FoundVolumes, CurrentAkReverbVolumes[Idx].AuxBusId) == INDEX_NONE )
		{
			// Our current volume was not found in the array of volumes at the current position. Begin fading it out
			CurrentAkReverbVolumes[Idx].bIsFadingOut = true;
		}
	}
}

void UAkComponent::UpdateGameObjectPosition()
{
	FAkAudioDevice * AkAudioDevice = FAkAudioDevice::Get();
	if ( bIsActive && AkAudioDevice )
	{
		AkSoundPosition soundpos;
			FAkAudioDevice::FVectorsToAKTransform(ComponentToWorld.GetTranslation(), ComponentToWorld.GetUnitAxis(EAxis::X), ComponentToWorld.GetUnitAxis(EAxis::Z), soundpos);

		AK::SoundEngine::SetPosition(GetAkGameObjectID(), soundpos );

		// Find and apply all AkReverbVolumes at this location
		if( bUseReverbVolumes && AkAudioDevice->GetMaxAuxBus() > 0 )
		{
			UpdateAkReverbVolumeList( GetComponentLocation() );
		}
	}
}

const float UAkComponent::OCCLUSION_FADE_RATE = 2.0f; // from 0.0 to 1.0 in 0.5 seconds
void UAkComponent::SetOcclusion(const float DeltaTime)
{
	// Fade the active occlusions
	for(int32 ListenerIdx = 0; ListenerIdx < ListenerOcclusionInfo.Num(); ListenerIdx++)
	{
		if( ListenerOcclusionInfo[ListenerIdx].CurrentValue != ListenerOcclusionInfo[ListenerIdx].TargetValue )
		{
			bool bIsFadingOut = ListenerOcclusionInfo[ListenerIdx].CurrentValue > ListenerOcclusionInfo[ListenerIdx].TargetValue;
			float sign = bIsFadingOut ? -1.0 : 1.0;
			float maxVal = bIsFadingOut ? ListenerOcclusionInfo[ListenerIdx].CurrentValue : ListenerOcclusionInfo[ListenerIdx].TargetValue;
			
			ListenerOcclusionInfo[ListenerIdx].CurrentValue = FMath::Clamp( 
				ListenerOcclusionInfo[ListenerIdx].CurrentValue + (sign * OCCLUSION_FADE_RATE * DeltaTime), 
				0.0f, 
				maxVal
				);

			FAkAudioDevice * AkAudioDevice = FAkAudioDevice::Get();
			if( AkAudioDevice )
			{
				AkAudioDevice->SetOcclusionObstruction(this, ListenerIdx, 0.0f, ListenerOcclusionInfo[ListenerIdx].CurrentValue);
			}
		}
	}

	UWorld* CurrentWorld = GetWorld();
	// Compute occlusion only when needed.
	// Have to have "LastOcclutionRefresh == -1" because GetWorld() might return nullptr in UAkComponent's constructor,
	// preventing us from initializing it to something smart.
	if( !CurrentWorld || ((CurrentWorld->GetTimeSeconds() - LastOcclusionRefresh) < OcclusionRefreshInterval && LastOcclusionRefresh != -1) )
	{
		return;
	}

	CalculateOcclusionValues(true);
}

void UAkComponent::CalculateOcclusionValues(bool CalledFromTick)
{
	LastOcclusionRefresh = GetWorld()->GetTimeSeconds();
	static FName NAME_SoundOcclusion = FName(TEXT("SoundOcclusion"));

	int32 NumListeners = 0;
	FAkAudioDevice * AkAudioDevice = FAkAudioDevice::Get();
	if( AkAudioDevice )
	{
		NumListeners = AkAudioDevice->GetNumListeners();
	}

	for(int32 ListenerIdx = 0; ListenerIdx < NumListeners; ListenerIdx++)
	{
		FHitResult OutHit;

		if( ListenerIdx >= ListenerOcclusionInfo.Num() )
		{
			ListenerOcclusionInfo.Add(FAkListenerOcclusion());
		}

		FVector ListenerPosition;
		AkAudioDevice = FAkAudioDevice::Get();
		if( AkAudioDevice )
		{
			ListenerPosition = AkAudioDevice->GetListenerPosition(ListenerIdx);
		}
		FVector SourcePosition = GetComponentLocation();
		UWorld* CurrentWorld = GetWorld();
		APlayerController* PlayerController = CurrentWorld ? CurrentWorld->GetFirstPlayerController() : NULL;
		FCollisionQueryParams CollisionParams(NAME_SoundOcclusion, true, GetOwner());
		if( PlayerController != NULL )
		{
			CollisionParams.AddIgnoredActor(PlayerController->GetPawn());
		}

		// CCP MOD BEGIN
		// NOTE (Siggi): Temp hack to get rid of error spam.  We need to track the cause of this causing an error in PhysX with AK:
		// For reference therror (in debug builds): NpSceneQueries.cpp 211) eINVALID_PARAMETER : PxScene::raycast(): maxDist is negative.
		bool bNowOccluded = false;// GetWorld()->LineTraceSingleByChannel(OutHit, SourcePosition, ListenerPosition, ECC_Visibility, CollisionParams);
		// CCP MOD END
		
		if( bNowOccluded )
		{
			FBox BoundingBox;

			if( OutHit.Actor.IsValid() )
			{
				BoundingBox = OutHit.Actor->GetComponentsBoundingBox();
			}
			else if( OutHit.Component.IsValid() )
			{
				BoundingBox = OutHit.Component->Bounds.GetBox();
			}

			// Translate the impact point to the bounding box of the obstacle
			TArray<FVector> Points;
			Points.Add(FVector(OutHit.ImpactPoint.X, BoundingBox.Min.Y, BoundingBox.Min.Z));
			Points.Add(FVector(OutHit.ImpactPoint.X, BoundingBox.Min.Y, BoundingBox.Max.Z));
			Points.Add(FVector(OutHit.ImpactPoint.X, BoundingBox.Max.Y, BoundingBox.Min.Z));
			Points.Add(FVector(OutHit.ImpactPoint.X, BoundingBox.Max.Y, BoundingBox.Max.Z));

			Points.Add(FVector(BoundingBox.Min.X, OutHit.ImpactPoint.Y, BoundingBox.Min.Z));
			Points.Add(FVector(BoundingBox.Min.X, OutHit.ImpactPoint.Y, BoundingBox.Max.Z));
			Points.Add(FVector(BoundingBox.Max.X, OutHit.ImpactPoint.Y, BoundingBox.Min.Z));
			Points.Add(FVector(BoundingBox.Max.X, OutHit.ImpactPoint.Y, BoundingBox.Max.Z));

			Points.Add(FVector(BoundingBox.Min.X, BoundingBox.Min.Y, OutHit.ImpactPoint.Z));
			Points.Add(FVector(BoundingBox.Min.X, BoundingBox.Max.Y, OutHit.ImpactPoint.Z));
			Points.Add(FVector(BoundingBox.Max.X, BoundingBox.Min.Y, OutHit.ImpactPoint.Z));
			Points.Add(FVector(BoundingBox.Max.X, BoundingBox.Max.Y, OutHit.ImpactPoint.Z));
				
			// Compute the number of "second order paths" that are also obstructed. This will allow us to approximate
			// "how obstructed" the source is.
			int32 NumObstructedPaths = 0;
			for(int32 PointIdx = 0; PointIdx < Points.Num(); PointIdx++)
			{
				FHitResult TempHit;
				bool bListenerToObstacle = GetWorld()->LineTraceSingleByChannel(TempHit, ListenerPosition, Points[PointIdx], ECC_Visibility, CollisionParams);
				bool bSourceToObstacle = GetWorld()->LineTraceSingleByChannel(TempHit, SourcePosition, Points[PointIdx], ECC_Visibility, CollisionParams);
				if(bListenerToObstacle || bSourceToObstacle)
				{
					NumObstructedPaths++;
				}
			}

			// Modulate occlusion by blocked secondary paths. 
			ListenerOcclusionInfo[ListenerIdx].TargetValue = (float)NumObstructedPaths / (float)Points.Num();

#define AK_DEBUG_OCCLUSION 0
#if AK_DEBUG_OCCLUSION
			// Draw bounding box and "second order paths"
			//UE_LOG(LogAkAudio, Log, TEXT("Target Occlusion level: %f"), ListenerOcclusionInfo[ListenerIdx].TargetValue);
			::FlushPersistentDebugLines(GetWorld());
			::FlushDebugStrings(GetWorld());
			::DrawDebugBox(GetWorld(), BoundingBox.GetCenter(), BoundingBox.GetExtent(), FColor::White, false, 4);
			::DrawDebugPoint(GetWorld(), ListenerPosition, 10.0f, FColor(0, 255, 0), false, 4);
			::DrawDebugPoint(GetWorld(), SourcePosition, 10.0f, FColor(0, 255, 0), false, 4);
			::DrawDebugPoint(GetWorld(), OutHit.ImpactPoint, 10.0f, FColor(0, 255, 0), false, 4);

			for(int32 i = 0; i < Points.Num(); i++)
			{
				::DrawDebugPoint(GetWorld(), Points[i], 10.0f, FColor(255, 255, 0), false, 4);
				::DrawDebugString(GetWorld(), Points[i], FString::Printf(TEXT("%d"), i), nullptr, FColor::White, 4);
				::DrawDebugLine(GetWorld(), Points[i], ListenerPosition, FColor::Cyan, false, 4);
				::DrawDebugLine(GetWorld(), Points[i], SourcePosition, FColor::Cyan, false, 4);
			}
			FColor LineColor = FColor::MakeRedToGreenColorFromScalar(1.0f - ListenerOcclusionInfo[ListenerIdx].TargetValue);
			::DrawDebugLine(GetWorld(), ListenerPosition, SourcePosition, LineColor, false, 4);
#endif // AK_DEBUG_OCCLUSION
		}
		else
		{
			ListenerOcclusionInfo[ListenerIdx].TargetValue = 0.0f;
		}

		if( !CalledFromTick )
		{
			ListenerOcclusionInfo[ListenerIdx].CurrentValue = ListenerOcclusionInfo[ListenerIdx].TargetValue;
			AkAudioDevice = FAkAudioDevice::Get();
			if( AkAudioDevice )
			{
				AkAudioDevice->SetOcclusionObstruction(this, ListenerIdx, 0.0f, ListenerOcclusionInfo[ListenerIdx].CurrentValue);
			}
		}
	}
}

void UAkComponent::ClearOcclusionValues()
{
	ClearingOcclusion = false;
	int32 NumListeners = 0;
	FAkAudioDevice * AkAudioDevice = FAkAudioDevice::Get();
	if (AkAudioDevice)
	{
		NumListeners = AkAudioDevice->GetNumListeners();
	}

	for (int32 ListenerIdx = 0; ListenerIdx < NumListeners && ListenerIdx < ListenerOcclusionInfo.Num(); ListenerIdx++)
	{
		ListenerOcclusionInfo[ListenerIdx].TargetValue = 0.0f;
		ClearingOcclusion |= ListenerOcclusionInfo[ListenerIdx].CurrentValue > 0.0f;
	}
}
