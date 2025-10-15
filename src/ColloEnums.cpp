#include "ColloLog/Levels.h"

std::string levelToString(const LogLevel& lvl)
{
    switch (lvl) {
        case LogLevel::debug: {
            return "DEBUG";
        }
        case LogLevel::info: {
            return "INFO";
        }
        case LogLevel::crit: {
            return "CRIT";
        }
        case LogLevel::warn: {
            return "WARN";
        }
        default: {
            return "INVALID_LEVEL";
        }
    }
}

const char* levelToCString(const LogLevel& lvl)
{
    switch (lvl) {
        case LogLevel::debug: {
            return " DBUG ";
        }
        case LogLevel::info: {
            return " INFO ";
        }
        case LogLevel::crit: {
            return " CRIT ";
        }
        case LogLevel::warn: {
            return " WARN ";
        }
        default: {
            return "EROR";
        }
    }
}
