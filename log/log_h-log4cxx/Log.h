#ifndef __LOG__H__
#define __LOG__H__

#ifdef USE_LOG4CXX

#include <log4cxx/logger.h>
#include <string>

using namespace std;

#define M_CLASS_NAME getClassName(__PRETTY_FUNCTION__).data()

extern log4cxx::LoggerPtr *g_logger;

bool InitLogger(string ConfigureFile, string logdir, string level);

//// LOG4CXX_LOCATION2 把函数名替换为自己实现的获取类名, 其实真的有必要吗？
#define LOG4CXX_LOCATION2 ::log4cxx::spi::LocationInfo(__FILE__,     \
													   M_CLASS_NAME, \
													   __LINE__)

#define LOG_TRACE(log)                                                                  \
	{                                                                                   \
		if ((*g_logger)->isTraceEnabled())                                              \
		{                                                                               \
			(*g_logger)->forcedLog(log4cxx::Level::getTrace(), log, LOG4CXX_LOCATION2); \
		}                                                                               \
	}
#define LOG_TRACE2(x, args...)                                                          \
	{                                                                                   \
		if ((*g_logger)->isTraceEnabled())                                              \
		{                                                                               \
			char log[4096];                                                             \
			snprintf(log, sizeof(log), x, ##args);                                      \
			(*g_logger)->forcedLog(log4cxx::Level::getTrace(), log, LOG4CXX_LOCATION2); \
		}                                                                               \
	}

#define LOG_DEBUG(log)                                                                  \
	{                                                                                   \
		if ((*g_logger)->isDebugEnabled())                                              \
		{                                                                               \
			(*g_logger)->forcedLog(log4cxx::Level::getDebug(), log, LOG4CXX_LOCATION2); \
		}                                                                               \
	}
#define LOG_DEBUG2(x, args...)                                                          \
	{                                                                                   \
		if ((*g_logger)->isDebugEnabled())                                              \
		{                                                                               \
			char log[4096];                                                             \
			snprintf(log, sizeof(log), x, ##args);                                      \
			(*g_logger)->forcedLog(log4cxx::Level::getDebug(), log, LOG4CXX_LOCATION2); \
		}                                                                               \
	}

#define LOG_INFO(log)                                                                  \
	{                                                                                  \
		if ((*g_logger)->isInfoEnabled())                                              \
		{                                                                              \
			(*g_logger)->forcedLog(log4cxx::Level::getInfo(), log, LOG4CXX_LOCATION2); \
		}                                                                              \
	}
#define LOG_INFO2(x, args...)                                                          \
	{                                                                                  \
		if ((*g_logger)->isInfoEnabled())                                              \
		{                                                                              \
			char log[4096];                                                            \
			snprintf(log, sizeof(log), x, ##args);                                     \
			(*g_logger)->forcedLog(log4cxx::Level::getInfo(), log, LOG4CXX_LOCATION2); \
		}                                                                              \
	}

#define LOG(log)                                                                       \
	{                                                                                  \
		if ((*g_logger)->isInfoEnabled())                                              \
		{                                                                              \
			(*g_logger)->forcedLog(log4cxx::Level::getInfo(), log, LOG4CXX_LOCATION2); \
		}                                                                              \
	}
#define LOG2(x, args...)                                                               \
	{                                                                                  \
		if ((*g_logger)->isInfoEnabled())                                              \
		{                                                                              \
			char log[4096];                                                            \
			snprintf(log, sizeof(log), x, ##args);                                     \
			(*g_logger)->forcedLog(log4cxx::Level::getInfo(), log, LOG4CXX_LOCATION2); \
		}                                                                              \
	}

#define LOG_WARN(log)                                                                  \
	{                                                                                  \
		if ((*g_logger)->isWarnEnabled())                                              \
		{                                                                              \
			(*g_logger)->forcedLog(log4cxx::Level::getWarn(), log, LOG4CXX_LOCATION2); \
		}                                                                              \
	}
#define LOG_WARN2(x, args...)                                                          \
	{                                                                                  \
		if ((*g_logger)->isWarnEnabled())                                              \
		{                                                                              \
			char log[4096];                                                            \
			snprintf(log, sizeof(log), x, ##args);                                     \
			(*g_logger)->forcedLog(log4cxx::Level::getWarn(), log, LOG4CXX_LOCATION2); \
		}                                                                              \
	}

#define LOG_ERROR(log)                                                                  \
	{                                                                                   \
		if ((*g_logger)->isErrorEnabled())                                              \
		{                                                                               \
			(*g_logger)->forcedLog(log4cxx::Level::getError(), log, LOG4CXX_LOCATION2); \
		}                                                                               \
	}
#define LOG_ERROR2(x, args...)                                                          \
	{                                                                                   \
		if ((*g_logger)->isErrorEnabled())                                              \
		{                                                                               \
			char log[4096];                                                             \
			snprintf(log, sizeof(log), x, ##args);                                      \
			(*g_logger)->forcedLog(log4cxx::Level::getError(), log, LOG4CXX_LOCATION2); \
		}                                                                               \
	}

#define LOG_FATAL(log)                                                                  \
	{                                                                                   \
		if ((*g_logger)->isFatalEnabled())                                              \
		{                                                                               \
			(*g_logger)->forcedLog(log4cxx::Level::getFatal(), log, LOG4CXX_LOCATION2); \
		}                                                                               \
	}
#define LOG_FATAL2(x, args...)                                                          \
	{                                                                                   \
		if ((*g_logger)->isFatalEnabled())                                              \
		{                                                                               \
			char log[4096];                                                             \
			snprintf(log, sizeof(log), x, ##args);                                      \
			(*g_logger)->forcedLog(log4cxx::Level::getFatal(), log, LOG4CXX_LOCATION2); \
		}                                                                               \
	}

string getClassName(const char *pPrettyFunc);

#endif

#endif