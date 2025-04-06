#include "RingBuffer.h"

RingBuffer::RingBuffer(size_t size)
    : mMaxSize{ size }, mHead{}, mTail{},
    mIsFull{}
{
    mBuffer = std::vector<std::string>(mMaxSize);
}

bool RingBuffer::isFull() const
{
    return mIsFull;
}

void RingBuffer::append(const std::string& message)
{
    mTail = (mIsFull) * ((mTail + 1) % mMaxSize)
        + (!mIsFull) * (mTail);

    mBuffer[mHead] = message;
    mHead = (mHead + 1) % mMaxSize;
    mIsFull = mHead == mTail;
}

std::string RingBuffer::flush(std::ofstream& file, const std::string& filePath)
{
    std::string output = "";
    size_t index = mTail;

    file.open(filePath, std::fstream::app);
    for (int i{}; i < mMaxSize; i++) {
        file.write(mBuffer[index].c_str(), mBuffer[index].size());
        index = (index + 1) % mMaxSize;
    }
    file.close();

    mHead = mTail;
    mIsFull = false;
    return output;
}
