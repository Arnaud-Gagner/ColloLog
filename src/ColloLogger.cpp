#include "ColloLogger.h"

#include <chrono>
#include <future>

ColloLogger::ColloLogger(const std::string& filePath)
    : mFilePath{ filePath}, mCurrentIndex{}, mNextIndex{}
{
    memset(mBuffer1, 0, BufferSize);
    memset(mBuffer2, 0, BufferSize);
    mCurrentBuffer = mBuffer1;
    mNextBuffer = mBuffer2;
}

ColloLogger::~ColloLogger()
{
    write();
    if (mFile.is_open()) {
        mFile.close();
    }
}

void ColloLogger::addLog(const LogLevel& lvl, const std::string& msg)
{
    auto time = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
    std::string message{
        '[' + std::to_string(time.time_since_epoch().count()) + ']' + levelToString(lvl) + " " + msg + '\n'
    };
    size_t messageSize = message.size();
    
    mLock.lock();
    if (BufferSize < mCurrentIndex + messageSize) {
        std::memcpy(mNextBuffer + mNextIndex, message.c_str(), messageSize);
        mNextIndex += messageSize;
        write();
    }
    else {
        std::memcpy(mCurrentBuffer + mCurrentIndex, message.c_str(), messageSize);
        mCurrentIndex += messageSize;
    }
    mLock.unlock();
}

void ColloLogger::swapBuffers()
{
    std::swap(mCurrentBuffer, mNextBuffer);
    mCurrentIndex = mNextIndex;
    mNextIndex = 0;
}

void ColloLogger::write()
{
    std::async(std::launch::async, [this]() {
        mFile.open(mFilePath, std::ios::app);
        mFile.write(mCurrentBuffer, mCurrentIndex);
        mFile.close();
        swapBuffers();
    });
}