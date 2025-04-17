#include "ThreadPool.h"

#include <cassert>

ThreadPool::ThreadPool(unsigned int nThreads)
{
    mNumThread = nThreads;
    assert(mNumThread < std::thread::hardware_concurrency()
        && "ThreadPool: the number of thread allocated in the pool exceeds the number of available thread by the hardware.");

    for (int i{}; i < mNumThread; i++) {
        mThreads.emplace_back(std::thread(&ThreadPool::threadLoop, this));
    }
}

ThreadPool::~ThreadPool()
{
    mIsRunning = false;

    std::unique_lock<std::mutex> lock(mLock);
    mTaskNotifier.notify_all();

    for (std::thread& thread : mThreads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
    mThreads.clear();
}

void ThreadPool::addTask(const std::function<void()>& task)
{
    std::unique_lock<std::mutex> lock(mTaskLock);
    mTasks.push(task);
    mTaskNotifier.notify_one();
}

void ThreadPool::threadLoop()
{
    while(mIsRunning) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(mTaskLock);
            mTaskNotifier.wait(lock, [this] {
                return mTasks.empty() && mIsRunning;
            });
            task = mTasks.front();
            mTasks.pop();
        }
        task();
    }
}
