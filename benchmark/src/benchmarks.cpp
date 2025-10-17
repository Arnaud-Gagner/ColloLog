#include "benchmarks.h"

#include <chrono>
#include <cstring>
#include <iostream>
#include <queue>
#include <string>
#include <thread>

#include <ColloLog/ColloLogger.h>
#include <ColloLog/LocalLogger.h>

#include "NaiveLogger.h"

const unsigned int Iterations = 10;
const int MessagesPerThread = 1000000;
const char FooWasCalledMessage[] = "Benchmark::foo was called";

std::thread colloTask(ColloLogger& logger, const char* msg = "")
{
    std::thread colloThread([&] {
        for (int i = 0; i < MessagesPerThread; ++i) {
            logger.info(msg);
        }
    });
    return colloThread;
}

std::thread colloDropRate(ColloLogger& logger)
{
    std::thread colloThread([&] {
        for (int i = 0; i < MessagesPerThread; ++i) {
            logger.info(std::to_string(i).c_str());
        }
    });
    return colloThread;
}

std::thread localTask(LocalLogger& logger, const char* msg = "")
{
    std::thread colloThread([&] {
        for (int i = 0; i < MessagesPerThread; ++i) {
            logger.info(msg);
        }
    });
    return colloThread;
}

std::thread LocalLDropRate(LocalLogger& logger)
{
    std::thread colloThread([&] {
        for (int i = 0; i < MessagesPerThread; ++i) {
            logger.info(std::to_string(i).c_str());
        }
    });
    return colloThread;
}

std::thread naiveTask(NaiveLogger& logger, const std::string& msg = "")
{
    std::thread colloThread([&] {
        for (int i = 0; i < MessagesPerThread; ++i) {
            logger.addInfo(msg);
        }
    });
    return colloThread;
}

std::thread naiveDropRate(NaiveLogger& logger)
{
    std::thread colloThread([&] {
        for (int i = 0; i < MessagesPerThread; ++i) {
            logger.addInfo(std::to_string(i));
        }
    });
    return colloThread;
}

long long runLongBenchmark(unsigned int nThreads)
{
    ColloLogger collo("Logs/test.log", FileOpen::Clear);
    std::queue<std::thread> threads;
    auto startTime = std::chrono::time_point_cast<std::chrono::microseconds>(
      std::chrono::system_clock::now());

    for (unsigned i = 0; i < nThreads; ++i) {
        threads.push(std::move(colloTask(collo, FooWasCalledMessage)));
    }
    size_t threads_number = threads.size();
    for (unsigned int i = 0; i < threads_number; ++i) {
        threads.front().join();
        threads.pop();
    }
    return (std::chrono::time_point_cast<std::chrono::microseconds>(
              std::chrono::system_clock::now())
            - startTime)
      .count();
}

long long runDropRateBenchmark(unsigned int nThreads)
{
    ColloLogger collo("Logs/test.log", FileOpen::Clear);
    std::queue<std::thread> threads;
    auto startTime = std::chrono::time_point_cast<std::chrono::milliseconds>(
      std::chrono::system_clock::now());

    for (unsigned int i = 0; i < nThreads; ++i) {
        threads.push(std::move(colloDropRate(collo)));
    }
    size_t threads_number = threads.size();
    for (unsigned int i = 0; i < threads_number; ++i) {
        threads.front().join();
        threads.pop();
    }
    return (std::chrono::time_point_cast<std::chrono::milliseconds>(
              std::chrono::system_clock::now())
            - startTime)
      .count();
}
