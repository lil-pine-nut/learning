#include "Log.h"
#include <iostream>
#include "spdlog/sinks/daily_file_sink.h"
#include "spdlog/sinks/stdout_sinks.h"
#include "spdlog/sinks/stdout_color_sinks.h"

#ifdef USE_SPDLOG
shared_ptr<logger> g_logger = NULL;
bool InitLogger(string logdir, string level, string pattern)
{
    if (logdir.empty())
        logdir = "./logs";
    if (level.empty())
        level = "debug";
    try
    {
        // 参考： https://blog.gmem.cc/spdlog
        /* 通过multi-sink的方式创建复合logger，实现方式为：先分别创建daily sink和控制台sink，
        并将两者放入sink 向量中，组成一个复合logger */

	/* daily sink */
	// base_filename: 指定日志文件的基础名称。实际的日志文件名将包括日期信息，例如 "base_filename_2024-01-24.txt"。
	// rotation_hour: 指定每天进行日志滚动的小时数（24小时制）。默认值为 0。
	// rotation_minute: 指定每天进行日志滚动的分钟数。默认值为 0。
	// truncate: 如果设置为 true，则在滚动日志时截断已存在的日志文件而不是追加。默认值为 false。重启软件输出日志时，true会清空改天日志，false是追加
	// max_files: 指定保留日志文件的最大数量。超过这个数量的日志文件将被删除。默认值为 std::numeric_limits<int>::max()，即不限制日志文件数量。
	// auto daily_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(logdir+"/log.log", 0, 0, false, 30);
	auto daily_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>(logdir+"/log.log", 0, 0);
	/* 控制台sink */
	auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

	daily_sink->set_pattern(pattern);
	// daily_sink->set_pattern("[%Y-%m-%d %T][%l]%v");

	console_sink->set_pattern(pattern);
	// %^%l%$：日志级别，%^ 和 %$ 之间的文本会被着色
    	// console_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e][%^%l%$][%s %!:%#] %v");
    	// console_sink->set_pattern("%+"); //默认格式的日志等级在控制台会输出颜色

	/* Sink组合 */
	std::vector<spdlog::sink_ptr> sinks;
	sinks.push_back(daily_sink);
	sinks.push_back(console_sink);

        g_logger = std::make_shared<spdlog::logger>("multi-sink", begin(sinks), end(sinks));
        spdlog::register_logger( g_logger );

        if (level == "trace")
        {
            ////设置全局注册日志等级
            g_logger->set_level(spdlog::level::trace);
            //遇到日志等级及以上级别会立马将缓存的buffer写到文件中，底层调用是std::fflush(_fd)
            g_logger->flush_on(spdlog::level::trace);
        }
        else if (level == "debug")
        {
            g_logger->set_level(spdlog::level::debug);
            g_logger->flush_on(spdlog::level::debug);
        }
        else if (level == "info")
        {
            g_logger->set_level(spdlog::level::info);
            g_logger->flush_on(spdlog::level::info);
        }
        else if (level == "warn")
        {
            g_logger->set_level(spdlog::level::warn);
            g_logger->flush_on(spdlog::level::warn);
        }
        else if (level == "error")
        {
            g_logger->set_level(spdlog::level::err);
            g_logger->flush_on(spdlog::level::err);
        }
        else if (level == "critical")
        {
            g_logger->set_level(spdlog::level::critical);
            g_logger->flush_on(spdlog::level::critical);
        }
    }
    catch (const spdlog::spdlog_ex &ex)
    {
        std::cout << "Log initialization failed: " << ex.what() << std::endl;
    }
}

void DropLogger()
{
    // Release and close all loggers 把所有的log对象智能指针放置到unordered_map中去，然后调用clear函数
    spdlog::drop_all();
}
#endif


inline const char *strrnchr(const char *const str, const char *const str_end, const char ch)
{
    for (const char *ptr = str_end - 1; ptr >= str; --ptr)
    {
        if (*ptr == ch)
        {
            return ptr;
        }
    }
    return NULL;
}

/**
 * @brief 获取类名，当仅为非类的成员函数时，获取函数名
 *
 * @param pPrettyFunc
 * @return string
 */
string getClassName(const char *pPrettyFunc)
{
    const char *pRight = index(pPrettyFunc, ':');
    if (NULL == pRight)
    {
        pRight = index(pPrettyFunc, '(');
        if (NULL == pRight)
            return pPrettyFunc;
    }
    const char *pLeft = strrnchr(pPrettyFunc, pRight, ' ');
    if (NULL == pLeft)
        return string(pPrettyFunc, pRight - pPrettyFunc);

    return string(pLeft+1, pRight - pLeft - 1);
}
