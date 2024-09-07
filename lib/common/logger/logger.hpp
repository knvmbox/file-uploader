#ifndef COMMON_LOGGER_HPP
#define COMMON_LOGGER_HPP

#include <string>


namespace common {

///////////////////////////////////////////////////////////////////////////////
class Logger {
public:
    enum LoggerLevel {LOG_CRITICAL, LOG_ERROR, LOG_WARN, LOG_INFO, LOG_DEBUG};

public:
    virtual ~Logger() {
    }

    virtual void debug(const std::string &msg) = 0;
    virtual void info(const std::string &msg) = 0;
    virtual void warn(const std::string &msg) = 0;
    virtual void error(const std::string &msg) = 0;
    virtual void critical(const std::string &msg) = 0;
};

}

#endif
