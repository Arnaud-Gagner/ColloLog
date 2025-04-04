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
    void writeLogs();

private:
    static const size_t BufferSize = 600;

private:
    std::string mFilePath;
    std::ofstream mFile;

    static thread_local char mBuffer1[BufferSize];
    static thread_local char mBuffer2[BufferSize];
    static thread_local char* mCurrentBuffer;
    static thread_local char* mNextBuffer;

    static thread_local int mCurrentIndex;
    static thread_local int mNextIndex;

    std::mutex mLock;
};

#endif // !LOCAL_LOGGER_H