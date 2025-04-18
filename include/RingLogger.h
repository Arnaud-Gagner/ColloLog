#ifndef RING_LOGGER_H
#define RING_LOGGER_H

#include <condition_variable>
#include <fstream>
#include <mutex>

#include "ColloEnums.h"
#include "RingBuffer.h"

class RingLogger
{
public:
    RingLogger(const std::string& filepath, const size_t& size = RingBuffer::DefaultSize);
    ~RingLogger();

    void addLog(const char* message);

private:
    static constexpr size_t MinimalLogSize = 27;
    static constexpr size_t LevelSize = 6;
    static constexpr size_t TimeSize = 20;

private:
    void write();

private:
    std::mutex mLocker;
    RingBuffer mBuffer;

    // std::thread mWriter;
    // std::condition_variable mWriteNotifier;
};

#endif // !RING_LOGGER_H
