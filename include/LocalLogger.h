#ifndef LOCAL_LOGGER_H
#define LOCAL_LOGGER_H

#include <fstream>
#include <mutex>
#include <thread>

#include "ColloEnums.h"

// TODO: lock-free stuff when writting in file
// TODO: flush at end of thread
class LocalLogger
{
public:
    LocalLogger(const std::string& filePath);
    ~LocalLogger();

    void addLog(const LogLevel& lvl, const std::string& msg);

private:
    void swapBuffers();
    void write();

private:
    static const size_t BufferSize = 1024;

private:
    std::string mFilePath;
    std::ofstream mFile;

    static thread_local char mBuffer1[BufferSize];
    static thread_local char mBuffer2[BufferSize];
    static thread_local char* mAppendBuffer;
    static thread_local char* mWriteBuffer;

    static thread_local unsigned int mAppendIndex;
    static thread_local unsigned int mWriteIndex;

    std::mutex mLock;
};

#endif // !LOCAL_LOGGER_H