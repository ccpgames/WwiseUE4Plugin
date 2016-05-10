#pragma once

#include "AkAudioDevice.h"

class AKAUDIO_API AkInputBuffer
{

public:
    TArray<uint8> Ring;
    uint32 writeHead;
    uint32 writeSize;
    uint32 readHead;
    uint32 readSize;
    FCriticalSection BufferCriticalSection;
    static const int SAMPLES = 96000;
public:
    AkInputBuffer();
	AkInputBuffer(const AkInputBuffer &source);

    void PushInput(const uint8 *InputBuffer, uint32 BufferSize);

    uint32 PullInput(uint8 *OutputBuffer, uint32 BufferSize);
    uint32 PeekInput(uint8 *OutputBuffer, uint32 BufferSize);

};
