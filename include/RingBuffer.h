#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <fstream>
#include <string>
#include <vector>

class RingBuffer
{
public:
    static constexpr size_t DefaultSize = 25;

public:
    RingBuffer(size_t size = DefaultSize);
    ~RingBuffer() = default;

    bool isFull() const;

    void append(const std::string& message);
    std::string flush(std::ofstream& file, const std::string& filePath);

private:
    std::vector<std::string> mBuffer;
    size_t mHead;
    size_t mTail;

    bool mIsFull;
    size_t mMaxSize;
};

#endif // !RING_BUFFER_H