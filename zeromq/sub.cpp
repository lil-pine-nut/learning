#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include "zmq.hpp"
#include <fstream>
#include <iostream>

using namespace zmq;
using namespace std;

int main()
{
    zmq::context_t context(1);
    zmq::socket_t socket(context, ZMQ_SUB);

    socket.bind("tcp://*:9987");
    const char *filter = "";
    socket.setsockopt(ZMQ_SUBSCRIBE, filter, strlen(filter));
    int ret;

    // std::ofstream outfile;
    // outfile.open("test.txt", ios::out | ios::binary);
    // if(!outfile.is_open())
    //     return 0;
    while (1)
    {
        zmq::message_t request;
        socket.recv(&request);
        printf("recv: %s\n", request.data());
        std::string message = std::string(static_cast<char *>(request.data()), request.size());
        printf("recv: %s\n", message.c_str());
        // outfile << message << "\n";
        // outfile.flush();
    }
    // outfile.close();

    return 0;
}