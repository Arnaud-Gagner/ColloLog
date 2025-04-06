#ifndef RING_LOCAL_LOGGER_H
#define RING_LOCAL_LOGGER_H

#include "ColloEnums.h"
#include "RingBuffer.h"

#include <fstream>
#include <mutex>

class RingLocalLogger
{
public:
    RingLocalLogger(const std::string& filePath, size_t size = RingBuffer::DefaultSize);
    ~RingLocalLogger();

    void addLog(const LogLevel& lvl, const std::string& message);

private:
    void write();

private:
    std::ofstream mFile;
    std::string mFilePath;

    static thread_local RingBuffer mBuffer;
    std::mutex mLock;
};

#endif // !RING_LOCAL_LOGGER_H