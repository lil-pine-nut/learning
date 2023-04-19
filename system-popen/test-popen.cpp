#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <errno.h>

using namespace std;

int main()
{
    string check_command = "ls -l";
    char buff[65536];
    int count = 0;
    FILE *ptr = NULL;
    if ((ptr = popen(check_command.c_str(), "r")) == NULL) //测试popen, 相当于fopen的读模式
    {
        fprintf(stderr, "popen: %s. strerror:%s\n", check_command.c_str(), strerror(errno));
        return false;
    }

    // memset(buff, 0, sizeof(buff));
    while ((fgets(buff, sizeof(buff), ptr)) != NULL)
    {
        ++count;
        printf("%s", buff);
    }

    pclose(ptr);
    if (count <= 0)
    {
        fprintf(stderr, "execute cmd:[%s] is woring!!!\n", check_command.c_str(), strerror(errno));
        return false;
    }
    return true;
}