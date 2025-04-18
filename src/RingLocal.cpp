#include "RingLocal.h"

#include <charconv>
#include <iostream>
#include "ThreadPool.h"

extern ThreadPool pool;

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
    /*
    size_t size = strlen(msg);

    char temp[1024];
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
    write();*/
}

void RingLocal::write()
{
    /*
    std::unique_lock<std::mutex> lock(mLock);
    mBuffer.flush();*/
}