#include "log_appender.h"

namespace zh
{

void StdoutLogAppender::log(LogEvent& event)
{
    std::cout << m_format->format(event) << std::endl;
}

FileLogAppender::FileLogAppender(std::string& path, LogFormatter::ptr& format_ptr) : 
    m_path(path),
    m_format_ptr(format_ptr)
{
    m_file_stream.open(m_path);
}

void FileLogAppender::log(LogEvent& event)
{
    //重新打开文件
    m_file_stream.close();
    m_file_stream.open(m_path);
    m_file_stream << m_format_ptr->format(event);
}


    
}