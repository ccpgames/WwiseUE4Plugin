
#pragma once

#include "AkAudioDevice.h"
#include "HydraPersistentAudio.generated.h"

UCLASS()
class AKAUDIO_API UHydraPersistentAudio : public UBlueprintFunctionLibrary
{
public:
	UFUNCTION(BlueprintCallable, Category = HydraPersistentAudio)
	static void PostEvent(class UAkAudioEvent* audioEvent);

	UFUNCTION(BlueprintCallable, Category = HydraPersistentAudio)
	static void Stop();

	UFUNCTION(BlueprintCallable, Category = HydraPersistentAudio)
	static void SetRTPCValue(FString RTPCName, float value, int32 interpolationTimeMs);

	UFUNCTION(BlueprintCallable, Category = HydraPersistentAudio)
	static void PostTrigger(FString trigger);

	UFUNCTION(BlueprintCallable, Category = HydraPersistentAudio)
	static void SetSwitch(FString switchGroup, FString switchState);

private:
	static AkGameObjectID GetRegisteredObject();

	GENERATED_BODY()
};
