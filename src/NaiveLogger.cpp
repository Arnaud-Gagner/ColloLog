#include "NaiveLogger.h"

#include <cassert>
#include <chrono>
#include <direct.h>
#include <iostream>
#include <string>

NaiveLogger::NaiveLogger(const std::string& filepath)
    : mFilePath{ filepath }, mCurrentIndex{}, mLevel{ debug }
{
    memset(mBuffer, 0, BufferSize);
}

NaiveLogger::~NaiveLogger()
{
    write();
    if (mFile.is_open()) {
        mFile.close();
    }
}

void NaiveLogger::setLogLevel(const LogLevel& lvl)
{
    std::unique_lock<std::mutex> lock(mLock);
    mLevel = lvl;
}

void NaiveLogger::addCrit(const std::string& msg)
{
    auto dateTime = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
    std::string message{
        '[' + std::to_string(dateTime.time_since_epoch().count()) + ']' + levelToString(crit) + " " + msg + '\n'
    };
    addLog(message);
}

void NaiveLogger::addDebug(const std::string& msg)
{
    if (mLevel > debug) { return; }

    auto dateTime = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
    std::string message{
        '[' + std::to_string(dateTime.time_since_epoch().count()) + ']' + levelToString(debug) + " " + msg + '\n'
    };
    addLog(message);
}

void NaiveLogger::addInfo(const std::string& msg)
{
    if (mLevel > info) { return; }

    auto dateTime = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
    std::string message{
        '[' + std::to_string(dateTime.time_since_epoch().count()) + ']' + levelToString(info) + " " + msg + '\n'
    };
    addLog(message);
}

void NaiveLogger::addWarn(const std::string& msg)
{
    if (mLevel > warn) { return; }
    
    auto dateTime = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
    std::string message{
        '[' + std::to_string(dateTime.time_since_epoch().count()) + ']' + levelToString(warn) + " " + msg + '\n'
    };
    addLog(message);
}

void NaiveLogger::addLog(const std::string& msg)
{
    size_t messageSize = msg.size();
    mLock.lock();
    if (BufferSize < mCurrentIndex + messageSize) {
        write();
    }
    if (BufferSize < mCurrentIndex) {
        assert(("Log message exceeds buffer size"));
    }
    
    std::memcpy(mBuffer + mCurrentIndex, msg.c_str(), messageSize);
    mCurrentIndex += static_cast<int>(messageSize);
    mLock.unlock();
}

void NaiveLogger::write()
{
    mFile.open(mFilePath, std::fstream::app);
    mFile.write(mBuffer, mCurrentIndex);
    mFile.close();
    mCurrentIndex = 0;
}
