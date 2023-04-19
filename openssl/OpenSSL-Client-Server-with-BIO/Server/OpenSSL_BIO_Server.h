/*
 * OpenSSL_BIO_Server.h
 *
 *  Created on: 29.11.2018
 *  Author: Denis Lugowski
 */

#ifndef OpenSSL_BIO_Server_H_
#define OpenSSL_BIO_Server_H_

#include <netinet/in.h>

struct ssl_ctx_st;
struct ssl_st;
struct bio_st;
typedef struct ssl_ctx_st SSL_CTX;
typedef struct ssl_st SSL;
typedef struct bio_st BIO;
#define BUFFER_SIZE 4096

class OpenSSL_BIO_Server
{
public:
    OpenSSL_BIO_Server();
    virtual ~OpenSSL_BIO_Server();

    // Socket functions
    void createSocket(int port);
    void waitForIncomingConnection();
    char* readFromSocket();
    void closeSocket();

    // OpenSSL_BIO_Server functions
    void initOpenSSL();
    void cleanupOpenSSL();
    SSL_CTX* createContext();
    void configureContext(SSL_CTX* ctx);
    bool doSSLHandshake();

    void sendHttpToSocket();

private:
    int serverSocket;
    int clientSocket;
    struct sockaddr_in serverAddress;
    struct sockaddr_in clientAddress;

    SSL* ssl;
    SSL_CTX* context;
    BIO* readBIO;
    BIO* writeBIO;

    const char* CERT_FILE = "./cert.pem";
    const char* KEY_FILE = "./key.pem";
};

#endif /* OpenSSL_BIO_Server_H_ */
