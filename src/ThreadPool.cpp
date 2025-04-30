#include "ThreadPool.h"

#include <cassert>

ThreadPool::ThreadPool(unsigned int nThreads)
{
    mIsRunning = true;
    mNumThread = nThreads;
    assert(mNumThread < std::thread::hardware_concurrency()
        && "ThreadPool: the number of thread allocated in the pool exceeds the number of available thread by the hardware.");

    for (unsigned int i = 0; i < mNumThread; i++) {
        mThreads.emplace_back(std::thread(&ThreadPool::threadLoop, this));
    }
}

ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(mLock);
        mIsRunning = false;
    }
    mTaskNotifier.notify_all();

    for (std::thread& thread : mThreads) {
        thread.join();
    }
    mThreads.clear();
}

void ThreadPool::addTask(const std::function<void()>& task)
{
    {
        std::unique_lock<std::mutex> lock(mTaskLock);
        mTasks.emplace(task);
    }
    mTaskNotifier.notify_one();
}

void ThreadPool::stop()
{
    mIsRunning = false;
}

void ThreadPool::threadLoop()
{
    while(mIsRunning) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(mTaskLock);
            mTaskNotifier.wait(lock, [this] {
                return !mTasks.empty() || !mIsRunning;
            });
            if (!mTasks.empty()) {
                task = std::move(mTasks.front());
                mTasks.pop();
            }
        }
        if (task) {
            task();
        }
    }
}
