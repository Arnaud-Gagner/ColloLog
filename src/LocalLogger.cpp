#include "ColloLog/LocalLogger.h"

#include <charconv>

#include "ColloLog/ThreadPool.h"

extern ThreadPool pool;

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
    swapBuffers();
    std::this_thread::sleep_for(std::chrono::seconds(2));
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
    }
    addLog(msgSize, msg, crit);
}

void LocalLogger::addDebug(const char* msg)
{
    if (debug < mLevel) {
        return;
    }

    size_t msgSize = strlen(msg);
    if (BufferSize <= mAppendIndex + MinimalLogSize + msgSize) {
        swapBuffers();
    }
    addLog(msgSize, msg, debug);
}

void LocalLogger::addInfo(const char* msg)
{
    if (debug < mLevel) {
        return;
    }

    size_t msgSize = strlen(msg);
    if (BufferSize <= mAppendIndex + MinimalLogSize + msgSize) {
        swapBuffers();
    }
    addLog(msgSize, msg, info);
}

void LocalLogger::addWarn(const char* msg)
{
    if (warn < mLevel) {
        return;
    }

    size_t msgSize = strlen(msg);
    if (BufferSize <= mAppendIndex + MinimalLogSize + msgSize) {
        swapBuffers();
    }
    addLog(msgSize, msg, warn);
}

void LocalLogger::addLog(const size_t& size, const char* msg, const LogLevel& lvl)
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

void LocalLogger::swapBuffers()
{
    std::swap(mAppendBuffer, mWriteBuffer);
    mWriteIndex = mAppendIndex;
    mAppendIndex = 0;
    const size_t size = mWriteIndex;
    char* bufferCopy = new char[size];
    std::memcpy(bufferCopy, mWriteBuffer, size);
    write(bufferCopy, size);
    // pool.addTask([this, bufferCopy, size] { write(bufferCopy, size); });
}

void LocalLogger::write(const char* data, const size_t size)
{
    std::unique_lock<std::mutex> lock(mLock);
    mFile.write(data, size);
}
