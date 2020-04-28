#ifndef __ZH_LOG_FORMATTER_H__
#define __ZH_LOG_FORMATTER_H__

#include <string>
#include <memory>
#include <vector>
#include <unordered_map>

#include "log_event.h"

namespace zh
{
    

//日志格式化
class LogFormatter 
{
public:
    typedef std::shared_ptr<LogFormatter> ptr;

    LogFormatter(std::string& pattern);
    void init();  
    std::string format(LogEvent& event);

private:
    std::string m_pattern;
    std::vector<FormatterItem::ptr> m_item_ptr_vec; 
    static std::unordered_map<char, FormatterItem::ptr> m_item_ptr_map;
};

class FormatterItem
{
public:
    typedef std::shared_ptr<FormatterItem> ptr;
    FormatterItem();
    virtual void format(LogEvent& event) = 0;
};

//非转义字符
class StringFormatterItem : public FormatterItem
{
public:
    StringFormatterItem(const std::string& s) : m_str(s){}
    void format(LogEvent& event) { event.get_stream() << m_str; }
private:
    std::string m_str;
};

//%m 日志内容
class MessageFormatItem : public FormatterItem
{
public:
    MessageFormatItem();
    void format(LogEvent& event) { event.get_stream() << event.get_content(); }
};

//%t 线程id
class ThreadIdFormatterItem : public FormatterItem
{
public:
    ThreadIdFormatterItem();
    void format(LogEvent& event) { event.get_stream() << event.get_thread_id(); }
};

//%F 协程id
class CoroutineIdFormatItem : public FormatterItem
{
public:
    CoroutineIdFormatItem();
    void format(LogEvent& event) { event.get_stream() << event.get_co_id(); }
};

//%p 等级
class LevelFormatItem : public FormatterItem
{
public:
    LevelFormatItem();
    void format(LogEvent& event) { event.get_stream() << LogLevel::getLevel(event.get_level());}
};

//%l 行号
class LineFormatterItem : public FormatterItem
{
public:
    LineFormatterItem();
    void format(LogEvent& event) { event.get_stream() << event.get_line(); };
};

//%D 时间输出格式%Y-%m-%d %H:%M:%S
class DataTimeFormatterItem : public FormatterItem
{
public:
    DataTimeFormatterItem();
    void format(LogEvent& event)
    {
        struct tm tm;
        char format[] = "%Y-%m-%d %H:%M:%S";
        time_t time = event.get_time();
        localtime_r(&time, &tm);
        char buf[64];
        strftime(buf, sizeof(buf), format, &tm);
        event.get_stream() << buf;
    }
};


}

#endif