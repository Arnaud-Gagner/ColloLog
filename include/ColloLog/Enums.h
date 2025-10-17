#ifndef COLLO_ENUMS_H
#define COLLO_ENUMS_H

#include <string>

enum LogLevel {
    Debug = 0,
    Info = 1,
    Crit = 3,
    Warn = 2,
};

enum LogRotation {
    Append,
    Clear,
    ClearDaily,
    ClearWeekly
};

enum FlushStrat {
    AutoAsync,
    AutoSameThread,
    ManualAsync,
    ManualSameThread
};

const char* levelToCString(const LogLevel& lvl);

#endif // !COLLO_ENUMS_H
