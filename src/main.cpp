#include <string>

#include "NaiveLogger.h"
#include <iostream>

int main()
{
    NaiveLogger log("../ColloLog/Logs/naive.log");

    for (int i{}; i < 1000; i++) {
        log.addLog(LogLevel::info, std::to_string(i));
    }
}
