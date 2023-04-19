
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <cstdarg>
#include <string>
#include <sys/time.h>
#include <unistd.h>

#include "log-macro.h"

using namespace std;

typedef unsigned long long guint64;
#define BEI_JING_TIME_ZONE 28800 // 8 * 60 * 60

Callback LogCallback = NULL;

// 获取当前微秒时间戳
guint64 GetCurrentMicroTime()
{
    struct timeval dwStart;
    gettimeofday(&dwStart, NULL);
    guint64 dwTime = 1000000 * dwStart.tv_sec + dwStart.tv_usec;
    return dwTime;
}

inline int isleap(int year)
{
    return (year) % 4 == 0 && ((year) % 100 != 0 || (year) % 400 == 0);
}

static time_t mon_yday[2][12] =
    {
        {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334},
        {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335},
};

const char Days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
// https://www.jianshu.com/p/fbb99644ade5 加上北京的时区
// 原版判断润年存在误差
void localtime2(unsigned long long time, struct tm *t)
{
    unsigned int Pass4year;
    int hours_per_year;
 
    if(time < 0)
    {
        time = 0;
    }
    time += BEI_JING_TIME_ZONE;
    //取秒时间
    t->tm_sec=(int)(time % 60);
    time /= 60;
    //取分钟时间
    t->tm_min=(int)(time % 60);
    time /= 60;
    //取过去多少个四年，每四年有 1461*24 小时
    Pass4year=((unsigned int)time / (1461L * 24L));
    //计算年份
    t->tm_year=(Pass4year << 2) + 1970;
    //四年中剩下的小时数
    time %= 1461L * 24L;
    //校正闰年影响的年份，计算一年中剩下的小时数
    for (;;)
    {
        //一年的小时数
        hours_per_year = 365 * 24;
        //判断闰年
        //if((t->tm_year & 3) == 0)
        if(isleap(t->tm_year))
        {
            //是闰年，一年则多24小时，即一天
            hours_per_year += 24;
        }
        if(time < hours_per_year)
        {
            break;
        }
        t->tm_year++;
        time -= hours_per_year;
    }
    //小时数
    t->tm_hour=(int)(time % 24);
    //一年中剩下的天数
    time /= 24;
    //假定为闰年
    time++;
    //校正闰年的误差，计算月份，日期
    //if((t->tm_year & 3) == 0)
    if(isleap(t->tm_year))
    {
        if(time > 60)
        {
            time--;
        }
        else
        {
            if(time == 60)
            {
                t->tm_mon = 1;
                t->tm_mday = 29;
                return ;
            }
        }
    }
    //计算月日
    for(t->tm_mon = 0; Days[t->tm_mon] < time; t->tm_mon++)
    {
        time -= Days[t->tm_mon];
    }
 
    t->tm_mday = (int)(time);

    return;
}

/**
 * @brief
 *
 * @param dwTime
 * @return string
 */
string GetFormatMicroTime(guint64 dwTime)
{
    guint64 sec = dwTime / 1000000;
    guint64 ucec = dwTime % 1000000;
    struct tm tmTime;
    localtime2(sec, &tmTime);
    char szTime[64] = {0};
    // sprintf(szTime, "%04d-%02d-%02d %02d:%02d:%02d.%06llu",tmTime.tm_year,tmTime.tm_mon+1,
    // tmTime.tm_mday,tmTime.tm_hour,tmTime.tm_min,tmTime.tm_sec, ucec);

    // tmTime.tm_year 使用strftime需要根据struct tm的提示得知需要减掉 1900, 测试使用 strftime 会比 sprintf 快
    tmTime.tm_year -= 1900;
    strftime(szTime, sizeof(szTime), "%Y-%m-%d %H:%M:%S", &tmTime);
    sprintf(szTime + strlen(szTime), ".%06llu", ucec);
    return szTime;
}

/**
 * @brief    分割字符
 *
 * @param input_string      输入字符串
 * @param pattern           分隔符，可以把char改为string
 * @return std::string      最后一个字符串
 */
const char *split(std::string input_string, char pattern)
{
    std::string::size_type lastPos = input_string.find_last_of(pattern);
    if (lastPos != std::string::npos)
    {
        return input_string.substr(lastPos + 1, input_string.size() - lastPos).c_str();
    }
    else
    {
        return input_string.c_str();
    }
}

/**
 * @brief 上面那个才是不好....
 *
 * @param input_str
 * @return const char*
 */
inline const char *split_path(const char *input_str)
{
    const char *str = rindex(input_str, '/'); // strrchr从后往前找, strchr从前往后找, 二者找不到都返回NULL
    if (str == NULL)
        return input_str;
    return str + 1;
}

void handleLogMsg1(LogLevel logLevel, const char *filename_in, const char *funcname_in, int line_in, const char *format, ...)
{
    char logTxt[2048];
    memset(logTxt, 0, sizeof(logTxt));

    sprintf(logTxt, "%s %s %s() line:%d  : [%s] ", GetFormatMicroTime(GetCurrentMicroTime()).c_str(),
            split_path(filename_in), funcname_in, line_in, LogTip[logLevel]);

    /**根据可变参数合成字符串**/
    va_list argp;
    va_start(argp, format);
    vsprintf(logTxt + strlen(logTxt), format, argp);
    va_end(argp); /* 将argp置为NULL */
    sprintf(logTxt + strlen(logTxt), "\n");
    if (LogCallback != NULL)
    {
        LogCallback(logTxt, strlen(logTxt));
    }
    else
    {
        std::cout << logTxt;
    }
}

void handleLogMsg2(LogLevel logLevel, const char *classname_in, const char *format, ...)
{
    char logTxt[2048];
    memset(logTxt, 0, sizeof(logTxt));

    sprintf(logTxt, "%s %s : [%s] ", GetFormatMicroTime(GetCurrentMicroTime()).c_str(),
            classname_in, LogTip[logLevel]);

    /**根据可变参数合成字符串**/
    va_list argp;
    va_start(argp, format);
    vsprintf(logTxt + strlen(logTxt), format, argp);
    va_end(argp); /* 将argp置为NULL */
    sprintf(logTxt + strlen(logTxt), "\n");
    if (LogCallback != NULL)
    {
        LogCallback(logTxt, strlen(logTxt));
    }
    else
    {
        std::cout << logTxt;
    }
}

void SetCallback(Callback callback)
{
    LogCallback = callback;
}

const char *strrnchr(const char *str, const char *str_end, const char ch)
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
    return string(pLeft + 1, pRight - pLeft - 1);
}