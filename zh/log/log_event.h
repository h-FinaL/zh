#ifndef __ZH_LOG_EVENT_H__
#define __ZH_LOG_EVENT_H__

#include <memory>
#include <vector>

#include "log_level.h"
#include "log_appender.h"
#include "logger.h"

namespace zh
{

class LogEvent
{
private:
    struct End{};  //终止流
public:
    typedef std::shared_ptr<LogEvent> ptr;
    LogEvent(Logger* logger, const char* file_name, int line, LogLevel::Level level, int thread_id = 0, int co_id = 0);

    LogEvent& operator<<(const char* str) { m_ss << str; }
    LogEvent& operator<<(std::string& str) { m_ss << str; }
    LogEvent& operator<<(int str) { m_ss << str; }
    LogEvent& operator<<(long str) { m_ss << str; }
    void operator<<(End);

    const char* get_file()          { return m_file_name; }
    int get_line()                  { return m_line; }
    int get_thread_id()             { return m_thread_id; }
    int get_co_id()                 { return m_co_id; }
    long get_time()                 { return m_time; }
    LogLevel::Level get_level()     { return m_level; }
    std::string get_content()       { return m_ss.str(); }
    std::stringstream& get_stream() { return m_ss; }

    End end;
private:
    const char* m_file_name;   // 文件名
    int m_line;                // 行号
    int m_thread_id;           // 线程id
    int m_co_id;               // 协程id
    long m_time;               // 时间戳
    std::stringstream m_ss;    // 日志内容
    LogLevel::Level m_level;   // 等级
    Logger* m_logger;           
};


}


#endif