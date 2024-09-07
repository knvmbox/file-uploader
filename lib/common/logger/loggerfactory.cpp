#include "emptylogger.hpp"
#include "loggerfactory.hpp"


namespace common {

///////////////////////////////////////////////////////////////////////////////
std::shared_ptr<Logger> LoggerFactory::s_logger;


//-----------------------------------------------------------------------------
std::shared_ptr<Logger> LoggerFactory::instance()
{
    if(s_logger.get() == nullptr)
    {
        s_logger.reset(new LoggerFactory());
        static_cast<LoggerFactory*>(s_logger.get())->setLogger(new EmptyLogger());
    }

    return s_logger;
}

//-----------------------------------------------------------------------------
void LoggerFactory::setupLogger(Logger *logger)
{
    if(s_logger.get() == nullptr)
        LoggerFactory::instance();

    static_cast<LoggerFactory*>(s_logger.get())->setLogger(logger);
}

//-----------------------------------------------------------------------------
LoggerFactory::LoggerFactory()
{
}

//-----------------------------------------------------------------------------
LoggerFactory::~LoggerFactory()
{
}

}

