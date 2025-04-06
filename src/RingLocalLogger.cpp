#include "RingLocalLogger.h"

#include <chrono>

thread_local RingBuffer RingLocalLogger::mBuffer(RingBuffer::DefaultSize);

RingLocalLogger::RingLocalLogger(const std::string& filePath, size_t size)
    : mFilePath{ filePath }
{
    mBuffer = RingBuffer(size);
}

RingLocalLogger::~RingLocalLogger()
{
    write();
    if (mFile.is_open()) {
        mFile.close();
    }
}

void RingLocalLogger::addLog(const LogLevel& lvl, const std::string& msg)
{
    auto time = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
    std::string message = '[' + std::to_string(time.time_since_epoch().count()) + ']'
        + levelToString(lvl) + " " + msg + '\n';

    mLock.lock();
    mBuffer.append(message);
    mLock.unlock();
    if (mBuffer.isFull()) {
        write();
    }
}

void RingLocalLogger::write()
{
    mLock.lock();
    mBuffer.flush(mFile, mFilePath);
    mLock.unlock();
}