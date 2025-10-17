#include "ColloLog/Enums.h"

const char* levelToCString(const LogLevel& lvl)
{
    switch (lvl) {
        case LogLevel::Debug: {
            return " DBUG ";
        }
        case LogLevel::Info: {
            return " INFO ";
        }
        case LogLevel::Crit: {
            return " CRIT ";
        }
        case LogLevel::Warn: {
            return " WARN ";
        }
        default: {
            return "EROR";
        }
    }
}
