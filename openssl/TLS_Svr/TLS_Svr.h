#ifndef __TLS__SERVER__H__
#define __TLS__SERVER__H__

#include <openssl/bio.h>
#include <openssl/ssl.h>

#define BUFFER_SIZE 4096

/* An instance of this object is created each time a client connection is
 * accepted. It stores the client file descriptor, the SSL objects, and data
 * which is waiting to be either written to socket or encrypted. */
struct ssl_client
{
    SSL *ssl;

    BIO *rbio; /* SSL reads from, we write to. */
    BIO *wbio; /* SSL writes to, we read from. */
};

class TLS_Svr
{
public:
    TLS_Svr(/* args */);
    ~TLS_Svr();

    // 传进client hello，传出server helo
    int ClientHello(char *&svr_helo, int &svr_len, char *cli_helo, int cli_len);

    //  在TLSv1.2版本，svr_len返回有值；在TLSv1.3版本，svr_len返回值为0；
    int ChangeCipher(char *&svr_cc, int &svr_len, char *cli_cc, int len);

    // 对i_dat解密
    int Decrypt(char *&o_buf, int &o_len, char *i_dat, int i_len);

    // 对i_dat加密
    int Encrypt(char *&o_buf, int &o_len, char *i_dat, int i_len);

    static void init_ssl(const char *certfile, const char *keyfile);

    static void init_ssl(const char *cert_buf, int cert_len, const char *key_buf, int key_len);

    void init_ssl_client();

    void cleanup_ssl_client();

private:
    /* Global SSL context */
    static SSL_CTX *g_ctx;

    ssl_client m_ssl_client;

    char *m_write_buff;
    int m_write_len;
    int m_max_buff_len;
};

#endif