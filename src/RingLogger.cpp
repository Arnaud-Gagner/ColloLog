#include "RingLogger.h"

#include <chrono>
#include <iostream>

#include "RingBuffer.h"

RingLogger::RingLogger(const std::string& filePath, size_t size)
    : mFilePath{ filePath }, mBuffer(size)
{
}

RingLogger::~RingLogger()
{
    write();
    if (mFile.is_open()) {
        mFile.close();
    }
}

void RingLogger::addLog(const std::string& msg)
{
    std::string message = std::to_string(std::clock()) + " INFO " + msg + '\n';

    mLocker.lock();
    mBuffer.append(message);
    mLocker.unlock();
    
    if (mBuffer.isFull()) {
        write();
    }
}

void RingLogger::write()
{
    std::unique_lock<std::mutex> lock(mLocker);
    mBuffer.flush(mFile, mFilePath);
}