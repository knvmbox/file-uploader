#ifndef COMMON_EMPTYLOGGER_HPP
#define COMMON_EMPTYLOGGER_HPP

#include <string>

#include "logger.hpp"


namespace common {

///////////////////////////////////////////////////////////////////////////////
class EmptyLogger : public Logger {
public:
    virtual void debug(const std::string&) {
    }
    virtual void info(const std::string&) {
    }
    virtual void warn(const std::string&) {
    }
    virtual void error(const std::string&) {
    }
    virtual void critical(const std::string&) {
    }
};


}

#endif
