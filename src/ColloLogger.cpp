#include "ColloLog/ColloLogger.h"

#include <charconv>
#include <cstring>
#include <ctime>
#include <iostream>

#include "ColloLog/ThreadPool.h"

extern ThreadPool pool;

ColloLogger::ColloLogger(const std::string& filePath, OpenStrat strat)
  : mFilePath{ filePath }
  , mAppendIndex{}
  , mWriteIndex{}
  , mLevel{ LogLevel::Debug }
{
    memset(mBuffer1, 0, BufferSize);
    memset(mBuffer2, 0, BufferSize);
    mAppendBuffer = mBuffer1;
    mWriteBuffer = mBuffer2;

    switch (strat) {
        case OpenStrat::Clear: {
            mFile.open(mFilePath, std::ios::trunc);
            break;
        }
        default: {
            mFile.open(mFilePath, std::ios::app);
            break;
        }
    }
}

ColloLogger::~ColloLogger()
{
    std::unique_lock<std::mutex> lock(mLock);
    flushNow();
    std::this_thread::sleep_for(std::chrono::seconds(2));
    if (mFile.is_open()) {
        mFile.close();
    }
}

void ColloLogger::setLogLevel(const LogLevel& lvl)
{
    mLevel = lvl;
}

void ColloLogger::crit(const char* msg, FlushStrat strat)
{
    switch (strat) {
        case FlushStrat::AutoAsync: {
            autoAsyncFlush(msg, LogLevel::Crit);
            break;
        }
        case FlushStrat::AutoSameThread: {
            autoSameThreadFlush(msg, LogLevel::Crit);
            break;
        }
        case FlushStrat::ManualAsync: {
            manualAsyncFlush(msg, LogLevel::Crit);
            break;
        }
        default: {
            manualSameThreadFlush(msg, LogLevel::Crit);
            break;
        }
    }
}

void ColloLogger::debug(const char* msg, FlushStrat strat)
{
    if (mLevel > LogLevel::Debug) {
        return;
    }

    size_t msgSize = strlen(msg);
    mLock.lock();

    if (BufferSize <= mAppendIndex + MinimalLogSize + msgSize) {
        swapBuffers();
        addLog(msgSize, msg, LogLevel::Debug);
        mLock.unlock();
        pool.addTask([this] { write(); });
    }
    else {
        addLog(msgSize, msg, LogLevel::Debug);
        mLock.unlock();
    }
}

void ColloLogger::info(const char* msg, FlushStrat strat)
{
    if (mLevel > LogLevel::Info) {
        return;
    }

    size_t msgSize = strlen(msg);
    mLock.lock();

    if (BufferSize <= mAppendIndex + MinimalLogSize + msgSize) {
        swapBuffers();
        addLog(msgSize, msg, LogLevel::Info);
        mLock.unlock();
        pool.addTask([this] { write(); });
    }
    else {
        addLog(msgSize, msg, LogLevel::Info);
        mLock.unlock();
    }
}

void ColloLogger::warn(const char* msg, FlushStrat strat)
{
    if (mLevel > LogLevel::Warn) {
        return;
    }

    size_t msgSize = strlen(msg);
    mLock.lock();

    if (BufferSize <= mAppendIndex + MinimalLogSize + msgSize) {
        swapBuffers();
        addLog(msgSize, msg, LogLevel::Warn);
        mLock.unlock();
        pool.addTask([this] { write(); });
    }
    else {
        addLog(msgSize, msg, LogLevel::Warn);
        mLock.unlock();
    }
}

void ColloLogger::flush()
{
    swapBuffers();
    write();
}

void ColloLogger::addLog(const size_t size, const char* msg, const LogLevel& lvl)
{
    char* message = mAppendBuffer + mAppendIndex;
    std::to_chars_result result
      = std::to_chars(message, message + TimeSize, static_cast<int>(std::clock()));
    message = result.ptr;

    const char* level = levelToCString(lvl);
    std::memcpy(message, level, LevelSize);
    message += LevelSize;

    std::memcpy(message, msg, size);
    message += size;

    *message++ = '\n';
    mAppendIndex = static_cast<unsigned int>(message - mAppendBuffer);
}

void ColloLogger::swapBuffers()
{
    std::swap(mWriteBuffer, mAppendBuffer);
    mWriteIndex = mAppendIndex;
    mAppendIndex = 0;
}

void ColloLogger::flushNow()
{
    std::unique_lock<std::mutex> lock(mFileLock);
    mFile << mAppendBuffer;
    mAppendIndex = 0;
}

void ColloLogger::flushMessage(size_t size, const char* msg, LogLevel level)
{
    std::unique_lock<std::mutex> lock(mFileLock);

    char* message = mWriteBuffer;
    std::to_chars_result result
      = std::to_chars(message, message + TimeSize, static_cast<int>(std::clock()));
    message = result.ptr;

    const char* lvl = levelToCString(level);
    std::memcpy(message, lvl, LevelSize);
    message += LevelSize;

    std::memcpy(message, msg, size);
    message += size;
    *message++ = '\n';

    mFile << message;
}

void ColloLogger::write()
{
    std::unique_lock<std::mutex> lock(mFileLock);
    mFile << mWriteBuffer;
}

void ColloLogger::autoAsyncFlush(const char* msg, LogLevel level)
{
    size_t msgSize = strlen(msg);
    mLock.lock();
    if (BufferSize <= mAppendIndex + MinimalLogSize + msgSize) {
        swapBuffers();
        addLog(msgSize, msg, level);
        mLock.unlock();
        pool.addTask([this] { write(); });
    }
    else {
        addLog(msgSize, msg, level);
        mLock.unlock();
    }
}

void ColloLogger::autoSameThreadFlush(const char* msg, LogLevel level)
{
    size_t msgSize = strlen(msg);
    std::unique_lock<std::mutex> lock(mLock);

    if (BufferSize <= mAppendIndex + MinimalLogSize + msgSize) {
        flushNow();
    }
    addLog(msgSize, msg, level);
}

void ColloLogger::manualAsyncFlush(const char* msg, LogLevel level)
{
    size_t msgSize = strlen(msg);
    mLock.lock();

    if (BufferSize <= mAppendIndex + MinimalLogSize + msgSize) {
        swapBuffers();
        mLock.unlock();
        pool.addTask([this, msgSize, msg, level] {
            write();
            flushMessage(msgSize, msg, level);
        });
    }
    else {
        addLog(msgSize, msg, level);
        swapBuffers();
        mLock.unlock();
        pool.addTask([this] { write(); });
    }
}

void ColloLogger::manualSameThreadFlush(const char* msg, LogLevel level)
{
    size_t msgSize = strlen(msg);
    std::unique_lock<std::mutex> lock(mLock);

    if (BufferSize <= mAppendIndex + MinimalLogSize + msgSize) {
        flushNow();
        flushMessage(msgSize, msg, level);
    }
    else {
        addLog(msgSize, msg, level);
        flushNow();
    }
}