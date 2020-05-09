#include "logger.h"

namespace zh
{

Logger::Logger(LogLevel::Level filer_level) : 
    m_filer_level(filer_level)
{
}

void Logger::log(LogEvent& event)
{
    if (event.get_level() >= m_filer_level)
    {
        for (auto& i : m_appender_ptr_vec)
        {
            i->log(event);
        }
    }
}

void Logger::add_appender(LogAppender::ptr& appender)
{
    m_appender_ptr_vec.push_back(appender);
}

}