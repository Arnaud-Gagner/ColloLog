#ifndef RING_LOGGER_H
#define RING_LOGGER_H

#include <condition_variable>
#include <fstream>
#include <mutex>

#include "Enums.h"
#include "RingBuffer.h"

class RingLogger
{
public:
    RingLogger(const std::string& filepath, const size_t& size = RingBuffer::DefaultSize);
    ~RingLogger();

    void setLogLevel(const LogLevel& lvl);

    void addCrit(const char* msg);
    void addDebug(const char* msg);
    void addInfo(const char* msg);
    void addWarn(const char* msg);

private:
    static constexpr size_t MinimalLogSize = 27;
    static constexpr size_t LevelSize = 6;
    static constexpr size_t TimeSize = 20;

private:
    void addLog(const char* message);

private:
    std::atomic_flag mAddingFlag = ATOMIC_FLAG_INIT;
    RingBuffer mBuffer;
    LogLevel mLevel;
};

#endif // !RING_LOGGER_H
