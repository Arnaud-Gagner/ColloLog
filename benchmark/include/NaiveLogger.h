#ifndef NAIVE_LOGGER_H
#define NAIVE_LOGGER_H

#include <fstream>
#include <mutex>
#include <thread>

#include <ColloLog/ColloEnums.h>

class NaiveLogger
{
public:
    NaiveLogger(const std::string& filePath);
    ~NaiveLogger();

    void setLogLevel(const LogLevel& lvl);

    void addCrit(const std::string& msg);
    void addDebug(const std::string& msg);
    void addInfo(const std::string& msg);
    void addWarn(const std::string& msg);

private:
    static const size_t BufferSize = 1024;

private:
    void addLog(const std::string& msg);
    void write();

private:
    std::string mFilePath;
    std::ofstream mFile;
    char mBuffer[BufferSize];
    int mCurrentIndex;

    LogLevel mLevel;
    std::mutex mLock;
};

#endif // !NAIVE_LOGGER_H
