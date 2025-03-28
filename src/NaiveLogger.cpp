#include "NaiveLogger.h"

#include <cassert>
#include <chrono>
#include <direct.h>
#include <iostream>
#include <string>

NaiveLogger::NaiveLogger(const std::string& filepath)
    : mFilePath{ filepath }, mCurrentIndex{}
{
    memset(mBuffer, 0, BufferSize);
}

NaiveLogger::~NaiveLogger()
{
    writeLogs();
    if (mFile.is_open()) {
        mFile.close();
    }
}

void NaiveLogger::addLog(const LogLevel& lvl, const std::string& msg)
{
    auto dateTime = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
    std::string message{
        '[' + std::to_string(dateTime.time_since_epoch().count()) + ']' + levelToString(lvl) + " " + msg + '\n'
    };

    size_t messageSize = message.size();
    if (BufferSize < mCurrentIndex + messageSize) {
        writeLogs();
    }
    if (BufferSize < mCurrentIndex) {
        assert(("Log message exceeds buffer size"));
    }
    std::memcpy(mBuffer + mCurrentIndex, message.c_str(), messageSize);
    mCurrentIndex += messageSize;
}

void NaiveLogger::writeLogs()
{
    mLock.lock();

    mFile.open(mFilePath, std::fstream::app);
    mFile.write((char*)&mBuffer, mCurrentIndex);
    mFile.close();
    mCurrentIndex = 0;

    mLock.unlock();
}
