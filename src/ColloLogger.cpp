#include "ColloLogger.h"

#include <ctime>
#include <cstring>
#include <charconv>
#include <iostream>

ColloLogger::ColloLogger(const std::string& filePath)
    : mFilePath{ filePath}, mAppendIndex{},
    mWriteIndex{}, mLevel{ LogLevel::debug }
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

void ColloLogger::setLogLevel(const LogLevel& lvl)
{
    mLevel = lvl;
}

void ColloLogger::addCrit(const char* msg)
{
    size_t msgSize = strlen(msg);
    mLock.lock();

    if (BufferSize <= mAppendIndex + MinimalLogSize + msgSize) {
        swapBuffers();
        char* tempIndex = mAppendBuffer + mAppendIndex;
        std::to_chars_result result = std::to_chars(tempIndex, tempIndex + TimeSize, static_cast<int>(std::clock()));
        tempIndex = result.ptr;
    
        const char* level = " CRIT ";
        std::memcpy(tempIndex, level, LevelSize);
        tempIndex += LevelSize;
        
        std::memcpy(tempIndex, msg, msgSize);
        tempIndex += msgSize;
    
        *tempIndex++ = '\n';
    
        mAppendIndex = static_cast<size_t>(tempIndex - mAppendBuffer);
        mLock.unlock();
        write();
    }
    else {
        char* tempIndex = mAppendBuffer + mAppendIndex;
        std::to_chars_result result = std::to_chars(tempIndex, tempIndex + TimeSize, static_cast<int>(std::clock()));
        tempIndex = result.ptr;
    
        const char* level = " CRIT ";
        std::memcpy(tempIndex, level, LevelSize);
        tempIndex += LevelSize;
        
        std::memcpy(tempIndex, msg, msgSize);
        tempIndex += msgSize;
    
        *tempIndex++ = '\n';
    
        mAppendIndex = static_cast<size_t>(tempIndex - mAppendBuffer);
        mLock.unlock();
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
    std::unique_lock<std::mutex> lock(mFileLock);
    mFile << mWriteBuffer;
}
