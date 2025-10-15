#include "ColloLog/RingLogger.h"

#include <cassert>
#include <charconv>
#include <iostream>

#include "ColloLog/RingBuffer.h"

RingLogger::RingLogger(const std::string& filePath, const size_t& size)
  : mBuffer(size, filePath)
  , mLevel{ debug }
{}

RingLogger::~RingLogger() {}

void RingLogger::setLogLevel(const LogLevel& lvl)
{
    mLevel = lvl;
}

void RingLogger::addCrit(const char* msg)
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

void RingLogger::addDebug(const char* msg)
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

void RingLogger::addInfo(const char* msg)
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

void RingLogger::addWarn(const char* msg)
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

void RingLogger::addLog(const char* msg)
{
    while (mAddingFlag.test_and_set(std::memory_order_acquire)) {
    }
    mBuffer.append(msg);
    mAddingFlag.clear(std::memory_order_release);
}
