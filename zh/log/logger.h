#ifndef __ZH_LOGGER_H__
#define __ZH_LOGGER_H__

#include <memory>
#include <vector>

#include "log_level.h"
#include "log_appender.h"
#include "log_event.h"
#include "log_formatter.h"

namespace zh
{

class Logger {

public:
    typedef std::shared_ptr<Logger> ptr;

    Logger(LogLevel::Level filer_level = LogLevel::Level::DEBUG);
    void log(LogLevel::Level level, LogEvent&& event);

    void debug(LogEvent&& event) { log(LogLevel::DEBUG, std::forward<LogEvent>(event)); }
    void info(LogEvent&& event) { log(LogLevel::INFO, std::forward<LogEvent>(event)); }
    void warn(LogEvent&& event) { log(LogLevel::WARN, std::forward<LogEvent>(event)); }
    void error(LogEvent&& event) { log(LogLevel::ERROR, std::forward<LogEvent>(event)); }
    void fatal(LogEvent&& event) { log(LogLevel::FATAL, std::forward<LogEvent>(event)); }

    void add_appender(LogAppender::ptr& appender);

    void set_filer_level(LogLevel::Level filer_level) { m_filer_level = filer_level; }
private:
    LogLevel::Level m_filer_level;   //过滤等级
    std::stringstream m_ss;        //日志内容
    std::vector<LogAppender::ptr> m_appender_ptr_vec;

};

}

#endif