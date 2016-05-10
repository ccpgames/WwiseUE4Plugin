#include "AkAudioDevice.h"
#include "AkInputBuffer.h"

AkInputBuffer::AkInputBuffer() :
writeHead(0)
, readHead(0)
, readSize(0)
{
    Ring.AddUninitialized(SAMPLES);
}

AkInputBuffer::AkInputBuffer(const AkInputBuffer &source)
{
	Ring = source.Ring;
	writeHead = source.writeHead;
	writeSize = source.writeSize;
	readHead = source.readHead;
	readSize = source.readSize;
}

void AkInputBuffer::PushInput(const uint8 *InputBuffer, uint32 BufferSize)
{
    FScopeLock ScopeLock(&BufferCriticalSection);
    uint32 RemainingSize = BufferSize;
    const uint8 * Buffer = InputBuffer;
    while (RemainingSize > 0)
    {
        uint32 writeLength = FMath::Min<uint32>(SAMPLES - writeHead, RemainingSize);
        FMemory::Memcpy(&Ring[writeHead], Buffer, writeLength);
        readSize += writeLength;
        writeHead = (writeHead + writeLength) % SAMPLES;
        Buffer += writeLength;
        RemainingSize -= writeLength;

        //if we have overrun, drop samples
        if (writeHead > readHead && (writeHead - readHead) < readSize)
        {
            readSize -= writeHead - readHead;
            readHead = writeHead;
        }
    }
}

uint32 AkInputBuffer::PullInput(uint8 *OutputBuffer, uint32 BufferSize)
{
    FScopeLock ScopeLock(&BufferCriticalSection);
    uint32 RemainingSize = BufferSize;
    uint32 Written = 0;
    uint8 * Buffer = OutputBuffer;
    while (RemainingSize > 0 && readSize > 0)
    {
        uint32 readLength = FMath::Min<uint32>(FMath::Min<uint32>(readSize, RemainingSize), SAMPLES - readHead);
        FMemory::Memcpy(Buffer, &Ring[readHead], readLength);
        readSize -= readLength;
        RemainingSize -= readLength;
        readHead = (readHead + readLength) % SAMPLES;
        Buffer += readLength;
        Written += readLength;
    }
    return Written;
}

uint32 AkInputBuffer::PeekInput(uint8 *OutputBuffer, uint32 BufferSize)
{
    FScopeLock ScopeLock(&BufferCriticalSection);
    uint32 RemainingSize = BufferSize;
    uint32 Written = 0;
    uint8 * Buffer = OutputBuffer;
    uint32 ReadSize = readSize;
    uint32 ReadHead = readHead;

    while (RemainingSize > 0 && ReadSize > 0)
    {
        uint32 readLength = FMath::Min<uint32>(FMath::Min<uint32>(ReadSize, RemainingSize), SAMPLES - ReadHead);
        FMemory::Memcpy(Buffer, &Ring[ReadHead], readLength);
        ReadSize -= readLength;
        RemainingSize -= readLength;
        ReadHead = (ReadHead + readLength) % SAMPLES;
        Buffer += readLength;
        Written += readLength;
    }
    return Written;
}
