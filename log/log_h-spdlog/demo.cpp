#include "Log.h"

struct demo
{
    void test()
    {
#ifdef USE_SPDLOG
// LOG_DEBUG2("this is debug log record, param: {}", 10);
#endif
    }
};

int main()
{
#ifdef USE_SPDLOG
    // 此 %s 是为了在 LOG_XXX2 中打印类名， 其实真的有必要吗？
    InitLogger("./logs", "trace", "%Y-%m-%d %H:%M:%S.%f [%l] [%s] %v");

    int param = 1;
    LOG_DEBUG("this is debug log record, param: {}", ++param);
    LOG_DEBUG2("nalgnlanglangla");

    string str = "nihao";
    LOG_DEBUG2("debug string: {}", str);

    demo demo22;
    demo22.test();

    int a_num = 0;
    LOG_TRACE("log a num: {}", a_num++);
    LOG_TRACE2("log a num: {}", a_num++);
    LOG_DEBUG("log a num: {}", a_num++);
    LOG_DEBUG2("log a num: {}", a_num++);
    LOG_INFO("log a num: {}", a_num++);
    LOG_INFO2("log a num: {}", a_num++);
    LOG("log a num: {}", a_num++);
    LOG2("log a num: {}", a_num++);
    LOG_WARN("log a num: {}", a_num++);
    LOG_WARN2("log a num: {}", a_num++);
    LOG_ERROR("log a num: {}", a_num++);
    LOG_ERROR2("log a num: {}", a_num++);
    LOG_CRITICAL("log a num: {}", a_num++);
    LOG_CRITICAL2("log a num: {}", a_num++);

    DropLogger();
#endif
}