#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <functional>
#include <mutex>
#include <queue>

class ThreadPool
{
public:
    explicit ThreadPool(unsigned int nThreads);
    ~ThreadPool();

    void addTask(const std::function<void()>& task);
    void stop();

private:
    void threadLoop();

private:
    unsigned int mNumThread;
    std::atomic<bool> mIsRunning;

    std::mutex mLock;

    std::queue<std::function<void()>> mTasks;
    std::mutex mTaskLock;
    std::condition_variable mTaskNotifier;

    std::vector<std::thread> mThreads;

};

#endif // !THREAD_POOL_H
