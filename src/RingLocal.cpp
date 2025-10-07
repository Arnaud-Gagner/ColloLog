#include "RingLocal.h"

#include <cassert>
#include <charconv>
#include <iostream>

#include "ThreadPool.h"

extern ThreadPool pool;

thread_local RingBuffer RingLocal::mBuffer(25, "../ColloLog/Logs/test.log");

RingLocal::RingLocal(const std::string& filePath)
  : mFilePath{ filePath }
  , mLevel{ debug }
{
    mFile.open(mFilePath, std::ios::trunc);
    mFile.close();
}

RingLocal::~RingLocal() {}

void RingLocal::setLogLevel(const LogLevel& lvl)
{
    std::unique_lock<std::mutex> lock(mLock);
    mLevel = lvl;
}

void RingLocal::addCrit(const char* msg)
{
    size_t size = strlen(msg);
    assert(size + MinimalLogSize > RingBuffer::MaxElementSize
           && "ColloLog::RingLogger Buffer overflow due to too big message.");

    char message[RingBuffer::MaxElementSize];
    char* messageIndex = message;
    std::to_chars_result result = std::to_chars(
      messageIndex, messageIndex + TimeSize, static_cast<int>(std::clock()));
    messageIndex = result.ptr;

    const char* level = levelToCString(crit);
    std::memcpy(messageIndex, level, LevelSize);
    messageIndex += LevelSize;

    std::memcpy(messageIndex, msg, size);
    messageIndex += size;

    *messageIndex++ = '\n';
    addLog(message);
}

void RingLocal::addDebug(const char* msg)
{
    if (mLevel > debug) {
        return;
    }

    size_t size = strlen(msg);
    assert(size + MinimalLogSize > RingBuffer::MaxElementSize
           && "ColloLog::RingLogger Buffer overflow due to too big message.");

    char message[RingBuffer::MaxElementSize];
    char* messageIndex = message;
    std::to_chars_result result = std::to_chars(
      messageIndex, messageIndex + TimeSize, static_cast<int>(std::clock()));
    messageIndex = result.ptr;

    const char* level = levelToCString(debug);
    std::memcpy(messageIndex, level, LevelSize);
    messageIndex += LevelSize;

    std::memcpy(messageIndex, msg, size);
    messageIndex += size;

    *messageIndex++ = '\n';
    addLog(message);
}

void RingLocal::addInfo(const char* msg)
{
    if (mLevel > info) {
        return;
    }

    size_t size = strlen(msg);
    assert(size + MinimalLogSize > RingBuffer::MaxElementSize
           && "ColloLog::RingLogger Buffer overflow due to too big message.");

    char message[RingBuffer::MaxElementSize];
    char* messageIndex = message;
    std::to_chars_result result = std::to_chars(
      messageIndex, messageIndex + TimeSize, static_cast<int>(std::clock()));
    messageIndex = result.ptr;

    const char* level = levelToCString(info);
    std::memcpy(messageIndex, level, LevelSize);
    messageIndex += LevelSize;

    std::memcpy(messageIndex, msg, size);
    messageIndex += size;

    *messageIndex++ = '\n';
    addLog(message);
}

void RingLocal::addWarn(const char* msg)
{
    if (mLevel > warn) {
        return;
    }

    size_t size = strlen(msg);
    assert(size + MinimalLogSize > RingBuffer::MaxElementSize
           && "ColloLog::RingLogger Buffer overflow due to too big message.");

    char message[RingBuffer::MaxElementSize];
    char* messageIndex = message;
    std::to_chars_result result = std::to_chars(
      messageIndex, messageIndex + TimeSize, static_cast<int>(std::clock()));
    messageIndex = result.ptr;

    const char* level = levelToCString(warn);
    std::memcpy(messageIndex, level, LevelSize);
    messageIndex += LevelSize;

    std::memcpy(messageIndex, msg, size);
    messageIndex += size;

    *messageIndex++ = '\n';
    addLog(message);
}

void RingLocal::addLog(const char* msg)
{
    mBuffer.append(msg);
    if (mBuffer.isFull()) {
        write();
    }
}

void RingLocal::write()
{
    std::unique_lock<std::mutex> lock(mLock);
    mBuffer.flush();
}
