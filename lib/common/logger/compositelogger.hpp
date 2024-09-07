#ifndef COMMON_COMPOSITELOGGER_HPP
#define COMMON_COMPOSITELOGGER_HPP

#include <list>
#include <memory>

#include "logger.hpp"


namespace common {

///////////////////////////////////////////////////////////////////////////////
class CompositeLogger : public cdba::sec21::Logger {
public:
    void add(std::shared_ptr<cdba::sec21::Logger> l) {
        m_loggers.push_back(l);
    }

public:
    void critical(const std::string&) override;
    void debug(const std::string&) override;
    void error(const std::string&) override;
    void info(const std::string&) override;
    void warn(const std::string&) override;

private:
    std::list<std::shared_ptr<cdba::sec21::Logger>> m_loggers;
};

}

#endif
