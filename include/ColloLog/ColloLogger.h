#ifndef COLLO_LOGGER_H
#define COLLO_LOGGER_H

#include <atomic>
#include <condition_variable>
#include <fstream>
#include <functional>
#include <mutex>
#include <queue>

#include "Enums.h"

class ColloLogger
{
public:
    ColloLogger(const std::string& filePath, FileOpen mode = FileOpen::Append);
    ~ColloLogger();

    void setLevel(const LogLevel& lvl);

    void crit(const char* msg, FlushStrat strat = FlushStrat::AutoAsync);
    void debug(const char* msg, FlushStrat strat = FlushStrat::AutoAsync);
    void info(const char* msg, FlushStrat strat = FlushStrat::AutoAsync);
    void warn(const char* msg, FlushStrat strat = FlushStrat::AutoAsync);

    void flush();
    void clear();

private:
    static constexpr size_t BufferSize = 32 * 1024;
    static constexpr size_t MinimalLogSize = 27;
    static constexpr size_t LevelSize = 6;
    static constexpr size_t TimeSize = 20;

private:
    void addLog(const size_t size, const char* msg, const LogLevel& lvl);
    // only this message gets flushed
    void flushMessage(size_t size, const char* msg, LogLevel level);
    // avoid swapping buffers when need to flush now
    void flushNow();
    void swapBuffers();
    void write();

    void task(const std::function<void()>& task);
    void threadLoop();

    void autoAsyncFlush(const char* msg, LogLevel level);
    void autoSameThreadFlush(const char* msg, LogLevel level);
    void manualAsyncFlush(const char* msg, LogLevel level);
    void manualSameThreadFlush(const char* msg, LogLevel level);

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
    std::mutex mTaskLock;

    std::atomic<bool> mIsThreadRunning;

    std::queue<std::function<void()>> mTasks;
    std::condition_variable mTaskNotifier;

    std::thread mWriteThread;
};

#endif // !COLLO_LOGGER_H
