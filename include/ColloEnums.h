#ifndef COLLO_ENUMS_H
#define COLLO_ENUMS_H

#include <string>

enum LogLevel {
    debug,
    info,
    crit,
    warn,
};
    
std::string levelToString(const LogLevel& lvl);
const char* levelToCString(const LogLevel& lvl);

#endif // !COLLO_ENUMS_H
