#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <fstream>
#include <mutex>
#include <string>
#include <vector>

class RingBuffer
{
public:
    static constexpr size_t MaxElementSize = 256;
    static constexpr size_t DefaultSize = 25 * 100;

public:
    RingBuffer(const size_t& size, const std::string& filePath);
    ~RingBuffer();

    bool isFull() const;

    void append(const char* message);
    void flush();
    void flushTail();

private:
    char mBuffer[DefaultSize][MaxElementSize];
    size_t mHead;
    size_t mTail;
    bool mIsFull;
    size_t mMaxSize;

    std::ofstream mFile;
    std::string mFilePath;
    std::mutex mLock;

    std::condition_variable mFlushNotifier;
};

#endif // !RING_BUFFER_H
