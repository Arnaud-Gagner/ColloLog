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

    void addCrit(const std::string_view& msg);
    void addDebug(const std::string_view& msg);
    void addInfo(const std::string_view& msg);
    void addWarn(const std::string_view& msg);

private:
    static constexpr size_t BufferSize = 1024;
    static constexpr size_t MinimalLogSize = 27;
    static constexpr size_t LevelSize = 6;
    static constexpr size_t TimeSize = 20;

private:
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