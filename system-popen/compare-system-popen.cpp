#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <errno.h>
#include <sstream>
#include <fstream>

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
    // 注意：system 串行, popen 并行, 使用 popen 解压文件会导致出错...
    // 测试无需获取命令结果
    string cmd = "ls -l > /dev/null";
    guint64 start;
    sleep(1);
    start = GetCurrentMicroTime();
    for (size_t i = 0; i < 1000; i++)
    {
        int status = system(cmd.c_str());
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
    }
    cerr << "system 1 Cost :" << GetCurrentMicroTime() - start << " us." << endl;
    sleep(1);
    start = GetCurrentMicroTime();
    for (size_t i = 0; i < 1000; i++)
    {
        FILE *ptr = NULL;
        if ((ptr = popen(cmd.c_str(), "r")) == NULL) //测试popen, 相当于fopen的读模式
        {
            fprintf(stderr, "popen: %s. strerror:%s\n", cmd.c_str(), strerror(errno));
            continue;
        }
        pclose(ptr);
    }
    cerr << "popen  1 Cost :" << GetCurrentMicroTime() - start << " us." << endl;

    // 测试获取命令结果
    char buff[4096];
    string tmp_file = "system_tmp_file";
    cmd = "ls -l > " + tmp_file;
    sleep(1);
    start = GetCurrentMicroTime();
    for (size_t i = 0; i < 1000; i++)
    {
        int status = system(cmd.c_str());
        if (WIFEXITED(status))
        {
            ifstream ifile;
            ifile.open(tmp_file.c_str());
            if (!ifile.is_open())
            {
                ostringstream oss;
                oss << "Open " << tmp_file << " failed !" << endl;
                cerr << oss.str().c_str() << endl;
                continue;
            }
            while (ifile.read(buff, sizeof(buff)).good())
            {
            }
            ifile.close();
            remove(tmp_file.c_str());
        }
        else if (WIFSIGNALED(status))
        {
            cerr << "abnormal termination, signal number = " << WTERMSIG(status) << endl; //如果cmdstring被信号中断，取得信号值
        }
        else if (WIFSTOPPED(status))
        {
            cerr << "abnormal stopped, signal number = " << WSTOPSIG(status) << endl; //如果cmdstring被信号暂停执行，取得信号值
        }
    }
    cerr << "system 2 Cost :" << GetCurrentMicroTime() - start << " us." << endl;
    // cerr << "buff:" << buff << endl;
    cmd = "ls -l";
    stringstream ss;
    memset(buff, 0, sizeof(buff));
    sleep(1);
    start = GetCurrentMicroTime();
    for (size_t i = 0; i < 1000; i++)
    {
        FILE *ptr = NULL;
        if ((ptr = popen(cmd.c_str(), "r")) == NULL) //测试popen, 相当于fopen的读模式
        {
            fprintf(stderr, "popen: %s. strerror:%s\n", cmd.c_str(), strerror(errno));
            continue;
        }
        ss.str("");
        memset(buff, 0, sizeof(buff));
        while (fread(buff, sizeof(buff), 1, ptr) > 0)
        {
            ss << buff;
        }
        if (strlen(buff) > 0)
            ss << buff;
        pclose(ptr);
    }
    cerr << "popen  2 Cost :" << GetCurrentMicroTime() - start << " us." << endl;
    // cerr << "buff:" << buff << endl;
    // cerr << "ss.str:" << ss.str() << endl;
    // string str1;
    // while (getline(ss, str1))
    // {
    //     cerr << str1 << endl;
    // }

    return 0;
}