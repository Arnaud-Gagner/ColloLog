#include "LocalLogger.h"

#include <charconv>

thread_local char LocalLogger::mBuffer1[LocalLogger::BufferSize];
thread_local char LocalLogger::mBuffer2[LocalLogger::BufferSize];

thread_local char* LocalLogger::mAppendBuffer = LocalLogger::mBuffer1;
thread_local char* LocalLogger::mWriteBuffer = LocalLogger::mBuffer2;

thread_local unsigned int LocalLogger::mAppendIndex = 0;
thread_local unsigned int LocalLogger::mWriteIndex = 0;
thread_local LogLevel LocalLogger::mLevel = debug;

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

void LocalLogger::setlevel(const LogLevel& lvl)
{
    mLevel = lvl;
}

void LocalLogger::addCrit(const char* msg)
{
    size_t msgSize = strlen(msg);
    if (BufferSize <= mAppendIndex + MinimalLogSize + msgSize) {
        swapBuffers();
        write();
    }
    addLog(msgSize, msg, crit);
}

void LocalLogger::addDebug(const char* msg)
{
    if (debug < mLevel) { return; }

    size_t msgSize = strlen(msg);
    if (BufferSize <= mAppendIndex + MinimalLogSize + msgSize) {
        swapBuffers();
        write();
    }
    addLog(msgSize, msg, debug);
}

void LocalLogger::addInfo(const char* msg)
{
    if (debug < mLevel) { return; }

    size_t msgSize = strlen(msg);
    if (BufferSize <= mAppendIndex + MinimalLogSize + msgSize) {
        swapBuffers();
        write();
    }
    addLog(msgSize, msg, info);
}

void LocalLogger::addWarn(const char* msg)
{
    if (warn < mLevel) { return; }

    size_t msgSize = strlen(msg);
    if (BufferSize <= mAppendIndex + MinimalLogSize + msgSize) {
        swapBuffers();
        write();
    }
    addLog(msgSize, msg, warn);
}

void LocalLogger::addLog(const size_t& size, const char* msg, const LogLevel& lvl)
{
    char* tempIndex = mAppendBuffer + mAppendIndex;
    std::to_chars_result result = std::to_chars(tempIndex, tempIndex + TimeSize, static_cast<int>(std::clock()));
    tempIndex = result.ptr;

    const char* level = levelToCString(lvl);
    std::memcpy(tempIndex, level, LevelSize);
    tempIndex += LevelSize;
    
    std::memcpy(tempIndex, msg, size);
    tempIndex += size;
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
