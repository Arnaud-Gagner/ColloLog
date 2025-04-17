#ifndef RING_LOCK_FREE_Z_ALLOCATOR_H
#define RING_LOCK_FREE_Z_ALLOCATOR_H

#include <fstream>
#include <mutex>
#include <string>

#include "ColloEnums.h"
#include "RingBuffer.h"

class RingLogger
{
public:
RingLogger(const std::string& filepath, size_t size = RingBuffer::DefaultSize);
    ~RingLogger();

    void addLog(const std::string& message);

private:
    void write();

private:
    std::string mFilePath;
    std::ofstream mFile;

    RingBuffer mBuffer;
    // TODO: switch to lock-free
    std::mutex mLocker;
};

#endif // !RING_LOCK_FREE_Z_ALLOCATOR_H
