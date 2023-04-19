#ifndef __LOG__0__9__7__H__
#define __LOG__0__9__7__H__

#ifdef USE_LOG4CXX_0_9_7

#include <log4cxx/logger.h>
#include <string>

using namespace std;

#define M_CLASS_NAME getClassName(__PRETTY_FUNCTION__).data()

extern log4cxx::LoggerPtr *g_logger;

bool InitLogger(string ConfigureFile, string logdir, string level);

#define LOG_DEBUG(log)                                                                  \
	{                                                                                   \
		if ((*g_logger)->isDebugEnabled())                                              \
		{                                                                               \
			(*g_logger)->forcedLog(log4cxx::Level::DEBUG, log, M_CLASS_NAME, __LINE__); \
		}                                                                               \
	}
#define LOG_DEBUG2(x, args...)                                                          \
	{                                                                                   \
		if ((*g_logger)->isDebugEnabled())                                              \
		{                                                                               \
			char log[4096];                                                             \
			snprintf(log, sizeof(log), x, ##args);                                      \
			(*g_logger)->forcedLog(log4cxx::Level::DEBUG, log, M_CLASS_NAME, __LINE__); \
		}                                                                               \
	}

#define LOG_INFO(log)                                                                  \
	{                                                                                  \
		if ((*g_logger)->isInfoEnabled())                                              \
		{                                                                              \
			(*g_logger)->forcedLog(log4cxx::Level::INFO, log, M_CLASS_NAME, __LINE__); \
		}                                                                              \
	}
#define LOG_INFO2(x, args...)                                                          \
	{                                                                                  \
		if ((*g_logger)->isInfoEnabled())                                              \
		{                                                                              \
			char log[4096];                                                            \
			snprintf(log, sizeof(log), x, ##args);                                     \
			(*g_logger)->forcedLog(log4cxx::Level::INFO, log, M_CLASS_NAME, __LINE__); \
		}                                                                              \
	}

#define LOG(log)                                                                       \
	{                                                                                  \
		if ((*g_logger)->isInfoEnabled())                                              \
		{                                                                              \
			(*g_logger)->forcedLog(log4cxx::Level::INFO, log, M_CLASS_NAME, __LINE__); \
		}                                                                              \
	}
#define LOG2(x, args...)                                                               \
	{                                                                                  \
		if ((*g_logger)->isInfoEnabled())                                              \
		{                                                                              \
			char log[4096];                                                            \
			snprintf(log, sizeof(log), x, ##args);                                     \
			(*g_logger)->forcedLog(log4cxx::Level::INFO, log, M_CLASS_NAME, __LINE__); \
		}                                                                              \
	}

#define LOG_WARN(log)                                                                  \
	{                                                                                  \
		if ((*g_logger)->isWarnEnabled())                                              \
		{                                                                              \
			(*g_logger)->forcedLog(log4cxx::Level::WARN, log, M_CLASS_NAME, __LINE__); \
		}                                                                              \
	}
#define LOG_WARN2(x, args...)                                                          \
	{                                                                                  \
		if ((*g_logger)->isWarnEnabled())                                              \
		{                                                                              \
			char log[4096];                                                            \
			snprintf(log, sizeof(log), x, ##args);                                     \
			(*g_logger)->forcedLog(log4cxx::Level::WARN, log, M_CLASS_NAME, __LINE__); \
		}                                                                              \
	}

#define LOG_ERROR(log)                                                                  \
	{                                                                                   \
		if ((*g_logger)->isErrorEnabled())                                              \
		{                                                                               \
			(*g_logger)->forcedLog(log4cxx::Level::ERROR, log, M_CLASS_NAME, __LINE__); \
		}                                                                               \
	}
#define LOG_ERROR2(x, args...)                                                          \
	{                                                                                   \
		if ((*g_logger)->isErrorEnabled())                                              \
		{                                                                               \
			char log[4096];                                                             \
			snprintf(log, sizeof(log), x, ##args);                                      \
			(*g_logger)->forcedLog(log4cxx::Level::ERROR, log, M_CLASS_NAME, __LINE__); \
		}                                                                               \
	}

#define LOG_FATAL(log)                                                                  \
	{                                                                                   \
		if ((*g_logger)->isFatalEnabled())                                              \
		{                                                                               \
			(*g_logger)->forcedLog(log4cxx::Level::FATAL, log, M_CLASS_NAME, __LINE__); \
		}                                                                               \
	}
#define LOG_FATAL2(x, args...)                                                          \
	{                                                                                   \
		if ((*g_logger)->isFatalEnabled())                                              \
		{                                                                               \
			char log[4096];                                                             \
			snprintf(log, sizeof(log), x, ##args);                                      \
			(*g_logger)->forcedLog(log4cxx::Level::FATAL, log, M_CLASS_NAME, __LINE__); \
		}                                                                               \
	}

string getClassName(const char *pPrettyFunc);

#endif

#endif