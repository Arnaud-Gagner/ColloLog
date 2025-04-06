#ifndef RING_LOCK_FREE_Z_ALLOCATOR_H
#define RING_LOCK_FREE_Z_ALLOCATOR_H

#include <fstream>
#include <mutex>
#include <string>

#include "ColloEnums.h"
#include "RingBuffer.h"

class LockFreeZAllocator
{
public:
    LockFreeZAllocator(const std::string& filepath, size_t size = RingBuffer::DefaultSize);
    ~LockFreeZAllocator();

    void addLog(const LogLevel& lvl, const std::string& message);

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