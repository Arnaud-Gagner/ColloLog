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

void ColloLogger::addLog(const std::string& msg)
{
    std::string message = std::to_string(static_cast<int>(std::clock()));
    message += " INFO ";
    message += msg;
    message += '\n';

    size_t size = message.size();

    mLock.lock();
    if (BufferSize <= mAppendIndex + size) {
        swapBuffers();
    }
    std::memcpy(mAppendBuffer + mAppendIndex, message.c_str(), size);
    mAppendIndex += size;
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
