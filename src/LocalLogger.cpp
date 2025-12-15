#include "ColloLog/LocalLogger.h"

#include <charconv>
#include <cstring>

thread_local char LocalLogger::mBuffer1[LocalLogger::BufferSize];

thread_local char* LocalLogger::mAppendBuffer = LocalLogger::mBuffer1;

thread_local unsigned int LocalLogger::mAppendIndex = 0;
thread_local LogLevel LocalLogger::mLevel = LogLevel::Debug;

LocalLogger::LocalLogger(const std::string& filepath, FileOpen mode)
  : mFilePath{ filepath }
{
    mAppendBuffer = mBuffer1;

    switch (mode) {
        case FileOpen::Clear: {
            mFile.open(mFilePath, std::ios::trunc);
            break;
        }
        default: {
            mFile.open(mFilePath, std::ios::app);
            break;
        }
    }
}

LocalLogger::~LocalLogger()
{
    swapBuffers();
    std::this_thread::sleep_for(std::chrono::seconds(2));
    if (mFile.is_open()) {
        mFile.close();
    }
}

void LocalLogger::setlevel(const LogLevel& lvl)
{
    mLevel = lvl;
}

void LocalLogger::crit(const char* msg, FlushStrat strat)
{
    size_t msgSize = strlen(msg);
    if (BufferSize <= mAppendIndex + MinimalLogSize + msgSize) {
        swapBuffers();
    }

    addLog(msgSize, msg, LogLevel::Crit);

    switch (strat) {
        case FlushStrat::ManualAsync: {
            [[fallthrough]];
        }
        case FlushStrat::ManualSameThread: {
            swapBuffers();
            break;
        }
        default: {
            break;
        }
    }
}

void LocalLogger::debug(const char* msg, FlushStrat strat)
{
    if (mLevel > LogLevel::Debug) {
        return;
    }

    size_t msgSize = strlen(msg);
    if (BufferSize <= mAppendIndex + MinimalLogSize + msgSize) {
        swapBuffers();
    }
    addLog(msgSize, msg, LogLevel::Debug);

    switch (strat) {
        case FlushStrat::ManualAsync: {
            [[fallthrough]];
        }
        case FlushStrat::ManualSameThread: {
            swapBuffers();
            break;
        }
        default: {
            break;
        }
    }
}

void LocalLogger::info(const char* msg, FlushStrat strat)
{
    if (mLevel > LogLevel::Info) {
        return;
    }

    const size_t msgSize = strlen(msg);
    if (BufferSize <= mAppendIndex + MinimalLogSize + msgSize) {
        swapBuffers();
    }
    addLog(msgSize, msg, LogLevel::Info);

    switch (strat) {
        case FlushStrat::ManualAsync: {
            [[fallthrough]];
        }
        case FlushStrat::ManualSameThread: {
            flush();
            break;
        }
        default: {
            break;
        }
    }
}

void LocalLogger::warn(const char* msg, FlushStrat strat)
{
    if (mLevel > LogLevel::Warn) {
        return;
    }

    size_t msgSize = strlen(msg);
    if (BufferSize <= mAppendIndex + MinimalLogSize + msgSize) {
        swapBuffers();
    }
    addLog(msgSize, msg, LogLevel::Warn);

    switch (strat) {
        case FlushStrat::ManualAsync: {
            [[fallthrough]];
        }
        case FlushStrat::ManualSameThread: {
            flush();
            break;
        }
        default: {
            break;
        }
    }
}

void LocalLogger::flush()
{
    swapBuffers();
}

void LocalLogger::clear()
{
    std::unique_lock<std::mutex> lock(mLock);
    if (mFile.is_open()) {
        mFile.close();
    }
    mFile.open(mFilePath, std::ios::trunc);
}

void LocalLogger::addLog(const size_t& size, const char* msg, const LogLevel& lvl)
{
    char* message = mAppendBuffer + mAppendIndex;
    std::to_chars_result result = std::to_chars(message, message + TimeSize, __rdtsc());
    message = result.ptr;

    const char* level = levelToCString(lvl);
    std::memcpy(message, level, LevelSize);
    message += LevelSize;

    std::memcpy(message, msg, size);
    message += size;
    *message++ = '\n';
    mAppendIndex = static_cast<unsigned int>(message - mAppendBuffer);
}

void LocalLogger::swapBuffers()
{
    write(mAppendBuffer, mAppendIndex);
    mAppendIndex = 0;
}

void LocalLogger::write(const char* data, const size_t size)
{
    std::unique_lock<std::mutex> lock(mLock);
    mFile.write(data, size);
}
