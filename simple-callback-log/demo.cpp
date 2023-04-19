#include "log-macro.h"
#include <iostream>

void RealizeCallback(const char *msg, int msg_len)
{
    std::cout << msg;
}

int main()
{
    SetCallback(RealizeCallback);

    // LogMsg 不支持此种格式
    LogMsg(LOG_DEBUG, "Test1", "Add C variable parameters");
    LogMsg(LOG_DEBUG, "Test1", "Add C variable parameters", "Add microsecond conversion");

    LogMsg(LOG_DEBUG, "Hahahaha");
    LogMsg(LOG_DEBUG, "%s", "Hahahaha2");
    LogMsg(LOG_DEBUG, "%s %s", "Add C variable parameters", "Add microsecond conversion");
    LogMsg(LOG_DEBUG, "%s %s %s", "Add C variable parameters", "Add microsecond conversion", "test");

    int a_num = 0;
    LogMsg(LOG_DEBUG, "log a num: %d", ++a_num);
    LogMsg(LOG_INFO, "log a num: %d", ++a_num);
    LogMsg(LOG_WARN, "log a num: %d", ++a_num);
    LogMsg(LOG_ERROR, "log a num: %d", ++a_num);
    LogMsg(LOG_FATAL, "log a num: %d", ++a_num);
    LogMsg(LOG_DEBUG, "log a num: %d", ++a_num);
    LogMsg(LOG_DEBUG, "log a num: %d", ++a_num);
    LogMsg2(LOG_DEBUG, "printf something: %s %d", "hello?", 1);

    a_num = 0;
    LogMsg2(LOG_DEBUG, "log a num: %d", ++a_num);
    LogMsg2(LOG_INFO, "log a num: %d", ++a_num);
    LogMsg2(LOG_WARN, "log a num: %d", ++a_num);
    LogMsg2(LOG_ERROR, "log a num: %d", ++a_num);
    LogMsg2(LOG_FATAL, "log a num: %d", ++a_num);
    LogMsg2(LOG_DEBUG, "log a num: %d", ++a_num);
    LogMsg2(LOG_DEBUG, "log a num: %d", ++a_num);
    LogMsg2(LOG_DEBUG, "printf something: %s %d", "hello?", 1);

    a_num = 0;
    LogMsg3(LOG_DEBUG, "log a num: %d", ++a_num);
    LogMsg3(LOG_INFO, "log a num: %d", ++a_num);
    LogMsg3(LOG_WARN, "log a num: %d", ++a_num);
    LogMsg3(LOG_ERROR, "log a num: %d", ++a_num);
    LogMsg3(LOG_FATAL, "log a num: %d", ++a_num);
    LogMsg3(LOG_DEBUG, "log a num: %d", ++a_num);
    LogMsg3(LOG_DEBUG, "log a num: %d", ++a_num);
    LogMsg3(LOG_DEBUG, "printf something: %s %d", "hello?", 1);
}