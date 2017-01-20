
#include "AkAudioDevice.h"
#include "AkAudioEvent.h"
#include "HydraPersistentAudio.h"

#ifdef AK_SUPPORT_WCHAR
#define CONVERT_WWISE_STRING(S) (*(S))
#else
#define CONVERT_WWISE_STRING(S) (TCHAR_TO_ANSI(*(S)))
#endif

void UHydraPersistentAudio::PostEvent(UAkAudioEvent* audioEvent)
{
	auto objectID = GetRegisteredObject();
	if (objectID != AK_INVALID_GAME_OBJECT && audioEvent)
	{
		AK::SoundEngine::PostEvent(CONVERT_WWISE_STRING(audioEvent->GetName()), objectID, 0, nullptr, nullptr);
	}
}

void UHydraPersistentAudio::Stop()
{
	auto objectID = GetRegisteredObject();
	if (objectID != AK_INVALID_GAME_OBJECT)
	{
		AK::SoundEngine::StopAll(objectID);
	}
}

void UHydraPersistentAudio::SetRTPCValue(FString RTPCName, float value, int32 interpolationTimeMs)
{
	auto objectID = GetRegisteredObject();
	if (objectID != AK_INVALID_GAME_OBJECT)
	{
		AK::SoundEngine::SetRTPCValue(CONVERT_WWISE_STRING(RTPCName), value, objectID, interpolationTimeMs);
	}
}

void UHydraPersistentAudio::PostTrigger(FString trigger)
{
	auto objectID = GetRegisteredObject();
	if (objectID != AK_INVALID_GAME_OBJECT)
	{
		AK::SoundEngine::PostTrigger(CONVERT_WWISE_STRING(trigger), objectID);
	}
}

void UHydraPersistentAudio::SetSwitch(FString switchGroup, FString switchState)
{
	auto objectID = GetRegisteredObject();
	if (objectID != AK_INVALID_GAME_OBJECT)
	{
		AK::SoundEngine::SetSwitch(CONVERT_WWISE_STRING(switchGroup), CONVERT_WWISE_STRING(switchState), objectID);
	}
}

AkGameObjectID UHydraPersistentAudio::GetRegisteredObject()
{
	if (auto device = FAkAudioDevice::Get())
	{
		static auto objectID = AK_INVALID_GAME_OBJECT;

		if (objectID == AK_INVALID_GAME_OBJECT)
		{
			objectID = reinterpret_cast<AkGameObjectID>(&UHydraPersistentAudio::GetRegisteredObject); 
#ifndef AK_OPTIMIZED
			AK::SoundEngine::RegisterGameObj(objectID, "HydraPersistentAudio");
#else
			AK::SoundEngine::RegisterGameObj(objectID);
#endif // AK_OPTIMIZED
		}

		return objectID;
	}
	return AK_INVALID_GAME_OBJECT;
}
