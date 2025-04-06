#include "RingLockFreeZAllocator.h"

#include <chrono>
#include <iostream>

#include "RingBuffer.h"

LockFreeZAllocator::LockFreeZAllocator(const std::string& filePath, size_t size)
    : mFilePath{ filePath }, mBuffer(size)
{
}

LockFreeZAllocator::~LockFreeZAllocator()
{
    write();
    if (mFile.is_open()) {
        mFile.close();
    }
}

void LockFreeZAllocator::addLog(const LogLevel& lvl, const std::string& msg)
{
    auto time = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
    std::string message = '[' + std::to_string(time.time_since_epoch().count()) + ']'
        + levelToString(lvl) + " " + msg + '\n';

    mLocker.lock();
    mBuffer.append(message);
    mLocker.unlock();
    
    if (mBuffer.isFull()) {
        write();
    }
}

void LockFreeZAllocator::write()
{
    mLocker.lock();
    mBuffer.flush(mFile, mFilePath);
    mLocker.unlock();
}