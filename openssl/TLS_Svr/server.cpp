#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <poll.h>
#include <string>

#include "TLS_Svr.h"

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("usage %s <port>\n", argv[0]);
        exit(1);
    }
    // int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    // if (serverSocket < 0) {
    //     perror("Unable to create socket");
    //     exit(EXIT_FAILURE);
    // }
    // struct sockaddr_in serverAddress;
    // serverAddress.sin_family = AF_INET;
    // serverAddress.sin_port = htons(atoi(argv[1]));
    // serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);

    // // Allow binding to already used port
    // int optval = 1;
    // setsockopt(serverSocket, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));

    // if (bind(serverSocket, (struct sockaddr*) &serverAddress, sizeof(serverAddress)) < 0) {
    //     perror("Unable to bind socket");
    //     exit(EXIT_FAILURE);
    // }

    // if (listen(serverSocket, 1) < 0) {
    //     perror("Listen on socket failed");
    //     exit(EXIT_FAILURE);
    // }

    // printf("Waiting for incoming connection...\n");
    // struct sockaddr_in clientAddress;
    // unsigned int clientAddressLen = sizeof(clientAddress);

    // TLS_Svr m_TLS_Svr;
    // m_TLS_Svr.init_ssl("./server.crt", "./server.key");
    // m_TLS_Svr.init_ssl_client();

    // char buffer[BUFFER_SIZE] = { 0 };
    // while (1) {

    //     int clientSocket = accept(serverSocket, (struct sockaddr*) &clientAddress, &clientAddressLen);

    //     if (clientSocket < 0) {
    //         perror("Accept on socket failed");
    //         exit(EXIT_FAILURE);
    //     }

    //     printf("Connection accepted!\n");

    //     // client hello
    //     int receivedBytes = read(clientSocket, buffer, BUFFER_SIZE);
    //     if (receivedBytes > 0) {
    //         printf("Host has received %d bytes data\n", receivedBytes);
    //         char* write_buff;
    //         int write_len;
    //         if(m_TLS_Svr.ClientHello(write_buff, write_len, buffer, receivedBytes) > 0)
    //         {
    //             printf("Host has send %d bytes data: %s\n", write_len, write_buff);
    //             int len = write(clientSocket, write_buff, write_len);
    //             printf("Host has write len: %d bytes data\n", len);
    //         }
    //     }

    //     // client Cipher
    //     receivedBytes = read(clientSocket, buffer, BUFFER_SIZE);
    //     if (receivedBytes > 0) {
    //         printf("Cipher Host has received %d bytes data\n", receivedBytes);
    //         char* write_buff;
    //         int write_len;
    //         if(m_TLS_Svr.ClientHello(write_buff, write_len, buffer, receivedBytes) > 0)
    //         {
    //             printf("Cipher Host has send %d bytes data\n", write_len);
    //             write(clientSocket, write_buff, write_len);
    //         }
    //     }

    //     close(clientSocket);
    // }

    char str[INET_ADDRSTRLEN];
    int port = atoi(argv[1]);

    int servfd = socket(AF_INET, SOCK_STREAM, 0);
    if (servfd < 0)
    {
        perror("Unable to create socket");
        exit(EXIT_FAILURE);
    }

    int enable = 1;
    if (setsockopt(servfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(enable)) < 0)
    {
        perror("Unable to setsockopt");
        exit(EXIT_FAILURE);
    }

    /* Specify socket address */
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    if (bind(servfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
    {
        perror("Unable to bind");
        exit(EXIT_FAILURE);
    }

    if (listen(servfd, 128) < 0)
    {
        perror("Unable to listen");
        exit(EXIT_FAILURE);
    }

    int clientfd;
    struct sockaddr_in peeraddr;
    socklen_t peeraddr_len = sizeof(peeraddr);

    struct pollfd fdset[2];
    memset(&fdset, 0, sizeof(fdset));

    fdset[0].fd = STDIN_FILENO;
    fdset[0].events = POLLIN;

    TLS_Svr m_TLS_Svr;
    m_TLS_Svr.init_ssl("server.crt", "server.key"); // see README to create these files

    while (1)
    {
        printf("waiting for next connection on port %d\n", port);

        clientfd = accept(servfd, (struct sockaddr *)&peeraddr, &peeraddr_len);
        if (clientfd < 0)
        {
            perror("Accept on socket failed");
            exit(EXIT_FAILURE);
        }

        m_TLS_Svr.init_ssl_client();

        inet_ntop(peeraddr.sin_family, &peeraddr.sin_addr, str, INET_ADDRSTRLEN);
        printf("new connection from %s:%d\n", str, ntohs(peeraddr.sin_port));

        fdset[1].fd = clientfd;

        /* event loop */

        fdset[1].events = POLLERR | POLLHUP | POLLNVAL | POLLIN;
#ifdef POLLRDHUP
        fdset[1].events |= POLLRDHUP;
#endif

        while (1)
        {
            fdset[1].events &= ~POLLOUT;

            int nready = poll(&fdset[0], 2, -1);

            if (nready == 0)
                continue; /* no fd ready */

            int revents = fdset[1].revents;
            if (revents & POLLIN)
            {
                char buf[BUFFER_SIZE];
                ssize_t n = read(fdset[1].fd, buf, sizeof(buf));
                printf("read len = %d...\n", n);
                if (n > 0)
                {
                    char *write_buff;
                    int write_len;
                    if (m_TLS_Svr.ClientHello(write_buff, write_len, buf, n) > 0)
                    {
                        n = write(fdset[1].fd, write_buff, write_len);
                        printf("ClientHello write len = %d...\n", n);
                    }
                }
                n = read(fdset[1].fd, buf, sizeof(buf));
                printf("read len = %d...\n", n);
                if (n > 0)
                {
                    char *write_buff;
                    int write_len;
                    if (m_TLS_Svr.ChangeCipher(write_buff, write_len, buf, n) > 0)
                    {
                        n = write(fdset[1].fd, write_buff, write_len);
                        printf("ChangeCipher write len = %d...\n", n);
                    }
                }

                n = read(fdset[1].fd, buf, sizeof(buf));
                printf("read len = %d...\n", n);
                if (n > 0)
                {
                    char *write_buff;
                    int write_len;
                    if (m_TLS_Svr.Decrypt(write_buff, write_len, buf, n) > 0)
                    {
                        printf("Decrypt buff - len: %d, : %s\n", n, write_buff);
                        std::string words = "hello openssl!";
                        std::string http_str = "HTTP/1.1  200 OK \r\n"
                                               "Content-Type:  text/html; charset=utf-8 \r\n"
                                               "Connection: keep-alive\r\n"
                                               // "Content-Length: " + std::to_string(words.size()) + "\r\n"
                                               // "Access-Control-Allow-Origin: *"
                                               "\r\n";
                        http_str += words;
                        char buffer[BUFFER_SIZE] = {0};
                        memcpy(buffer, http_str.c_str(), http_str.size());
                        if (m_TLS_Svr.Encrypt(write_buff, write_len, buffer, http_str.size()) > 0)
                        {
                            n = write(fdset[1].fd, write_buff, write_len);
                            printf("Encrypt write len = %d\n", n);
                        }
                    }
                }
            }
            break;
            if (revents & POLLOUT)
                // if (do_sock_write() == -1)
                break;
            if (revents & (POLLERR | POLLHUP | POLLNVAL))
                break;
#ifdef POLLRDHUP
            if (revents & POLLRDHUP)
                break;
#endif
        }
        printf("close : %d\n", fdset[1].fd);
        close(fdset[1].fd);
        m_TLS_Svr.cleanup_ssl_client();
    }

    return 0;
}