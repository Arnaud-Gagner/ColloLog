#include "RingBuffer.h"

#include <iostream>

RingBuffer::RingBuffer(const size_t& size, const std::string& filePath)
    : mMaxSize{ size }, mHead{}, mTail{},
    mIsFull{}, mFilePath{ filePath }
{
    mFile.open(mFilePath, std::ios::app);
}

RingBuffer::~RingBuffer()
{
    if (mFile.is_open()) {
        mFile.close();
    }
}

bool RingBuffer::isFull() const
{
    return mIsFull;
}

void RingBuffer::append(const char* message)
{
    mTail = (mTail + mIsFull) % mMaxSize;

    strncpy_s(mBuffer[mHead], MaxElementSize, message, MaxElementSize);

    mHead = (mHead + 1) % mMaxSize;
    mIsFull = mHead == mTail;
}

void RingBuffer::flush()
{
    size_t index = mTail;
    for (int i{}; i < mMaxSize; i++) {
        mFile << mBuffer[index];
        index = (index + 1) % mMaxSize;
    }
    mHead = mTail;
    mIsFull = false;
}

void RingBuffer::flushTail()
{
    mFile << mBuffer[mTail];
    mTail = (mTail + mIsFull) % mMaxSize;
}
