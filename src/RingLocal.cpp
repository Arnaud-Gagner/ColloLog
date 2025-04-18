#include "RingLocal.h"

#include <charconv>
#include <iostream>
#include <cassert>

#include "ThreadPool.h"

extern ThreadPool pool;

thread_local RingBuffer RingLocal::mBuffer(25,"../ColloLog/Logs/ringlocal.log");

RingLocal::RingLocal(const std::string& filePath, const size_t& size)
    : mFilePath{ filePath }
{
}

RingLocal::~RingLocal()
{
    if (mFile.is_open()) {
        mFile.close();
    }
}

void RingLocal::addLog(const char* msg)
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
    
    mBuffer.append(temp);
    if (mBuffer.isFull()) {
        write();
    }
}

void RingLocal::write()
{
    std::unique_lock<std::mutex> lock(mLock);
    mBuffer.flush();
}
