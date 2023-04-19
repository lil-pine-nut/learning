#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <iostream>
#include "zmq.hpp"

using namespace std;
using namespace zmq;

int main(int argc, char **argv)
{
    zmq::context_t context(1);
    zmq::socket_t socket(context, ZMQ_PUB);
    if (argc == 1)
    {
        printf("connect: tcp://localhost:9987...\n");
        socket.connect("tcp://localhost:9987");
    }
    else
    {
        printf("connect: %s...\n", argv[1]);
        socket.connect(argv[1]);
    }
    int i = 0;
    while (1)
    {
        char szBuf[1024] = {0};
        snprintf(szBuf, sizeof(szBuf), "server i=%d", i);
        std::string text = szBuf;
        zmq::message_t message(text.size());
        memcpy(message.data(), text.c_str(), text.size());
        socket.send(message);
        printf("send: %s\n", text.c_str());
        sleep(1);
        ++i;
    }
    return 0;
}