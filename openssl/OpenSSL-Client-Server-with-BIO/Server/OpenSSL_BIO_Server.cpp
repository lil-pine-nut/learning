/*
 * OpenSSL_BIO_Server.cpp
 *
 *  Created on: 29.11.2018
 *  Author: Denis Lugowski
 */

#include "OpenSSL_BIO_Server.h"

#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <cstring>
#include <string>

OpenSSL_BIO_Server::OpenSSL_BIO_Server() {}

OpenSSL_BIO_Server::~OpenSSL_BIO_Server() {}

void OpenSSL_BIO_Server::createSocket(int port)
{
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (serverSocket < 0) {
        perror("Unable to create socket");
        exit(EXIT_FAILURE);
    }

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(port);
    serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);

    // Allow binding to already used port
    int optval = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval));

    if (bind(serverSocket, (struct sockaddr*) &serverAddress, sizeof(serverAddress)) < 0) {
        perror("Unable to bind socket");
        exit(EXIT_FAILURE);
    }

    if (listen(serverSocket, 1) < 0) {
        perror("Listen on socket failed");
        exit(EXIT_FAILURE);
    }
}

void OpenSSL_BIO_Server::waitForIncomingConnection()
{
    printf("Waiting for incoming connection...\n");
    unsigned int clientAddressLen = sizeof(clientAddress);

    clientSocket = accept(serverSocket, (struct sockaddr*) &clientAddress, &clientAddressLen);

    if (clientSocket < 0) {
        perror("Accept on socket failed");
        exit(EXIT_FAILURE);
    }

    printf("Connection accepted!\n");

    // ====== Begin SSL handling ====== //
    // doSSLHandshake();
    // ====== End SSL handling ====== //
}

bool OpenSSL_BIO_Server::doSSLHandshake()
{
    char buffer[BUFFER_SIZE] = { 0 };
    int err_cnt;
    bool SSLHandshake_Success = true;

    while (!SSL_is_init_finished(ssl)) {
        SSL_do_handshake(ssl);
        int bytesToWrite = BIO_read(writeBIO, buffer, BUFFER_SIZE);

        if (bytesToWrite > 0) {
            err_cnt = 0;
            printf("Host has %d bytes encrypted data to send\n", bytesToWrite);
            write(clientSocket, buffer, bytesToWrite);
        }
        else {
            int receivedBytes = read(clientSocket, buffer, BUFFER_SIZE);
            if (receivedBytes > 0) {
                err_cnt = 0;
                printf("Host has received %d bytes data\n", receivedBytes);
                BIO_write(readBIO, buffer, receivedBytes);
            }
            else
            {
                if(err_cnt++ == 1000)
                {
                    SSLHandshake_Success = false;
                    break;
                }
            }
        }
    }

    if(SSLHandshake_Success)
        printf("Host SSL handshake done!\n");
    else
        printf("Host SSL handshake failed !!!!!!!!!!\n");
    return SSLHandshake_Success;
}

char* OpenSSL_BIO_Server::readFromSocket()
{
    char buffer[BUFFER_SIZE];

    int receivedBytes = read(clientSocket, buffer, BUFFER_SIZE);
    if (receivedBytes > 0) {
        printf("Host has received %d bytes encrypted data\n", receivedBytes);
        BIO_write(readBIO, buffer, receivedBytes);
    }

    // SSL_read overrides buffer
    int sizeUnencryptBytes = SSL_read(ssl, buffer, receivedBytes);
    if (sizeUnencryptBytes < 0) {
        perror("SSL_read() failed");
        exit(EXIT_FAILURE);
    }

    char* msg = new char[sizeUnencryptBytes];
    memcpy(msg, buffer, sizeUnencryptBytes);

    return msg;
}

void OpenSSL_BIO_Server::initOpenSSL()
{
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();

    context = createContext();
    configureContext(context);

    ssl = SSL_new(context);
    readBIO = BIO_new(BIO_s_mem());
    writeBIO = BIO_new(BIO_s_mem());

    SSL_set_bio(ssl, readBIO, writeBIO);
    SSL_set_accept_state(ssl); // Server
}

