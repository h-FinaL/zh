#ifndef __ZH_LOG_LEVEL_H__
#define __ZH_LOG_LEVEL_H__

namespace zh
{

class LogLevel 
{
public:
    enum Level 
    {
        DEBUG = 1,
        INFO = 2,
        WARN = 3,
        ERROR = 4,
        FATAL = 5
    };

    static const char* get_level(LogLevel::Level level);
};

}

#endif