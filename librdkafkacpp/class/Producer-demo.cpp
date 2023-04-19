#include "KafkaProducer.h"
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

    KafkaProducer the_KafkaProducer;
    if (!the_KafkaProducer.Init(argv[1]))
    {
        cerr << "KafkaProducer Init [" << argv[1] << "] failed!" << endl;
        exit(1);
    }

    cerr << "start to input ..." << endl;

    // 退出要摁 Ctrl+C + 回车
    for (std::string line; run && std::getline(std::cin, line);)
    {
        if (line.empty())
        {
            continue;
        }
        the_KafkaProducer.Send(line.c_str(), line.size());
    }

    run = true;
    // 退出前处理完输出队列中的消息
    while (run && the_KafkaProducer.outq_len() > 0)
    {
        std::cerr << "Waiting for " << the_KafkaProducer.outq_len() << std::endl;
        the_KafkaProducer.poll(1000);
    }
}