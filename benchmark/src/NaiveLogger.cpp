#include "NaiveLogger.h"

#include <cassert>
#include <chrono>
#include <direct.h>
#include <iostream>
#include <string>

NaiveLogger::NaiveLogger(const std::string& filepath)
  : mFilePath{ filepath }
  , mCurrentIndex{}
  , mLevel{ Debug }
{
    mFile.open(mFilePath, std::ios::trunc);
    mFile.close();
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
    auto dateTime = std::chrono::time_point_cast<std::chrono::milliseconds>(
      std::chrono::system_clock::now());
    std::string message{ '[' + std::to_string(dateTime.time_since_epoch().count()) + ']'
                         + "CRIT" + " " + msg + '\n' };
    addLog(message);
}

void NaiveLogger::addDebug(const std::string& msg)
{
    if (mLevel > LogLevel::Debug) {
        return;
    }

    auto dateTime = std::chrono::time_point_cast<std::chrono::milliseconds>(
      std::chrono::system_clock::now());
    std::string message{ '[' + std::to_string(dateTime.time_since_epoch().count()) + ']'
                         + "DEBUG" + " " + msg + '\n' };
    addLog(message);
}

void NaiveLogger::addInfo(const std::string& msg)
{
    if (mLevel > LogLevel::Info) {
        return;
    }

    auto dateTime = std::chrono::time_point_cast<std::chrono::milliseconds>(
      std::chrono::system_clock::now());
    std::string message{ '[' + std::to_string(dateTime.time_since_epoch().count()) + ']'
                         + "INFO" + " " + msg + '\n' };
    addLog(message);
}

void NaiveLogger::addWarn(const std::string& msg)
{
    if (mLevel > LogLevel::Warn) {
        return;
    }

    auto dateTime = std::chrono::time_point_cast<std::chrono::milliseconds>(
      std::chrono::system_clock::now());
    std::string message{ '[' + std::to_string(dateTime.time_since_epoch().count()) + ']'
                         + "WARN" + " " + msg + '\n' };
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
    mFile.open(mFilePath, std::ios::app);
    mFile.write(mBuffer, mCurrentIndex);
    mFile.close();
    mCurrentIndex = 0;
}