SSL_CTX* OpenSSL_BIO_Server::createContext()
{
    const SSL_METHOD* method;
    SSL_CTX* ctx;

    // Creates a server that will negotiate the highest version of SSL/TLS supported
    // by the client it is connecting to.
    method = TLS_server_method();

    ctx = SSL_CTX_new(method);
    if (!ctx) {
        perror("Unable to create SSL context");
        exit(EXIT_FAILURE);
    }

    const long flags = SSL_EXT_TLS1_3_ONLY;
    SSL_CTX_set_options(ctx, flags);

    return ctx;
}

void handle_error(const char *file, int lineno, const char *msg) {
  fprintf(stderr, "** %s:%i %s\n", file, lineno, msg);
  ERR_print_errors_fp(stderr);
  exit(-1);
}

#define int_error(msg) handle_error(__FILE__, __LINE__, msg)

void OpenSSL_BIO_Server::configureContext(SSL_CTX* ctx)
{
    SSL_CTX_set_ecdh_auto(ctx, 1);

    /* Set the key and cert */
    if (SSL_CTX_use_certificate_file(ctx, CERT_FILE, SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

    if (SSL_CTX_use_PrivateKey_file(ctx, KEY_FILE, SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        exit(EXIT_FAILURE);
    }

//     /* Load certificate and private key files, and check consistency */
//   if (CERT_FILE && KEY_FILE) {
//     if (SSL_CTX_use_certificate_file(ctx, CERT_FILE,  SSL_FILETYPE_PEM) != 1)
//       int_error("SSL_CTX_use_certificate_file failed");

//     if (SSL_CTX_use_PrivateKey_file(ctx, KEY_FILE, SSL_FILETYPE_PEM) != 1)
//       int_error("SSL_CTX_use_PrivateKey_file failed");

//     /* Make sure the key and certificate file match. */
//     if (SSL_CTX_check_private_key(ctx) != 1)
//       int_error("SSL_CTX_check_private_key failed");
//     else
//       printf("certificate and private key loaded and verified\n");
//   }

//   /* Recommended to avoid SSLv2 & SSLv3 */
//   SSL_CTX_set_options(ctx, SSL_OP_ALL|SSL_OP_NO_SSLv2|SSL_OP_NO_SSLv3);
}

void OpenSSL_BIO_Server::closeSocket()
{
    close(clientSocket);
}

void OpenSSL_BIO_Server::cleanupOpenSSL()
{
    SSL_CTX_free(context);
    EVP_cleanup();
}

void OpenSSL_BIO_Server::sendHttpToSocket()
{
    std::string words = "hello openssl!";
    std::string http_str = "HTTP/1.1  200 OK \r\n"
        "Content-Type:  text/html; charset=utf-8 \r\n"
        "Connection: keep-alive\r\n"
        // "Content-Length: " + std::to_string(words.size()) + "\r\n"
        // "Access-Control-Allow-Origin: *"
        "\r\n";
    http_str += words;
    char buffer[BUFFER_SIZE] = { 0 };
    memcpy(buffer, http_str.c_str(), http_str.size());

    // buffer[http_str.size()] = '\0';

    if (http_str.size() > 0) {
        // Note: No need to do BIO_write(readBIO) before, SSL_write takes
        // buffer with unencrypted data directly.
        // See: https://www.openssl.org/docs/man1.1.1/man3/SSL_write.html
        int n = SSL_write(ssl, buffer, http_str.size());
        printf("SSL_write len = %d...\n", n);
        int bytesToWrite = BIO_read(writeBIO, buffer, sizeof(buffer));

        if (bytesToWrite > 0) {
            printf("Host has %d bytes encrypted data to send\n", bytesToWrite);
            write(clientSocket, buffer, bytesToWrite);
        }
    }
}