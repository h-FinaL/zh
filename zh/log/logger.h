#ifndef __ZH_LOGGER_H__
#define __ZH_LOGGER_H__

#include <memory>
#include <vector>

#include "log_level.h"
#include "log_appender.h"
#include "log_event.h"
#include "log_formatter.h"

//LogEvent(const char* file_name, int line, LogLevel::Level level, int thread_id = 0, int co_id = 0);

#define LOG(logger, level) LogEvent(&logger, __FILE__, __LINE__, level).get_stream()
#define LOG_END LogEvent::end

namespace zh
{

class Logger {

public:
    typedef std::shared_ptr<Logger> ptr;

    Logger(LogLevel::Level filer_level = LogLevel::Level::DEBUG);
    void log(LogEvent& event);

    void add_appender(LogAppender::ptr& appender);
    void set_filer_level(LogLevel::Level filer_level) { m_filer_level = filer_level; }

private:
    LogLevel::Level m_filer_level;   //过滤等级
    std::stringstream m_ss;        //日志内容
    std::vector<LogAppender::ptr> m_appender_ptr_vec;

};

}

#endif