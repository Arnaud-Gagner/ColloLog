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
    void write();

private:
    std::ofstream mFile;
    std::string mFilePath;

    static thread_local RingBuffer mBuffer;

    LogLevel mLevel;
    std::mutex mLock;
};

#endif // !RING_LOCAL_H
