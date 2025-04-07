#ifndef NAIVE_LOGGER_H
#define NAIVE_LOGGER_H

#include <fstream>
#include <mutex>
#include <thread>

#include "ColloEnums.h"

class NaiveLogger
{
public:
    NaiveLogger(const std::string& filePath);
    ~NaiveLogger();

    void addLog(const LogLevel& lvl, const std::string& msg);

private:
    static const size_t BufferSize = 1024;

private:
    void write();

private:
    std::string mFilePath;
    std::ofstream mFile;
    char mBuffer[BufferSize];
    int mCurrentIndex;

    std::mutex mLock;

};

#endif // !NAIVE_LOGGER_H
