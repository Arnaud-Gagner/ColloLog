#ifndef COLLOLOG_H
#define COLLOLOG_H

#include "ColloLog/Enums.h"

namespace collog
{
    void init(const char* path, FileOpen mode = FileOpen::Append);

    void setlevel(const LogLevel& lvl);

    void crit(const char* msg, FlushStrat strat = FlushStrat::AutoAsync);
    void debug(const char* msg, FlushStrat strat = FlushStrat::AutoAsync);
    void info(const char* msg, FlushStrat strat = FlushStrat::AutoAsync);
    void warn(const char* msg, FlushStrat strat = FlushStrat::AutoAsync);

    void flush();
    void clear();

}; // collog

namespace localog
{
    void init(const char* path, FileOpen mode = FileOpen::Append);

    void setlevel(const LogLevel& lvl);

    void crit(const char* msg, FlushStrat strat = FlushStrat::AutoAsync);
    void debug(const char* msg, FlushStrat strat = FlushStrat::AutoAsync);
    void info(const char* msg, FlushStrat strat = FlushStrat::AutoAsync);
    void warn(const char* msg, FlushStrat strat = FlushStrat::AutoAsync);

    void flush();
    void clear();

}; // localog

#endif // COLLOLOG_H
