#include "LocalLogger.h"

#include <charconv>

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
    mFile.open(mFilePath, std::ios::app);
}

LocalLogger::~LocalLogger()
{
    mFile << mWriteBuffer;
    if (mFile.is_open()) {
        mFile.close();
    }
}

void LocalLogger::addLog(const char* msg)
{
    size_t msgSize = strlen(msg);

    if (BufferSize <= mAppendIndex + MinimalLogSize + msgSize) {
        swapBuffers();
        write();
    }
    char* tempIndex = mAppendBuffer + mAppendIndex;
    std::to_chars_result result = std::to_chars(tempIndex, tempIndex + TimeSize, static_cast<int>(std::clock()));
    tempIndex = result.ptr;

    const char* level = " CRIT ";
    std::memcpy(tempIndex, level, LevelSize);
    tempIndex += LevelSize;
    
    std::memcpy(tempIndex, msg, msgSize);
    tempIndex += msgSize;

    *tempIndex++ = '\n';

    mAppendIndex = static_cast<size_t>(tempIndex - mAppendBuffer);

}

void LocalLogger::swapBuffers()
{
    std::swap(mAppendBuffer, mWriteBuffer);
    mWriteIndex = mAppendIndex;
    mAppendIndex = 0;
}

void LocalLogger::write()
{
    std::unique_lock<std::mutex> lock(mLock);
    mFile << mWriteBuffer;
}
