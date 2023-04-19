#include <iostream>
#include "Poller.h"
#include <unistd.h>

int main()
{
    Poller::Ptr poller = Poller::Instance();

    poller->doDelayTask(2.0f, []()
                        {cerr << "repeat 3" << endl; return false; });
    usleep(100);
    poller->doDelayTask(0.5f, []()
                        {cerr << "repeat 1" << endl; return true; });
    poller->doDelayTask(1.0f, []()
                        {cerr << "repeat 2" << endl; return true; });

    getchar();
}