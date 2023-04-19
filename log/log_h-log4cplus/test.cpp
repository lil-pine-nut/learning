#include "Log.h"
#include <stdio.h>
#include <unistd.h>

int main()
{
#ifdef USE_LOG4CPLUS
    // 初始化选择配置文件和输出日志目录
    InitLogger("", "", "");
    // InitLogger("log4cplus.properties.2", "", "");

    int param = 0;
    // while (1)
    {
        LOG_TRACE("this is param is:" << param++);
        LOG_TRACE2("this is param is:" << param++);
        LOG_DEBUG("this is param is:" << param++);
        LOG_DEBUG2("this is param is:" << param++);
        LOG_INFO("this is param is:" << param++);
        LOG_INFO2("this is param is:" << param++);
        LOG("this is param is:" << param++);
        LOG2("this is param is:" << param++);
        LOG_WARN("this is param is:" << param++);
        LOG_WARN2("this is param is:" << param++);
        LOG_ERROR("this is param is:" << param++);
        LOG_ERROR2("this is param is:" << param++);
        LOG_FATAL("this is param is:" << param++);
        LOG_FATAL2("this is param is:" << param++);

        LOG_DEBUG2("log a str:"
                   << "this is a string: hello!");

        // usleep(100000);
    }

    DropLogger();
#endif
}