#ifndef __ZH_Log_APPENDER_H__
#define __ZH_Log_APPENDER_H__

#include <string>
#include <memory>
#include <fstream>
#include <sstream> 
#include <iostream>

#include "log_event.h"
#include "log_formatter.h"

namespace zh
{
    

//日志输出
class LogAppender 
{
public:
    typedef std::shared_ptr<LogAppender> ptr;
    virtual ~LogAppender();

    virtual void log(LogEvent& event) = 0;

};

//输出到控制台
class StdoutLogAppender : public LogAppender 
{
public:
    StdoutLogAppender(LogFormatter::ptr format) : m_format(format) {};
    void log(LogEvent& event) override;

private:
    LogFormatter::ptr m_format;
};

//输出到文件
class FileLogAppender : public LogAppender 
{
public:
    FileLogAppender(std::string& path, LogFormatter::ptr& format_ptr);
    void log(LogEvent& event) override;

private:
    std::string m_path;
    std::ofstream m_file_stream;
    LogFormatter::ptr m_format_ptr;
};


}

#endif