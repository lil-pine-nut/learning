#include "OpenSSL_Server.h"
#include <openssl/err.h>
#include <openssl/pem.h>

static SSL_CTX *g_ctx = NULL;

void init_ssl_ctx(const char * certfile, const char* keyfile)
{
    printf("initialising SSL\n");

    /* SSL library initialisation */
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    ERR_load_BIO_strings();
    ERR_load_crypto_strings();

    /* create the SSL server context */
    g_ctx = SSL_CTX_new(SSLv23_method());
    if (!g_ctx)
    {
        perror("SSL_CTX_new()");
        exit(1);
    }

    /* Load certificate and private key files, and check consistency */
    if (certfile && keyfile)
    {
        if (SSL_CTX_use_certificate_file(g_ctx, certfile, SSL_FILETYPE_PEM) != 1)
        {
            fprintf(stderr, "SSL_CTX_use_certificate_file:%s failed\n", certfile);
            exit(1);
        }

        if (SSL_CTX_use_PrivateKey_file(g_ctx, keyfile, SSL_FILETYPE_PEM) != 1)
        {
            fprintf(stderr, "SSL_CTX_use_PrivateKey_file:%s failed\n", keyfile);
            exit(1);
        }

        /* Make sure the key and certificate file match. */
        if (SSL_CTX_check_private_key(g_ctx) != 1)
        {
            fprintf(stderr, "SSL_CTX_check_private_key failed\n");
            exit(1);
        }
        else
            printf("certificate and private key loaded and verified\n");
    }
}

OpenSSL_Server::OpenSSL_Server(/* args */)
{
    memset(this, 0, sizeof(*this));
    m_buff = (char*)malloc(4096);
    if(m_buff == NULL)
    {
        fprintf(stderr, "malloc 4096 failed\n");
        exit(1);
    }
    m_buff_len = 4096;

    rbio = BIO_new(BIO_s_mem());
    wbio = BIO_new(BIO_s_mem());
    ssl = SSL_new(g_ctx);

    SSL_set_accept_state(ssl);  /* ssl server mode */
    // SSL_set_connect_state(ssl); /* ssl client mode */

    SSL_set_bio(ssl, rbio, wbio);

}

OpenSSL_Server::~OpenSSL_Server()
{
    free(m_buff);
    if(ssl != NULL)
    {
        SSL_free(ssl);
    }
}

// hello 和 cipher 阶段在 openssl 都为握手阶段
int OpenSSL_Server::SSLHandshake(char *in_prt, int in_len, char*& out_ptr, size_t & out_Len)
{
    m_len = 0;
    int n;
    while (in_len > 0) {
        n = BIO_write(rbio, in_prt, in_len);
        // printf("ClientHello BIO_write len = %d...\n", n);
        if (n <= 0)
            return -1; /* assume bio write failure is unrecoverable */

        in_prt += n;
        in_len -= n;

        if (!SSL_is_init_finished(ssl)) {
            int n = SSL_do_handshake(ssl);
            do {
                char buf[BUFFER_SIZE] = { 0 };
                n = BIO_read(wbio, buf, sizeof(buf));
                // printf("BIO_read len = %d...\n", n);
                if (n > 0)
                {
                    if(m_len+n > m_buff_len)
                    {
                        // 按实际大小扩展
                        m_buff = (char*)realloc(m_buff, m_len+n);
                        m_buff_len = m_len+n;
                    }
                    memcpy(m_buff+m_len, buf, n);
                    m_len += n;
                }
                else if (!BIO_should_retry(wbio))
                    return -1;
            } while (n>0);
        }
    }
    if(m_len < m_buff_len)
        m_buff[m_len] = '\0';
    out_ptr = m_buff;
    out_Len = m_len;
    return 1;
}

int OpenSSL_Server::Encrypt(char *in_prt, int in_len, char*& out_ptr, size_t & out_Len)
{
    int n;
    while (in_len > 0) {
        n = SSL_write(ssl, in_prt, in_len);
        // printf("SSL_write len = %d...\n", n);
        if (n <= 0)
        {
            int ret = SSL_get_error(ssl, n);
            printf("Encrypt SSL_write error: %d\n", ret);
            return -1; /* assume bio write failure is unrecoverable */
        }
        in_prt += n;
        in_len -= n;
    }
    m_len = 0;
    do {
        char buf[BUFFER_SIZE] = { 0 };
        n = BIO_read(wbio, buf, sizeof(buf));
        // printf("EncryptBytes = %d\n", n);
        if (n > 0)
        {
            if(m_len+n > m_buff_len)
            {
                // 按实际大小扩展
                m_buff = (char*)realloc(m_buff, m_len+n);
                m_buff_len = m_len+n;
            }
            memcpy(m_buff+m_len, buf, n);
            m_len += n;
        }
        else if (!BIO_should_retry(wbio))
        {
            perror("!BIO_should_retry wbio");
            return -1;
        }
    } while (n>0);

    if(m_len < m_buff_len)
        m_buff[m_len] = '\0';
    out_ptr = m_buff;
    out_Len = m_len;
    return 1;
}

int OpenSSL_Server::Decrypt(char *in_prt, int in_len, char*& out_ptr, size_t & out_Len)
{
    int n;
    while (in_len > 0) {
        n = BIO_write(rbio, in_prt, in_len);
        // printf("BIO_write len = %d...\n", n);
        if (n <= 0)
            return -1; /* assume bio write failure is unrecoverable */

        in_prt += n;
        in_len -= n;
    }
    m_len = 0;
    do {
        char buf[BUFFER_SIZE] = { 0 };
        n = SSL_read(ssl, buf, sizeof(buf));
        // printf("Decrypt = %d\n", n);
        if (n > 0)
        {
            if(m_len+n > m_buff_len)
            {
                // 按实际大小扩展
                m_buff = (char*)realloc(m_buff, m_len+n);
                m_buff_len = m_len+n;
            }
            memcpy(m_buff+m_len, buf, n);
            m_len += n;
        }
        else
        {
            int ret = SSL_get_error(ssl, n);
            if(ret == SSL_ERROR_WANT_READ)
            {
                if(m_len < m_buff_len)
                    m_buff[m_len] = '\0';
                out_ptr = m_buff;
                out_Len = m_len;
                return 1;
            }
            printf("Decrypt SSL_read error: %d\n", ret);
            return -1; /* assume bio write failure is unrecoverable */
        }

    } while (n>0);
    if(m_len < m_buff_len)
        m_buff[m_len] = '\0';
    out_ptr = m_buff;
    out_Len = m_len;
    return 1;
}