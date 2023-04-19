/*
 * Main.cpp
 *
 *  Created on: 29.11.2018
 *  Author: Denis Lugowski
 */

#include <stdio.h>
#include "OpenSSL_BIO_Server.h"
#include <unistd.h>
#include <string.h>

int main(int argc, char **argv)
{
    OpenSSL_BIO_Server server;

    server.createSocket(8000);
    // server.initOpenSSL();


    while (1) {
        server.initOpenSSL();
        server.waitForIncomingConnection();
        bool ret = server.doSSLHandshake();
        if(ret)
        {
            // for (size_t i = 0; i < 100; i++)
            {
                printf("server.readFromSocket ...\n");
                char* msg = server.readFromSocket();
                printf("Message.len: %d\n", strlen(msg));
                // printf("Message: %s\n", msg);
                delete (msg);
                printf("server.sendHttpToSocket ...\n");
                server.sendHttpToSocket();
                // sleep(10);
            }
        }
        printf("server.restart ...\n\n");
        server.closeSocket();
        server.cleanupOpenSSL();
        sleep(1);
    }

    server.closeSocket();
    server.cleanupOpenSSL();

}

