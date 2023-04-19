#include "KafkaConsumer.h"
#include <iostream>
#include <signal.h>

using namespace std;

static bool run = true;

static void sigterm(int sig)
{
    run = false;
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        cerr << "Usage: " << argv[0] << " <config file>" << endl;
        exit(0);
    }

    signal(SIGINT, sigterm);
    signal(SIGTERM, sigterm);

    KafkaConsumer the_KafkaConsumer;
    if (!the_KafkaConsumer.Init(argv[1]))
    {
        cerr << "KafkaConsumer Init [" << argv[1] << "] failed!" << endl;
        exit(1);
    }

    // 退出要摁 Ctrl+C + 回车
    char *data = NULL;
    int data_Len = 0;
    while (run)
    {
        string topcie_name;
        if (the_KafkaConsumer.Get(data, data_Len, topcie_name))
        {
            cerr << data << endl;
        }
    }

    std::cerr << "Processed a total of " << the_KafkaConsumer.GetMsgCount() << " messages" << endl;
}