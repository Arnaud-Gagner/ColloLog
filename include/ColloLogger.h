#ifndef COLLO_LOGGER_H
#define COLLO_LOGGER_H

#include <fstream>
#include <mutex>

#include "ColloEnums.h"

// TODO: lock-free
// TODO: optimize log message format
class ColloLogger
{
public:
    ColloLogger(const std::string& filePath);
    ~ColloLogger();

    void addLog(const LogLevel& lvl, const std::string& msg);

private:
    static constexpr size_t BufferSize = 1024;

private:
    void swapBuffers();
    void write();

private:
    std::ofstream mFile;
    std::string mFilePath;

    char mBuffer1[BufferSize];
    char mBuffer2[BufferSize];
    char* mCurrentBuffer;
    char* mNextBuffer;

    unsigned int mCurrentIndex;
    unsigned int mNextIndex;

    std::mutex mLock;
};


#endif // !COLLO_LOGGER_H