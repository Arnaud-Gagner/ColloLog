#include "ColloLogger.h"

#include <ctime>
#include <cstring>
#include <charconv>
#include <iostream>

#include "ThreadPool.h"

extern ThreadPool pool;

ColloLogger::ColloLogger(const std::string& filePath)
    : mFilePath{ filePath }, mAppendIndex{},
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
    pool.addTask([this]{ write(); });
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
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
        addLog(msgSize, msg, crit);
        mLock.unlock();
        pool.addTask([this]{ write(); });
    }
    else {
        addLog(msgSize, msg, crit);
        mLock.unlock();
    }
}

void ColloLogger::addDebug(const char* msg)
{
    if (mLevel > debug) { return; }
    
    size_t msgSize = strlen(msg);
    mLock.lock();

    if (BufferSize <= mAppendIndex + MinimalLogSize + msgSize) {
        swapBuffers();
        addLog(msgSize, msg, debug);
        mLock.unlock();
        pool.addTask([this]{ write(); });
    }
    else {
        addLog(msgSize, msg, debug);
        mLock.unlock();
    }
}

void ColloLogger::addInfo(const char* msg)
{
    if (mLevel > info) { return; }

    size_t msgSize = strlen(msg);
    mLock.lock();

    if (BufferSize <= mAppendIndex + MinimalLogSize + msgSize) {
        swapBuffers();
        addLog(msgSize, msg, info);
        mLock.unlock();
        pool.addTask([this]{ write(); });
    }
    else {
        addLog(msgSize, msg, info);
        mLock.unlock();
    }
}

void ColloLogger::addWarn(const char* msg)
{
    if (mLevel > warn) { return; }

    size_t msgSize = strlen(msg);
    mLock.lock();

    if (BufferSize <= mAppendIndex + MinimalLogSize + msgSize) {
        swapBuffers();
        addLog(msgSize, msg, warn);
        mLock.unlock();
        pool.addTask([this]{ write(); });
    }
    else {
        addLog(msgSize, msg, warn);
        mLock.unlock();
    }
}

void ColloLogger::addLog(const size_t size, const char* msg, const LogLevel& lvl)
{
    char* message = mAppendBuffer + mAppendIndex;
    std::to_chars_result result = std::to_chars(message, message + TimeSize, static_cast<int>(std::clock()));
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

void ColloLogger::write()
{
    std::unique_lock<std::mutex> lock(mFileLock);
    mFile << mWriteBuffer;
}
