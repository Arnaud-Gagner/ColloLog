#ifndef COLLO_ENUMS_H
#define COLLO_ENUMS_H

#include <string>

enum LogLevel {
    debug,
    info,
    fatal,
    warn,
};
    
std::string levelToString(const LogLevel& lvl);

#endif // !COLLO_ENUMS_H
