#include "LocalLogger.h"

#include <chrono>
#include <future>

thread_local char LocalLogger::mBuffer1[LocalLogger::BufferSize];
thread_local char LocalLogger::mBuffer2[LocalLogger::BufferSize];

thread_local char* LocalLogger::mAppendBuffer = LocalLogger::mBuffer1;
thread_local char* LocalLogger::mWriteBuffer = LocalLogger::mBuffer2;

thread_local unsigned int LocalLogger::mAppendIndex = 0;
thread_local unsigned int LocalLogger::mWriteIndex = 0;

LocalLogger::LocalLogger(const std::string& filepath)
    : mFilePath{ filepath }
{
    mAppendBuffer = mBuffer1;
    mWriteBuffer = mBuffer2;
}

LocalLogger::~LocalLogger()
{
    write();
    if (mFile.is_open()) {
        mFile.close();
    }
}

void LocalLogger::addLog(const LogLevel& lvl, const std::string& msg)
{
    auto time = std::chrono::time_point_cast<std::chrono::milliseconds>(std::chrono::system_clock::now());
    std::string message = "[";
    message += std::to_string(time.time_since_epoch().count());
    message += ']';
    message += levelToString(lvl);
    message += ' ';
    message += msg;
    message += '\n';
    size_t messageSize = message.size();
    const char* cstr = message.c_str();

    if (BufferSize < mAppendIndex + messageSize) {
        swapBuffers();
        write();
    }
    for (int i = 0; i < messageSize; i++) {
        mAppendBuffer[mAppendIndex+i] = message[i];
    }
    mAppendIndex += messageSize;
}

void LocalLogger::swapBuffers()
{
    std::swap(mAppendBuffer, mWriteBuffer);
    mWriteIndex = mAppendIndex;
    mAppendIndex = 0;
}

void LocalLogger::write()
{
    mLock.lock();
    mFile.open(mFilePath, std::fstream::app);
    mFile.write(mWriteBuffer, mWriteIndex);
    mFile.close();
    mLock.unlock();
}
