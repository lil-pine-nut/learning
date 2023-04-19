#ifndef __LOG__H__
#define __LOG__H__

#define USE_LOG4CPLUS
#ifdef USE_LOG4CPLUS

#include <log4cplus/logger.h>
#include <log4cplus/loglevel.h>
#include <log4cplus/loggingmacros.h>
#include <string>

using namespace std;

#define M_CLASS_NAME getClassName(__PRETTY_FUNCTION__).data()

extern log4cplus::Logger *g_logger;

bool InitLogger(string ConfigureFile, string logdir, string level);
void DropLogger();

#define LOG4CPLUS_MACRO_BODY2(logger, logEvent, logLevel)                                  \
    LOG4CPLUS_SUPPRESS_DOWHILE_WARNING()                                                   \
    do                                                                                     \
    {                                                                                      \
        log4cplus::Logger const &_l = log4cplus::detail::macros_get_logger(logger);        \
        if (LOG4CPLUS_MACRO_LOGLEVEL_PRED(                                                 \
                _l.isEnabledFor(log4cplus::logLevel), logLevel))                           \
        {                                                                                  \
            LOG4CPLUS_MACRO_INSTANTIATE_OSTRINGSTREAM(_log4cplus_buf);                     \
            _log4cplus_buf << logEvent;                                                    \
            log4cplus::detail::macro_forced_log(_l,                                        \
                                                log4cplus::logLevel, _log4cplus_buf.str(), \
                                                __FILE__, __LINE__,                        \
                                                M_CLASS_NAME);                             \
        }                                                                                  \
    } while (0)                                                                            \
        LOG4CPLUS_RESTORE_DOWHILE_WARNING()

//// LOG_XXX2 把函数名替换为自己实现的获取类名, 其实真的有必要吗？

#define LOG_TRACE(...) LOG4CPLUS_MACRO_BODY(*g_logger, __VA_ARGS__, TRACE_LOG_LEVEL)
#define LOG_TRACE2(...) LOG4CPLUS_MACRO_BODY2(*g_logger, __VA_ARGS__, TRACE_LOG_LEVEL)

// #define LOG_DEBUG(...) LOG4CPLUS_DEBUG(*g_logger, __VA_ARGS__)
#define LOG_DEBUG(...) LOG4CPLUS_MACRO_BODY(*g_logger, __VA_ARGS__, DEBUG_LOG_LEVEL)
#define LOG_DEBUG2(...) LOG4CPLUS_MACRO_BODY2(*g_logger, __VA_ARGS__, DEBUG_LOG_LEVEL)

#define LOG_INFO(...) LOG4CPLUS_MACRO_BODY(*g_logger, __VA_ARGS__, INFO_LOG_LEVEL)
#define LOG_INFO2(...) LOG4CPLUS_MACRO_BODY2(*g_logger, __VA_ARGS__, INFO_LOG_LEVEL)

#define LOG(...) LOG4CPLUS_MACRO_BODY(*g_logger, __VA_ARGS__, INFO_LOG_LEVEL)
#define LOG2(...) LOG4CPLUS_MACRO_BODY2(*g_logger, __VA_ARGS__, INFO_LOG_LEVEL)

#define LOG_WARN(...) LOG4CPLUS_MACRO_BODY(*g_logger, __VA_ARGS__, WARN_LOG_LEVEL)
#define LOG_WARN2(...) LOG4CPLUS_MACRO_BODY2(*g_logger, __VA_ARGS__, WARN_LOG_LEVEL)

#define LOG_ERROR(...) LOG4CPLUS_MACRO_BODY(*g_logger, __VA_ARGS__, ERROR_LOG_LEVEL)
#define LOG_ERROR2(...) LOG4CPLUS_MACRO_BODY2(*g_logger, __VA_ARGS__, ERROR_LOG_LEVEL)

#define LOG_FATAL(...) LOG4CPLUS_MACRO_BODY(*g_logger, __VA_ARGS__, FATAL_LOG_LEVEL)
#define LOG_FATAL2(...) LOG4CPLUS_MACRO_BODY2(*g_logger, __VA_ARGS__, FATAL_LOG_LEVEL)

#endif

string getClassName(const char *pPrettyFunc);

#endif