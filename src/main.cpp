#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <Windows.h>

#include "benchmarks.h"
#include "ThreadPool.h"

ThreadPool pool(3);
const unsigned int MaxThreads = 10;
const unsigned int Rounds = 10;
const char* CsvFile = "D:\\LocalLog\\result.csv";
const char* TempLogFile = "Logs/test.log";
const std::string VersionDir = "D:\\LocalLog\\";

// in benchmark, change used version if wanted

void allRoundsOfThread(const unsigned int nThreads)
{
    std::ofstream result(CsvFile, std::ios::app);

    for (unsigned int i = 0; i < Rounds; ++i) {
        std::cout << "starting round " << i << '\n';
        // init
        FILETIME creationTime, exitTime, kernelTime, userTime;
        double cpuTime = 0.0;

        // benchmark and measurements
        HANDLE hProcess = GetCurrentProcess();

        long long throughputTime = runLongBenchmark(nThreads);

        if (GetProcessTimes(hProcess, &creationTime, &exitTime, &kernelTime, &userTime)) {
            ULARGE_INTEGER k, u;
            k.LowPart = kernelTime.dwLowDateTime;
            k.HighPart = kernelTime.dwHighDateTime;
            u.LowPart = userTime.dwLowDateTime;
            u.HighPart = userTime.dwHighDateTime;

            cpuTime = (k.QuadPart + u.QuadPart) / 10000.0;
        }
        // post process
        result << nThreads << ',' << throughputTime << ',' << cpuTime << '\n';

        std::string path = VersionDir + std::to_string(nThreads);
        std::filesystem::create_directory(path);
        path += "\\" + std::to_string(i) + ".log";

        if (rename(TempLogFile, path.c_str())) {
            std::cout << "Need to move manually the log file :(";
        }
    }
}

int main()
{
    for (unsigned int i = 1; i < MaxThreads; ++i) {
        allRoundsOfThread(i);
    }
}
