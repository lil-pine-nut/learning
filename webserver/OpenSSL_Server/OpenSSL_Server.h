#ifndef __OPENSSL__SERVER__H__
#define __OPENSSL__SERVER__H__

#include <openssl/bio.h>
#include <openssl/ssl.h>

#define BUFFER_SIZE 4096

void init_ssl_ctx(const char * certfile, const char* keyfile);

struct OpenSSL_Server
{
    OpenSSL_Server();
    ~OpenSSL_Server();
    SSL *ssl;

    BIO *rbio; /* SSL reads from, we write to. */
    BIO *wbio; /* SSL writes to, we read from. */

    char*   m_buff;
    int     m_len;
    int     m_buff_len;

    // hello 和 cipher 阶段在 openssl 都为握手阶段
    int SSLHandshake(char *in_prt, int in_len, char*& out_ptr, size_t & out_Len);

    int Encrypt(char *in_prt, int in_len, char*& out_ptr, size_t & out_Len);

    int Decrypt(char *in_prt, int in_len, char*& out_ptr, size_t & out_Len);
};

#endif