#include "RingLogger.h"

#include <charconv>
#include <iostream>
#include <cassert>

#include "RingBuffer.h"

RingLogger::RingLogger(const std::string& filePath, const size_t& size)
    : mBuffer(size, filePath), mLevel{ debug }
{
}

RingLogger::~RingLogger()
{
}

void RingLogger::setLogLevel(const LogLevel& lvl)
{
    mLevel = lvl;
}

void RingLogger::addCrit(const char* msg)
{
    size_t size = strlen(msg);
    
    assert(size > 256
        && "ColloLog::RingLogger Buffer overflow due to too big message.");

    char message[256];
    char* messageIndex = message;
    std::to_chars_result result = std::to_chars(messageIndex, messageIndex + TimeSize, static_cast<int>(std::clock()));
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
    if (mLevel > debug) { return; }
    size_t size = strlen(msg);
    
    assert(size > 256
        && "ColloLog::RingLogger Buffer overflow due to too big message.");

    char message[256];
    char* messageIndex = message;
    std::to_chars_result result = std::to_chars(messageIndex, messageIndex + TimeSize, static_cast<int>(std::clock()));
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
    if (mLevel > info) { return; }
    size_t size = strlen(msg);
    
    assert(size > 256
        && "ColloLog::RingLogger Buffer overflow due to too big message.");

    char message[256];
    char* messageIndex = message;
    std::to_chars_result result = std::to_chars(messageIndex, messageIndex + TimeSize, static_cast<int>(std::clock()));
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
    if (mLevel > warn) { return; }
    size_t size = strlen(msg);
    
    assert(size > 256
        && "ColloLog::RingLogger Buffer overflow due to too big message.");

    char message[256];
    char* messageIndex = message;
    std::to_chars_result result = std::to_chars(messageIndex, messageIndex + TimeSize, static_cast<int>(std::clock()));
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
    std::unique_lock<std::mutex> lock(mLocker);
    mBuffer.append(msg);
    if (mBuffer.isFull()) {
        mBuffer.flush();
    }
}

void RingLogger::write()
{
    mBuffer.flushTail();
}
