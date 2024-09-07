#ifndef COMMON_LOGGERFACTORY_HPP
#define COMMON_LOGGERFACTORY_HPP

#include <memory>

#include "logger.hpp"


namespace common {

///////////////////////////////////////////////////////////////////////////////
class LoggerFactory : public Logger {
public:
    static std::shared_ptr<Logger> instance();
    static void setupLogger(Logger *logger);

public:
    void debug(const std::string &m) override {
        m_logger->debug(m);
    }
    void info(const std::string &m) override {
        m_logger->info(m);
    }
    void warn(const std::string &m) override {
        m_logger->warn(m);
    }
    void error(const std::string &m) override {
        m_logger->error(m);
    }
    void critical(const std::string &m) override {
        m_logger->critical(m);
    }

    ~LoggerFactory();

private:
    LoggerFactory();

    void setLogger(Logger *logger) {
        m_logger.reset(logger);
    }

private:
    static std::shared_ptr<Logger> s_logger;

    std::unique_ptr<Logger> m_logger;
};


}

#endif
