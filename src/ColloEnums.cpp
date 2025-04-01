#include "ColloEnums.h"

std::string levelToString(const LogLevel& lvl)
{
    std::string level;
    switch (lvl)
    {
    case LogLevel::debug: {
        level = "DEBUG";
        break;
    }
    case LogLevel::info: {
        level = "INFO";
        break;
    }
    case LogLevel::fatal: {
        level = "FATAL";
        break;
    }
    case LogLevel::warn: {
        level = "WARN";
        break;
    }
    default: {
        level = "INVALID_LEVEL";
        break;
    }
    }
    return level;
}
