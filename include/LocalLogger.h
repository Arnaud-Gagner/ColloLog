#ifndef LOCAL_LOGGER_H
#define LOCAL_LOGGER_H

#include <fstream>
#include <mutex>
#include <thread>

#include "ColloEnums.h"

class LocalLogger
{
public:
    LocalLogger(const std::string& filePath);
    ~LocalLogger();

    void setlevel(const LogLevel& lvl);

    void addCrit(const char* msg);
    void addDebug(const char* msg);
    void addInfo(const char* msg);
    void addWarn(const char* msg);

private:
    void addLog(const size_t& size, const char* msg, const LogLevel& lvl);
    void swapBuffers();
    void write();

private:
    static const size_t BufferSize = 1024;
    static constexpr size_t MinimalLogSize = 27;
    static constexpr size_t LevelSize = 6;
    static constexpr size_t TimeSize = 20;

private:
    std::string mFilePath;
    std::ofstream mFile;

    static thread_local char mBuffer1[BufferSize];
    static thread_local char mBuffer2[BufferSize];
    static thread_local char* mAppendBuffer;
    static thread_local char* mWriteBuffer;

    static thread_local unsigned int mAppendIndex;
    static thread_local unsigned int mWriteIndex;
    static thread_local LogLevel mLevel;

    std::mutex mLock;
};

#endif // !LOCAL_LOGGER_H
