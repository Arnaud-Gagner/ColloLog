#include "LocalLogger.h"

#include <cassert>
#include <chrono>
#include <direct.h>
#include <iostream>
#include <string>

thread_local char LocalLogger::mBuffer1[LocalLogger::BufferSize];
thread_local char LocalLogger::mBuffer2[LocalLogger::BufferSize];

thread_local char* LocalLogger::mCurrentBuffer = LocalLogger::mBuffer1;
thread_local char* LocalLogger::mNextBuffer = LocalLogger::mBuffer2;

thread_local int LocalLogger::mCurrentIndex = 0;
thread_local int LocalLogger::mNextIndex = 0;

LocalLogger::LocalLogger(const std::string& filepath)
    : mFilePath{ filepath }
{
    mCurrentBuffer = mBuffer1;
    mNextBuffer = mBuffer2;
}

LocalLogger::~LocalLogger()
{
    writeLogs();
    if (mFile.is_open()) {
        mFile.close();
    }
}

void LocalLogger::addLog(const LogLevel& lvl, const std::string& msg)
{
    auto time = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
    std::string message{
        '[' + std::to_string(time.time_since_epoch().count()) + ']' + levelToString(lvl) + " " + msg + '\n'
    };

    size_t messageSize = message.size();

    if (BufferSize < mCurrentIndex + messageSize) {
        std::memcpy(mNextBuffer + mNextIndex, message.c_str(), messageSize);
        mNextIndex += messageSize;
        writeLogs();
    }
    else {
        std::memcpy(mCurrentBuffer + mCurrentIndex, message.c_str(), messageSize);
        mCurrentIndex += messageSize;
    }
}

void LocalLogger::swapBuffers()
{
    std::swap(mCurrentBuffer, mNextBuffer);
    mCurrentIndex = mNextIndex;
    mNextIndex = 0;
}

void LocalLogger::writeLogs()
{
    // TODO: add lock-free protection
    mLock.lock();

    mFile.open(mFilePath, std::fstream::app);
    mFile.write(mCurrentBuffer, mCurrentIndex);
    mFile.close();
    mCurrentIndex = 0;

    mLock.unlock();

    swapBuffers();
}
