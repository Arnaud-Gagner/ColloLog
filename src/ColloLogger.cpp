#include "ColloLogger.h"

#include <ctime>
#include <chrono>
#include <future>
#include <cstring>
#include <sstream>

#include <iostream>

ColloLogger::ColloLogger(const std::string& filePath)
    : mFilePath{ filePath}, mAppendIndex{}, mWriteIndex{}
{
    memset(mBuffer1, 0, BufferSize);
    memset(mBuffer2, 0, BufferSize);
    mAppendBuffer = mBuffer1;
    mWriteBuffer = mBuffer2;
    mFile.open(mFilePath, std::ios::app);
}

ColloLogger::~ColloLogger()
{
    swapBuffers();
    write();
    if (mFile.is_open()) {
        mFile.close();
    }
}

void ColloLogger::addLog(const std::string_view& msg)
{
    clock_t time = std::clock();
    size_t msgSize = msg.size();
    size_t size = MinimalLogSize + msgSize;

    mLock.lock();
    if (BufferSize <= mAppendIndex + size) {
        swapBuffers();
    }

    char* tempIndex = mAppendBuffer + mAppendIndex;
    std::to_chars_result result = std::to_chars(tempIndex, tempIndex + TimeSize, static_cast<int>(time));
    tempIndex = result.ptr;

    const char* level = " INFO ";
    std::memcpy(tempIndex, level, LevelSize);
    tempIndex += LevelSize;
    
    std::memcpy(tempIndex, msg.data(), msgSize);
    tempIndex += msgSize;

    *tempIndex++ = '\n';

    mAppendIndex = static_cast<size_t>(tempIndex - mAppendBuffer);
    mLock.unlock();

    if (BufferSize <= mAppendIndex + size) {
        write();
    }
}

void ColloLogger::swapBuffers()
{
    std::swap(mWriteBuffer, mAppendBuffer);
    mWriteIndex = mAppendIndex;
    mAppendIndex = 0;
}

void ColloLogger::write()
{
    std::async(std::launch::async, [this]() {
        mFileLock.lock();
        mFile.write(mWriteBuffer, mWriteIndex);
        mFileLock.unlock();
    });
}
