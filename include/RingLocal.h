#ifndef RING_LOCAL_H
#define RING_LOCAL_H

#include "ColloEnums.h"
#include "RingBuffer.h"

#include <fstream>
#include <mutex>

class RingLocal
{
public:
    RingLocal(const std::string& filePath, const size_t& size = RingBuffer::DefaultSize);
    ~RingLocal();

    void addLog(const char* message);

private:
    static constexpr size_t MinimalLogSize = 27;
    static constexpr size_t LevelSize = 6;
    static constexpr size_t TimeSize = 20;

private:
    void write();

private:
    std::ofstream mFile;
    std::string mFilePath;

    static thread_local RingBuffer mBuffer;
    std::mutex mLock;
};

#endif // !RING_LOCAL_H
