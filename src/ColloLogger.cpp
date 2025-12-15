#include "ColloLog/ColloLogger.h"

#include <charconv>
#include <cstring>
#include <ctime>
#include <iostream>

#include "CollogUtils.h"

ColloLogger::ColloLogger(const std::string& filePath, FileOpen mode)
  : mFilePath{ filePath }
  , mAppendIndex{}
  , mWriteIndex{}
  , mLevel{ LogLevel::Debug }
  , mIsThreadRunning{ true }
{
    mAppendBuffer = mBuffer1;
    mWriteBuffer = mBuffer2;

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

    mWriteThread = std::thread(&ColloLogger::threadLoop, this);
}

ColloLogger::~ColloLogger()
{
    std::unique_lock<std::mutex> lock(mLock);
    mIsThreadRunning = false;
    mTaskNotifier.notify_all();

    std::this_thread::sleep_for(std::chrono::seconds(2));

    mWriteThread.join();
    flushNow();

    if (mFile.is_open()) {
        mFile.close();
    }
}

void ColloLogger::setLevel(const LogLevel& lvl)
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

    switch (strat) {
        case FlushStrat::AutoAsync: {
            autoAsyncFlush(msg, LogLevel::Debug);
            break;
        }
        case FlushStrat::AutoSameThread: {
            autoSameThreadFlush(msg, LogLevel::Debug);
            break;
        }
        case FlushStrat::ManualAsync: {
            manualAsyncFlush(msg, LogLevel::Debug);
            break;
        }
        default: {
            manualSameThreadFlush(msg, LogLevel::Debug);
            break;
        }
    }
}

void ColloLogger::info(const char* msg, FlushStrat strat)
{
    if (mLevel > LogLevel::Info) {
        return;
    }

    switch (strat) {
        case FlushStrat::AutoAsync: {
            autoAsyncFlush(msg, LogLevel::Info);
            break;
        }
        case FlushStrat::AutoSameThread: {
            autoSameThreadFlush(msg, LogLevel::Info);
            break;
        }
        case FlushStrat::ManualAsync: {
            manualAsyncFlush(msg, LogLevel::Info);
            break;
        }
        default: {
            manualSameThreadFlush(msg, LogLevel::Info);
            break;
        }
    }
}

void ColloLogger::warn(const char* msg, FlushStrat strat)
{
    if (mLevel > LogLevel::Warn) {
        return;
    }

    switch (strat) {
        case FlushStrat::AutoAsync: {
            autoAsyncFlush(msg, LogLevel::Warn);
            break;
        }
        case FlushStrat::AutoSameThread: {
            autoSameThreadFlush(msg, LogLevel::Warn);
            break;
        }
        case FlushStrat::ManualAsync: {
            manualAsyncFlush(msg, LogLevel::Warn);
            break;
        }
        default: {
            manualSameThreadFlush(msg, LogLevel::Warn);
            break;
        }
    }
}

void ColloLogger::flush()
{
    swapBuffers();
    write();
}

void ColloLogger::clear()
{
    std::unique_lock<std::mutex> lock(mFileLock);
    if (mFile.is_open()) {
        mFile.close();
    }
    mFile.open(mFilePath, std::ios::trunc);
}

void ColloLogger::addLog(const size_t size, const char* msg, const LogLevel& lvl)
{
    char* message = mAppendBuffer + mAppendIndex;
    auto result = std::to_chars(message, message + TimeSize, collogUtils::rdtsc());
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
    mFile.write(mAppendBuffer, mAppendIndex);
    mAppendIndex = 0;
}

void ColloLogger::flushMessage(size_t size, const char* msg, LogLevel level)
{
    std::unique_lock<std::mutex> lock(mFileLock);

    char* message = mWriteBuffer;
    auto result = std::to_chars(message, message + TimeSize, collogUtils::rdtsc());
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
    mFile.write(mWriteBuffer, mWriteIndex);
}

void ColloLogger::task(const std::function<void()>& task)
{
    {
        std::unique_lock<std::mutex> lock(mTaskLock);
        mTasks.emplace(task);
    }
    mTaskNotifier.notify_one();
}

void ColloLogger::threadLoop()
{
    while (mIsThreadRunning) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(mTaskLock);
            mTaskNotifier.wait(lock,
                               [this] { return !mTasks.empty() || !mIsThreadRunning; });
            if (!mTasks.empty()) {
                task = std::move(mTasks.front());
                mTasks.pop();
            }
        }
        if (task) {
            task();
        }
    }
}

void ColloLogger::autoAsyncFlush(const char* msg, LogLevel level)
{
    const size_t msgSize = std::strlen(msg);
    mLock.lock();
    if (BufferSize <= mAppendIndex + MinimalLogSize + msgSize) {
        swapBuffers();
        addLog(msgSize, msg, level);
        mLock.unlock();
        task([this] { write(); });
    }
    else {
        addLog(msgSize, msg, level);
        mLock.unlock();
    }
}

void ColloLogger::autoSameThreadFlush(const char* msg, LogLevel level)
{
    const size_t msgSize = std::strlen(msg);
    std::unique_lock<std::mutex> lock(mLock);

    if (BufferSize <= mAppendIndex + MinimalLogSize + msgSize) {
        flushNow();
    }
    addLog(msgSize, msg, level);
}

void ColloLogger::manualAsyncFlush(const char* msg, LogLevel level)
{
    const size_t msgSize = std::strlen(msg);
    mLock.lock();

    if (BufferSize <= mAppendIndex + MinimalLogSize + msgSize) {
        swapBuffers();
        mLock.unlock();
        task([this, msgSize, msg, level] {
            write();
            flushMessage(msgSize, msg, level);
        });
    }
    else {
        addLog(msgSize, msg, level);
        swapBuffers();
        mLock.unlock();
        task([this] { write(); });
    }
}

void ColloLogger::manualSameThreadFlush(const char* msg, LogLevel level)
{
    const size_t msgSize = std::strlen(msg);
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