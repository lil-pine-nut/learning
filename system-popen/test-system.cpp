
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <errno.h>

typedef unsigned long long guint64;
typedef unsigned int guint32;
typedef unsigned short guint16;
typedef unsigned char guint8;

#include <sys/time.h>
#include <unistd.h>
guint64 GetCurrentMicroTime()
{
    struct timeval dwStart;
    gettimeofday(&dwStart, NULL);
    guint64 dwTime = 1000000 * dwStart.tv_sec + dwStart.tv_usec;
    return dwTime;
}

using namespace std;

int main()
{
    const char *cmdstring = "abcd 2>>/dev/null ";
    // const char* cmdstring = "unzip -o -d /memfs/test/ /memfs/test/TD-LTE_IMM-MM-MDT_ERICSSON_OMC1-188.1.131.96_20220223120000_1.zip  \
    // TD-LTE_IMM-MM-MDT_908904_ERICSSON_OMC1_20220223120000.zip \
    // TD-LTE_IMM-MM-MDT_908946_ERICSSON_OMC1_20220223120000.zip \
    // TD-LTE_IMM-MM-MDT_908958_ERICSSON_OMC1_20220223120000.zip  > /dev/null";
    int status;
    if (NULL == cmdstring) //如果cmdstring为空趁早闪退吧，尽管system()函数也能处理空指针
    {
        return 0;
    }
    guint64 start = GetCurrentMicroTime();
    status = system(cmdstring);
    if (WIFEXITED(status))
    {
    }
    else if (WIFSIGNALED(status))
    {
        cerr << "abnormal termination, signal number = " << WTERMSIG(status) << endl; //如果cmdstring被信号中断，取得信号值
    }
    else if (WIFSTOPPED(status))
    {
        cerr << "abnormal stopped, signal number = " << WSTOPSIG(status) << endl; //如果cmdstring被信号暂停执行，取得信号值
    }
    cerr << "system Cost :" << GetCurrentMicroTime() - start << endl;

    // const char* cmdstring1 = "unzip -o -d /memfs/test/ /memfs/test/TD-LTE_IMM-MM-MDT_ERICSSON_OMC1-188.1.131.96_20220223120000_1.zip  \
    // TD-LTE_IMM-MM-MDT_908904_ERICSSON_OMC1_20220223120000.zip > /dev/null";
    // const char* cmdstring2 = "unzip -o -d /memfs/test/ /memfs/test/TD-LTE_IMM-MM-MDT_ERICSSON_OMC1-188.1.131.96_20220223120000_1.zip  \
    // TD-LTE_IMM-MM-MDT_908946_ERICSSON_OMC1_20220223120000.zip > /dev/null";
    // const char* cmdstring3 = "unzip -o -d /memfs/test/ /memfs/test/TD-LTE_IMM-MM-MDT_ERICSSON_OMC1-188.1.131.96_20220223120000_1.zip  \
    // TD-LTE_IMM-MM-MDT_908958_ERICSSON_OMC1_20220223120000.zip > /dev/null";

    // start = GetCurrentMicroTime();
    // status = system(cmdstring1);
    // status = system(cmdstring2);
    // status = system(cmdstring3);
    // cerr << "system Cost :" << GetCurrentMicroTime() - start << endl;
}