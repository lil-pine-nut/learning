#ifndef __LOG__H__
#define __LOG__H__

#ifdef USE_SPDLOG

#include "spdlog/spdlog.h"
#include <string>

using namespace std;
using namespace spdlog;

#define M_CLASS_NAME getClassName(__PRETTY_FUNCTION__).data()

extern shared_ptr<logger> g_logger;

bool InitLogger(string logdir, string level, string pattern = "%Y-%m-%d %H:%M:%S.%f [%l] [%@] %v"); // pattern = timestamp, filename and line number
void DropLogger();

//// LOG_XXX2 把文件名替换为自己实现的获取类名, 其实真的有必要吗？
#define LOG_TRACE(...) SPDLOG_LOGGER_CALL(g_logger, spdlog::level::trace, ##__VA_ARGS__)
#define LOG_TRACE2(...) g_logger->log(spdlog::source_loc{M_CLASS_NAME, __LINE__, SPDLOG_FUNCTION}, spdlog::level::trace, ##__VA_ARGS__)

#define LOG_DEBUG(...) SPDLOG_LOGGER_CALL(g_logger, spdlog::level::debug, ##__VA_ARGS__)
#define LOG_DEBUG2(...) g_logger->log(spdlog::source_loc{M_CLASS_NAME, __LINE__, SPDLOG_FUNCTION}, spdlog::level::debug, ##__VA_ARGS__)

#define LOG_INFO(...) SPDLOG_LOGGER_CALL(g_logger, spdlog::level::info, ##__VA_ARGS__)
#define LOG_INFO2(...) g_logger->log(spdlog::source_loc{M_CLASS_NAME, __LINE__, SPDLOG_FUNCTION}, spdlog::level::info, ##__VA_ARGS__)

#define LOG(...) SPDLOG_LOGGER_CALL(g_logger, spdlog::level::info, ##__VA_ARGS__)
#define LOG2(...) g_logger->log(spdlog::source_loc{M_CLASS_NAME, __LINE__, SPDLOG_FUNCTION}, spdlog::level::info, ##__VA_ARGS__)

#define LOG_WARN(...) SPDLOG_LOGGER_CALL(g_logger, spdlog::level::warn, ##__VA_ARGS__)
#define LOG_WARN2(...) g_logger->log(spdlog::source_loc{M_CLASS_NAME, __LINE__, SPDLOG_FUNCTION}, spdlog::level::warn, ##__VA_ARGS__)

#define LOG_ERROR(...) SPDLOG_LOGGER_CALL(g_logger, spdlog::level::err, ##__VA_ARGS__)
#define LOG_ERROR2(...) g_logger->log(spdlog::source_loc{M_CLASS_NAME, __LINE__, SPDLOG_FUNCTION}, spdlog::level::err, ##__VA_ARGS__)

#define LOG_CRITICAL(...) SPDLOG_LOGGER_CALL(g_logger, spdlog::level::critical, ##__VA_ARGS__)
#define LOG_CRITICAL2(...) g_logger->log(spdlog::source_loc{M_CLASS_NAME, __LINE__, SPDLOG_FUNCTION}, spdlog::level::critical, ##__VA_ARGS__)

#endif

string getClassName(const char *pPrettyFunc);

#endif