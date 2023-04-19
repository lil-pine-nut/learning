#include "Log.h"
#include "Log-0-9-7.h"
#include <stdio.h>
#include <unistd.h>

#ifdef USE_LOG4CXX
#undef USE_LOG4CXX_0_9_7
#endif

int main()
{
#ifdef USE_LOG4CXX
    // 初始化选择配置文件和输出日志目录
    // InitLogger("", "", "DEBUG");
    InitLogger("log4cxx.properties", "", "");

    int param = 0;
    // while (1)
    {
        LOG_TRACE("log a str: hello log4cxx");
        LOG_TRACE2("this is param is: %d", param++);
        LOG_DEBUG("log a str: hello log4cxx");
        LOG_DEBUG2("this is param is: %d", param++);
        LOG_INFO("log a str: hello log4cxx");
        LOG_INFO2("this is param is: %d", param++);
        LOG("log a str: hello log4cxx");
        LOG2("this is param is: %d", param++);
        LOG_WARN("log a str: hello log4cxx");
        LOG_WARN2("this is param is: %d", param++);
        LOG_ERROR("log a str: hello log4cxx");
        LOG_ERROR2("this is param is: %d", param++);
        LOG_FATAL("log a str: hello log4cxx");
        LOG_FATAL2("this is param is: %d", param++);

        LOG_DEBUG2("this is a words : %s %d %s", "今天是疯狂星期", 4, ".");
        // usleep(100000);
    }

#endif

#ifdef USE_LOG4CXX_0_9_7
    // 初始化选择配置文件和输出日志目录
    // InitLogger("", "", "DEBUG");
    InitLogger("log4cxx-0.9.7.properties", "", "DEBUG");

    int param = 0;
    // while (1)
    {
        LOG_DEBUG("log a str: hello log4cxx");
        LOG_DEBUG2("this is param is: %d", param++);
        LOG_INFO("log a str: hello log4cxx");
        LOG_INFO2("this is param is: %d", param++);
        LOG("log a str: hello log4cxx");
        LOG2("this is param is: %d", param++);
        LOG_WARN("log a str: hello log4cxx");
        LOG_WARN2("this is param is: %d", param++);
        LOG_ERROR("log a str: hello log4cxx");
        LOG_ERROR2("this is param is: %d", param++);
        LOG_FATAL("log a str: hello log4cxx");
        LOG_FATAL2("this is param is: %d", param++);

        LOG_DEBUG2("this is a words : %s %d %s", "今天是疯狂星期", 4, ".");
        usleep(100000);
    }
#endif
}