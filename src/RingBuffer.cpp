#include "RingBuffer.h"

#include <sstream>

RingBuffer::RingBuffer(size_t size)
    : mMaxSize{ size }, mHead{}, mTail{},
    mIsFull{}
{
    mBuffer.resize(mMaxSize);
}

bool RingBuffer::isFull() const
{
    return mIsFull;
}

void RingBuffer::append(const std::string& message)
{
    mTail = (mTail + mIsFull) % mMaxSize;

    mBuffer[mHead] = message;
    mHead = (mHead + 1) % mMaxSize;
    mIsFull = mHead == mTail;
}

void RingBuffer::flush(std::ofstream& file, const std::string& filePath)
{
    std::stringstream output;
    size_t index = mTail;

    for (int i{}; i < mMaxSize; i++) {
        output << mBuffer[index];
        index = (index + 1) % mMaxSize;
    }
    
    file.open(filePath, std::ios::app);
    file.write(output.str().c_str(), output.str().size());
    file.close();

    mHead = mTail;
    mIsFull = false;
}
