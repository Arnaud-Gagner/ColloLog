#ifndef COLLO_ENUMS_H
#define COLLO_ENUMS_H

#include <string>

enum LogLevel {
    debug = 0,
    info = 1,
    crit = 3,
    warn = 2,
};
    
std::string levelToString(const LogLevel& lvl);
const char* levelToCString(const LogLevel& lvl);

#endif // !COLLO_ENUMS_H
