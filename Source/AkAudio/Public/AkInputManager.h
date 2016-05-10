
/*=============================================================================
	AkAudioDevice.h: Audiokinetic audio interface object.
=============================================================================*/

#pragma once

/*------------------------------------------------------------------------------------
	AkAudioDevice system headers
------------------------------------------------------------------------------------*/

#include "AkAudioDevice.h"
#include "AkInputBuffer.h"
#include <Containers/Array.h>
#include <Containers/Map.h>

#define M_PI  3.14159265358979323846
#define FREQUENCY 440.0 // that extra ".0" is important!



class AKAUDIO_API AkInputManager
{
    static const int SAMPLES = 44100;
    int data[SAMPLES];

public:
    AkInputManager()
    {
        const double PERIOD = 1 / FREQUENCY;
        const double TIME_STEP = 1 / double(SAMPLES);

        int data[SAMPLES];
        double time = 0;
        for (int i = 0; i < SAMPLES; ++i) 
        {
            double angle = (2 * M_PI / PERIOD) * time;
            double factor = 0.5 * (sin(angle) + 1); // convert range that sin returns from [-1, 1] to [0, 1]

            //
            // factor is in the range [0, 1]
            // set the current sample to 2^(16-1) * factor
            // (since we're dealing with 16-bit PCM)
            // for a quieter wave, change 32768 to some
            // other maximum amplitude.
            //
            int x = int(32768 * factor);
            data[i] = x;
            time += TIME_STEP;
        }
    };
    static AkInputManager& Initialize(AkAudioInputPluginExecuteCallbackFunc in_pfnExecCallback,
        AkAudioInputPluginGetFormatCallbackFunc in_pfnGetFormatCallback = NULL, // Optional
        AkAudioInputPluginGetGainCallbackFunc in_pfnGetGainCallback = NULL      // Optional
        );

    static AkInputManager& Get()
    {
        if (!Instance)
        {
            SetAudioInputCallbacks(Execute, GetFormat);
            Instance = new AkInputManager;
        }
        return *Instance;
    }

    static void InitInputBank();

    void TrackAudio(uint32  VoiceId, AkUInt32 PlayingId);
    void QueueAudio(uint32 VoiceId, const uint8 *Buffer, uint32 BufferSize);
 public:
    static void Execute(AkPlayingID in_playingID, AkAudioBuffer *io_pBufferOut);
    static void GetFormat(AkPlayingID in_playingID, AkAudioFormat& io_AudioFormat);



private:
    ~AkInputManager(){};
    static AkInputManager* Instance;

    TMap<uint32, AkInputBuffer> InputMap;
    TMap<uint32, uint32> PlayingMap;

    void ExecuteInternal(AkPlayingID in_playingID, AkAudioBuffer *io_pBufferOut);
    void GetFormatInternal(AkAudioFormat& io_AudioFormat);


};

namespace AkInputCallback
{
    void Execute(AkPlayingID in_playingID, AkAudioBuffer *io_pBufferOut);
    void GetFormat(AkPlayingID in_playingID, AkAudioFormat& io_AudioFormat);
}

namespace AkInputTracking
{
    void AKAUDIO_API TrackAudio(uint32  VoiceId, AkUInt32 PlayingId);
    void AKAUDIO_API QueueAudio(uint32 VoiceId, const uint8 *Buffer, uint32 BufferSize);

    void AKAUDIO_API PlayVoice(uint32  VoiceId, class UAkComponent* AkComponent);

}
