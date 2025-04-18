#include "RingLogger.h"

#include <charconv>
#include <iostream>
#include <cassert>

#include "RingBuffer.h"

RingLogger::RingLogger(const std::string& filePath, const size_t& size)
    : mBuffer(size, filePath)
{
}

RingLogger::~RingLogger()
{
}

void RingLogger::addLog(const char* msg)
{
    size_t size = strlen(msg);
    
    assert(size > 256
        && "ColloLog::RingLogger Buffer overflow due to too big message.");

    char temp[256];
    char* tempIndex = temp;
    std::to_chars_result result = std::to_chars(tempIndex, tempIndex + TimeSize, static_cast<int>(std::clock()));
    tempIndex = result.ptr;

    const char* level = levelToCString(crit);
    std::memcpy(tempIndex, level, LevelSize);
    tempIndex += LevelSize;
    
    std::memcpy(tempIndex, msg, size);
    tempIndex += size;

    *tempIndex++ = '\n';

    {
        std::unique_lock<std::mutex> lock(mLocker);
        mBuffer.append(temp);
        if (mBuffer.isFull()) {
            mBuffer.flush();
        }
    }
}

void RingLogger::write()
{
    mBuffer.flushTail();
}
