#include "AkAudioDevice.h"
#include <AkInputManager.h>
#include <Voice.h>




AkInputManager* AkInputManager::Instance = nullptr;

AkInputManager& AkInputManager::Initialize(AkAudioInputPluginExecuteCallbackFunc in_pfnExecCallback,
    AkAudioInputPluginGetFormatCallbackFunc in_pfnGetFormatCallback,
    AkAudioInputPluginGetGainCallbackFunc in_pfnGetGainCallback 
    )
{
    SetAudioInputCallbacks(in_pfnExecCallback, in_pfnGetFormatCallback, in_pfnGetGainCallback);
    if (!Instance)
    {
        Instance = new AkInputManager;
    }
    return *Instance;
}

void AkInputManager::InitInputBank()
{
    FAkAudioDevice * AudioDevice = FAkAudioDevice::Get();
    if (AudioDevice)
    {
        AkUInt32 bankID;
        AudioDevice->LoadBank("Microphone.bnk", AK_DEFAULT_POOL_ID, bankID);
    }
}
void AkInputManager::TrackAudio(uint32 VoiceId, AkUInt32 PlayingId)
{
    PlayingMap.Add(PlayingId, VoiceId);
    AkInputBuffer& Input = InputMap.Add(VoiceId);
    Input.Ring.AddUninitialized(SAMPLES);
    Input.writeHead = 0;
    Input.readHead = 0;
    Input.readSize = 0;
}

void AkInputManager::QueueAudio(uint32 VoiceId, const uint8 *Buffer, uint32 BufferSize)
{
    AkInputBuffer* Input = InputMap.Find(VoiceId);
    if (Input)
    {
        FScopeLock ScopeLock(&Input->BufferCriticalSection);
        uint32 writeLength = FMath::Min<uint32>(SAMPLES - Input->writeHead, BufferSize);
        FMemory::Memcpy(&Input->Ring[Input->writeHead], Buffer, writeLength);
        Input->readSize += writeLength;
        Input->writeHead = (Input->writeHead + writeLength) % SAMPLES;
        if (writeLength < BufferSize)
        {
            writeLength = BufferSize - writeLength;
            FMemory::Memcpy(&Input->Ring[Input->writeHead], Buffer, writeLength);
            Input->readSize += writeLength;
            Input->writeHead += writeLength;
        }

        //if we have overrun, drop samples
        if (Input->writeHead > Input->readHead && (Input->writeHead - Input->readHead) < Input->readSize)
        {
            Input->readSize -= Input->writeHead - Input->readHead;
            Input->readHead = Input->writeHead;
        }
    }
}


void AkInputManager::Execute(AkPlayingID in_playingID, AkAudioBuffer *io_pBufferOut)
{
    AkInputManager::Get().ExecuteInternal(in_playingID, io_pBufferOut);
}

void AkInputManager::GetFormat(AkPlayingID in_playingID, AkAudioFormat& io_AudioFormat)
{
    AkInputManager::Get().GetFormatInternal(io_AudioFormat);
}

void AkInputCallback::Execute(AkPlayingID in_playingID, AkAudioBuffer *io_pBufferOut)
{
    AkInputManager::Execute(in_playingID, io_pBufferOut);
}

void AkInputCallback::GetFormat(AkPlayingID in_playingID, AkAudioFormat& io_AudioFormat)
{
    AkInputManager::GetFormat(in_playingID, io_AudioFormat);
}



void AkInputManager::ExecuteInternal(AkPlayingID		in_playingID, AkAudioBuffer*	io_pBufferOut)
{
    uint32* VoiceId = PlayingMap.Find(in_playingID);
    if (!VoiceId)
    {
        io_pBufferOut->eState = AK_NoDataReady;
        io_pBufferOut->uValidFrames = 0;
        return;
    }
    AkInputBuffer* Input = InputMap.Find(*VoiceId);
    if (!Input)
    {
        io_pBufferOut->eState = AK_NoDataReady;
        io_pBufferOut->uValidFrames = 0;
        return;
    }
    if (Input->readSize < 2)
    {
        io_pBufferOut->eState = AK_NoDataReady;
        io_pBufferOut->uValidFrames = 0;
        return;
    }
    FScopeLock ScopeLock(&Input->BufferCriticalSection);
    AkUInt32 uMaxFrames = io_pBufferOut->MaxFrames();
    AkInt8 * AK_RESTRICT pBufOut = (AkInt8*)(io_pBufferOut->GetChannel(0));
    while (Input->readSize > 0 && io_pBufferOut->uValidFrames < uMaxFrames)
    {
        uint32 readSize = FMath::Min<uint32>(Input->readSize, SAMPLES - Input->readHead);
        AkUInt32 BytesToCopy = FMath::Min<uint32>((uMaxFrames - io_pBufferOut->uValidFrames) * sizeof(uint16), readSize);
        FMemory::Memcpy(pBufOut, &Input->Ring[Input->readHead], BytesToCopy);
        io_pBufferOut->uValidFrames += (BytesToCopy / sizeof(AkInt16));
        pBufOut += BytesToCopy;
        Input->readHead = (Input->readHead + BytesToCopy) % SAMPLES;
        Input->readSize -= BytesToCopy;
    }

    if (io_pBufferOut->uValidFrames > 0)
        io_pBufferOut->eState = AK_DataReady;
    else
        io_pBufferOut->eState = AK_NoDataReady;

}

void AkInputManager::GetFormatInternal(
    AkAudioFormat&  io_AudioFormat  // Already filled format, modify it if required.
    )
{
    io_AudioFormat.SetAll(
        VOICE_SAMPLE_RATE,// Sample rate
        AK_SPEAKER_SETUP_MONO,
        16,						// Bits per samples
        2,						// 2 bytes per samples
        AK_INT,					// feeding integers(signed)
        AK_NONINTERLEAVED
        );
}


namespace AkInputTracking
{
    void TrackAudio(uint32  VoiceId, AkUInt32 PlayingId)
    {
        AkInputManager::Get().TrackAudio(VoiceId, PlayingId);
    }

    void QueueAudio(uint32 VoiceId, const uint8 *Buffer, uint32 BufferSize)
    {
        AkInputManager::Get().QueueAudio(VoiceId, Buffer, BufferSize);
    }
    void PlayVoice(uint32  VoiceId, class UAkComponent* AkComponent)
    {
        //UWorld* CurrentWorld = AkComponent->GetWorld();
        FString EventName = "Play_Microphone";

        //if (CurrentWorld->AllowAudioPlayback() && FAkAudioDevice::Get())
        {
#ifndef AK_SUPPORT_WCHAR
            ANSICHAR* szEventName = TCHAR_TO_ANSI(*EventName);
#else
            const WIDECHAR * szEventName = *EventName;
#endif
            AkPlayingID PlayingId = AK::SoundEngine::PostEvent(szEventName, (AkGameObjectID)AkComponent);
            if (PlayingId == AK_INVALID_PLAYING_ID)
            {
                UE_LOG(LogAkAudio,
                    Error, TEXT("Failed to Post Voice event."));
            }
            TrackAudio(VoiceId, PlayingId);
        }

    }
}
