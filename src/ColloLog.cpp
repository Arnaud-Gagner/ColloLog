#include "ColloLog.h"

#include "ColloLog/ColloLogger.h"
#include "ColloLog/LocalLogger.h"

#include <memory>
#include <mutex>

namespace
{
    std::unique_ptr<ColloLogger> collogInstance;
    std::once_flag initFlag;

    ColloLogger& instance()
    {
        if (!collogInstance) {
            throw std::runtime_error(
              "Logger not initialized. Call collolog::init() first.");
        }
        return *collogInstance;
    }
};

namespace
{
    std::unique_ptr<LocalLogger> localogInstance;
    std::once_flag localInitFlag;

    LocalLogger& localInstance()
    {
        if (!localogInstance) {
            throw std::runtime_error(
              "Logger not initialized. Call collolog::init() first.");
        }
        return *localogInstance;
    }
};

void collog::init(const char* path, FileOpen mode)
{
    std::call_once(initFlag,
                   [&]() { collogInstance = std::make_unique<ColloLogger>(path, mode); });

    if (!collogInstance) {
        throw std::runtime_error("Logger failed to initialize.");
    }
}

void collog::setLevel(const LogLevel& lvl)
{
    instance().setLevel(lvl);
}

void collog::crit(const char* msg, FlushStrat strat)
{
    instance().crit(msg, strat);
}

void collog::debug(const char* msg, FlushStrat strat)
{
    instance().debug(msg, strat);
}

void collog::info(const char* msg, FlushStrat strat)
{
    instance().info(msg, strat);
}

void collog::warn(const char* msg, FlushStrat strat)
{
    instance().warn(msg, strat);
}

void collog::flush()
{
    instance().flush();
}

void collog::clear()
{
    instance().clear();
}

void localog::init(const char* path, FileOpen mode) {
        std::call_once(localInitFlag,
                   [&]() { localogInstance = std::make_unique<LocalLogger>(path, mode); });

    if (!localogInstance) {
        throw std::runtime_error("Logger failed to initialize.");
    }
}

void localog::setlevel(const LogLevel& lvl) {
    localInstance().setlevel(lvl);
}

void localog::crit(const char* msg, FlushStrat strat) {
    localInstance().crit(msg, strat);
}

void localog::debug(const char* msg, FlushStrat strat) {
    localInstance().debug(msg, strat);

}

void localog::info(const char* msg, FlushStrat strat) {
    localInstance().info(msg, strat);

}

void localog::warn(const char* msg, FlushStrat strat) {
    localInstance().warn(msg, strat);

}

void localog::flush() {
    localInstance().flush();
}

void localog::clear() {
    localInstance().clear();
}