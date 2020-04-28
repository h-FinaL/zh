#ifndef __ZH_LOG_EVENT_H__
#define __ZH_LOG_EVENT_H__

#include <memory>
#include <vector>

#include "log_level.h"
#include "log_appender.h"

namespace zh
{

class LogEvent
{
public:
    typedef std::shared_ptr<LogEvent> ptr;
    LogEvent(const char* file_name, int line, LogLevel::Level level, int thread_id = 0, int co_id = 0);

    const char* get_file()          { return m_file_name; }
    int get_line()                  { return m_line; }
    int get_thread_id()             { return m_thread_id; }
    int get_co_id()                 { return m_co_id; }
    long get_time()                 { return m_time; }
    LogLevel::Level get_level()     { return m_level; }
    std::string get_content()       { return m_ss.str(); }
    std::stringstream& get_stream() { return m_ss; }

private:
    const char* m_file_name;   // 文件名
    int m_line;                // 行号
    int m_thread_id;           // 线程id
    int m_co_id;               // 协程id
    long m_time;               // 时间戳
    std::stringstream m_ss;    // 日志内容
    LogLevel::Level m_level;   // 等级
};


}


#endif