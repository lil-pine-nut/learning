#ifndef LOG_MACRO_H
#define LOG_MACRO_H

#include <string>
#include <stdio.h>

#define GET_CLASS_NAME (getClassName(__PRETTY_FUNCTION__).data())

#define LogMsg(logLevel, format, ...) handleLogMsg1(logLevel, __FILE__, __FUNCTION__, __LINE__, format, ##__VA_ARGS__)

#define LogMsg2(logLevel, format, ...) handleLogMsg2(logLevel, GET_CLASS_NAME, format, ##__VA_ARGS__)

const char LogTip[][8] = {"", "Debug", "Info", "Warn", "Error", "Fatal"};

#define LogMsg3(logLevel, format, ...)                                             \
    {                                                                              \
        char log[2048];                                                            \
        snprintf(log, sizeof(log), "[%s:%d][%s] [%s] " format, __FILE__, __LINE__, \
                 __FUNCTION__, GET_CLASS_NAME, ##__VA_ARGS__);                     \
        std::cout << log << std::endl;                                             \
    }

typedef enum enLogLevel
{
    LOG_DEBUG = 1,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
    LOG_FATAL
} LogLevel;

//回调函数
typedef void (*Callback)(const char *msg, int msg_len);

/**
 * @brief 设置回调函数
 *
 * @param callback
 */
void SetCallback(Callback callback);

/**
 * @brief
 *
 * @param logLevel      log等级
 * @param filename_in   文件名
 * @param funcname_in   函数名
 * @param line_in       行号
 * @param format        字符串
 * @param ...
 */
void handleLogMsg1(LogLevel logLevel, const char *filename_in, const char *funcname_in, int line_in, const char *format, ...);
void handleLogMsg2(LogLevel logLevel, const char *classname_in, const char *format, ...);

/**
 * @brief 获取类名，当仅为非类的成员函数时，获取函数名
 *
 * @param pPrettyFunc
 * @return string
 */
std::string getClassName(const char *pPrettyFunc);

#endif