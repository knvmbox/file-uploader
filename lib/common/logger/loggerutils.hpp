#ifndef COMMON_LOGGERUTILS_HPP
#define COMMON_LOGGERUTILS_HPP

#include <sstream>
#include <utility>

#include "logger.hpp"


namespace common {
namespace log {

//-----------------------------------------------------------------------------
static
void logger_wrapper(std::ostringstream&) {
}

//-----------------------------------------------------------------------------
template<typename T, typename... Args>
void logger_wrapper(std::ostringstream &s, T value, Args&&... args) {
    s <<value <<" ";
    logger_wrapper(s, std::forward<Args>(args)...);
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
template<typename Logger, typename... Args>
void critical(std::shared_ptr<Logger> &logger, Args&&... args) {
    std::ostringstream os;
    logger_wrapper(os, std::forward<Args>(args)...);

    logger->critical(os.str());
}

//-----------------------------------------------------------------------------
template<typename Logger, typename... Args>
void debug(std::shared_ptr<Logger> &logger, Args&&... args) {
    std::ostringstream os;
    logger_wrapper(os, std::forward<Args>(args)...);

    logger->debug(os.str());
}

//-----------------------------------------------------------------------------
template<typename Logger, typename... Args>
void error(std::shared_ptr<Logger> &logger, Args&&... args) {
    std::ostringstream os;
    logger_wrapper(os, std::forward<Args>(args)...);

    logger->error(os.str());
}

//-----------------------------------------------------------------------------
template<typename Logger, typename... Args>
void info(std::shared_ptr<Logger> &logger, Args&&... args) {
    std::ostringstream os;
    logger_wrapper(os, std::forward<Args>(args)...);

    logger->info(os.str());
}

//-----------------------------------------------------------------------------
template<typename Logger, typename... Args>
void warn(std::shared_ptr<Logger> &logger, Args&&... args) {
    std::ostringstream os;
    logger_wrapper(os, std::forward<Args>(args)...);

    logger->warn(os.str());
}


}}

#endif
