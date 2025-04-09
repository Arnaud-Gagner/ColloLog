#include "ColloLogger.h"

#include <chrono>
#include <future>
#include <cstring>

#include <iostream>

ColloLogger::ColloLogger(const std::string& filePath)
    : mFilePath{ filePath}, mAppendIndex{}, mWriteIndex{}
{
    memset(mBuffer1, 0, BufferSize);
    memset(mBuffer2, 0, BufferSize);
    mAppendBuffer = mBuffer1;
    mWriteBuffer = mBuffer2;
}

ColloLogger::~ColloLogger()
{
    swapBuffers();
    write();
    if (mFile.is_open()) {
        mFile.close();
    }
}

void ColloLogger::addLog(const LogLevel& lvl, const std::string& msg)
{
    auto time = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
    std::string message = "[";
    message += std::to_string(time.time_since_epoch().count());
    message += ']';
    message += levelToString(lvl);
    message += ' ';
    message += msg;
    message += '\n';
    size_t messageSize = message.size();
    const char* cstr = message.c_str();

    if (BufferSize <= mAppendIndex + messageSize) {
        mLock.lock();
        swapBuffers();
        for (int i = 0; i < messageSize; i++) {
            mAppendBuffer[mAppendIndex+i] = message[i];
        }
        mAppendIndex += messageSize;
        mLock.unlock(); 
        write();
    }
    else {
        mLock.lock();
        for (int i = 0; i < messageSize; i++) {
            mAppendBuffer[mAppendIndex+i] = message[i];
        }
        mAppendIndex += messageSize;
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
    std::async(std::launch::async, [this]() {
        mFileLock.lock();
        mFile.open(mFilePath, std::ios::app);
        mFile.write(mWriteBuffer, mWriteIndex);
        mFile.close();
        mFileLock.unlock();
    });
}
