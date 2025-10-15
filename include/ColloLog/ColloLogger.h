#ifndef COLLO_LOGGER_H
#define COLLO_LOGGER_H

#include <fstream>
#include <mutex>

#include "ColloEnums.h"

// TODO: lock-free
class ColloLogger
{
public:
    ColloLogger(const std::string& filePath);
    ~ColloLogger();

    void setLogLevel(const LogLevel& lvl);

    void addCrit(const char* msg);
    void addDebug(const char* msg);
    void addInfo(const char* msg);
    void addWarn(const char* msg);

    void flush();
    
private:
    static constexpr size_t BufferSize = 100 * 1024;
    static constexpr size_t MinimalLogSize = 27;
    static constexpr size_t LevelSize = 6;
    static constexpr size_t TimeSize = 20;

private:
    void addLog(const size_t size, const char* msg, const LogLevel& lvl);
    void swapBuffers();
    void write();

private:
    std::ofstream mFile;
    std::string mFilePath;

    char mBuffer1[BufferSize];
    char mBuffer2[BufferSize];
    char* mAppendBuffer;
    char* mWriteBuffer;

    unsigned int mAppendIndex;
    unsigned int mWriteIndex;

    LogLevel mLevel;

    std::mutex mLock;
    std::mutex mFileLock;
};

#endif // !COLLO_LOGGER_H
