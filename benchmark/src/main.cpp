#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <thread>

#include "ColloLog.h"
#include "benchmarks.h"

const unsigned int MaxThreads = 10;
const unsigned int Rounds = 10;
const char* CsvFile = "D:/Iteration2/ColloLog/result.csv";
const char* TempLogFile = "Logs/test.log";
const std::string VersionDir = "D:/Iteration2/ColloLog/";

void copyFile(const std::string& source, const std::string& destination)
{
    std::ifstream in(source, std::ios::binary);
    std::ofstream out(destination, std::ios::binary);
    if (!in || !out) {
        return;
    }

    out << in.rdbuf();
    out.close();
    in.close();
}

void dropRate()
{
    runDropRateBenchmark(5);
    std::cout << "Finished drop log";
    std::string path = VersionDir + "drop_rate.log";
    copyFile(TempLogFile, path.c_str());
}

void allRoundsOfThread(const unsigned int nThreads)
{
    std::ofstream result(CsvFile, std::ios::app);
    double totalLoggingTime = 0.0;
    for (unsigned int i = 0; i < Rounds; ++i) {
        localog::clear();
        std::cout << "\n\tstarting round " << i;

        long long throughputTime = runLongBenchmark(nThreads);
        totalLoggingTime += throughputTime;
        std::cout << "\n\tmean time per log(ns): "
                  << throughputTime / (nThreads * MessagesPerThread);
    }
    std::cout << "\nAverage throughput across all rounds: " << std::fixed
              << std::setprecision(2)
              << ((Rounds * nThreads * MessagesPerThread) / totalLoggingTime) * 1e9
              << " logs/sec\n";
}

int main()
{
    collog::init("Logs/collog.log", FileOpen::Clear);
    localog::init("Logs/localog.log", FileOpen::Clear);
    // dropRate();
    for (unsigned int i = 1; i <= MaxThreads; ++i) {
        std::cout << "\nStarting for threads: " << i;
        allRoundsOfThread(i);
    }
    return 0;
}
