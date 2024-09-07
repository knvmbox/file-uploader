#include <algorithm>

#include "compositelogger.hpp"


namespace common {

//-----------------------------------------------------------------------------
void CompositeLogger::critical(const std::string &msg)
{
    std::for_each(m_loggers.begin(), m_loggers.end(), [&msg](std::shared_ptr<Logger> &l){
        l->critical(msg);
    });
}

//-----------------------------------------------------------------------------
void CompositeLogger::debug(const std::string &msg)
{
    std::for_each(m_loggers.begin(), m_loggers.end(), [&msg](std::shared_ptr<Logger> &l){
        l->debug(msg);
    });
}

//-----------------------------------------------------------------------------
void CompositeLogger::error(const std::string &msg)
{
    std::for_each(m_loggers.begin(), m_loggers.end(), [&msg](std::shared_ptr<Logger> &l){
        l->error(msg);
    });
}

//-----------------------------------------------------------------------------
void CompositeLogger::info(const std::string &msg)
{
    std::for_each(m_loggers.begin(), m_loggers.end(), [&msg](std::shared_ptr<Logger> &l){
        l->info(msg);
    });
}

//-----------------------------------------------------------------------------
void CompositeLogger::warn(const std::string &msg)
{
    std::for_each(m_loggers.begin(), m_loggers.end(), [&msg](std::shared_ptr<Logger> &l){
        l->warn(msg);
    });
}

}
