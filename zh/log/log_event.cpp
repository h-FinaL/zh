#include "log_event.h"

namespace zh
{

LogEvent::LogEvent(
    const char* file_name, 
    int line,
    LogLevel::Level level,
    int thread_id, 
    int co_id) :
    m_file_name(file_name), 
    m_line(line),
    m_level(level),
    m_thread_id(thread_id),
    m_co_id(co_id)
{
    time(&m_time);
}

}