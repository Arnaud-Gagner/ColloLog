#ifndef LOCAL_LOGGER_H
#define LOCAL_LOGGER_H

#include <fstream>
#include <mutex>
#include <thread>

#include "Enums.h"

class LocalLogger
{
public:
    LocalLogger(const std::string& filePath, FileOpen mode = FileOpen::Append);
    ~LocalLogger();

    void setlevel(const LogLevel& lvl);

    void crit(const char* msg, FlushStrat strat = FlushStrat::AutoAsync);
    void debug(const char* msg, FlushStrat strat = FlushStrat::AutoAsync);
    void info(const char* msg, FlushStrat strat = FlushStrat::AutoAsync);
    void warn(const char* msg, FlushStrat strat = FlushStrat::AutoAsync);

    void flush();

private:
    void addLog(const size_t& size, const char* msg, const LogLevel& lvl);
    void swapBuffers();
    void write(const char* data, const size_t size);

private:
    static const size_t BufferSize = 8 * 1024;
    static constexpr size_t MinimalLogSize = 27;
    static constexpr size_t LevelSize = 6;
    static constexpr size_t TimeSize = 20;

private:
    std::string mFilePath;
    std::ofstream mFile;

    static thread_local char mBuffer1[BufferSize];
    static thread_local char* mAppendBuffer;

    static thread_local unsigned int mAppendIndex;
    static thread_local LogLevel mLevel;

    std::mutex mLock;
};

#endif // !LOCAL_LOGGER_H
