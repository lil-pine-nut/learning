#include "TLS_Svr.h"
#include <openssl/err.h>
#include <openssl/pem.h>

SSL_CTX *TLS_Svr::g_ctx = NULL;

TLS_Svr::TLS_Svr(/* args */)
{
    m_write_buff = (char *)malloc(65536);
    if (m_write_buff == NULL)
    {
        fprintf(stderr, "malloc 65536 failed\n");
        exit(1);
    }
    m_max_buff_len = 65536;
    memset(&m_ssl_client, 0, sizeof(struct ssl_client));
}

TLS_Svr::~TLS_Svr()
{
    free(m_write_buff);
    if (m_ssl_client.ssl != NULL)
    {
        SSL_free(m_ssl_client.ssl);
    }
}

void TLS_Svr::init_ssl(const char *certfile, const char *keyfile)
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
            fprintf(stderr, "SSL_CTX_use_certificate_file failed\n");
            exit(1);
        }

        if (SSL_CTX_use_PrivateKey_file(g_ctx, keyfile, SSL_FILETYPE_PEM) != 1)
        {
            fprintf(stderr, "SSL_CTX_use_PrivateKey_file failed\n");
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

void TLS_Svr::init_ssl(const char *cert_buf, int cert_len, const char *key_buf, int key_len)
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
    if (cert_buf && key_buf)
    {
        if (SSL_CTX_use_certificate_ASN1(g_ctx, cert_len, (const unsigned char *)cert_buf) != 1)
        {
            fprintf(stderr, "SSL_CTX_use_certificate_ASN1 failed\n");
            exit(1);
        }

        if (SSL_CTX_use_PrivateKey_ASN1(SSL_FILETYPE_PEM, g_ctx, (const unsigned char *)key_buf, key_len) != 1)
        {
            fprintf(stderr, "SSL_CTX_use_PrivateKey_ASN1 failed\n");
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

void TLS_Svr::init_ssl_client()
{
    memset(&m_ssl_client, 0, sizeof(struct ssl_client));

    m_ssl_client.rbio = BIO_new(BIO_s_mem());
    m_ssl_client.wbio = BIO_new(BIO_s_mem());
    m_ssl_client.ssl = SSL_new(g_ctx);

    SSL_set_accept_state(m_ssl_client.ssl); /* ssl server mode */
    // SSL_set_connect_state(m_ssl_client.ssl); /* ssl client mode */

    SSL_set_bio(m_ssl_client.ssl, m_ssl_client.rbio, m_ssl_client.wbio);
}

void TLS_Svr::cleanup_ssl_client()
{
    SSL_free(m_ssl_client.ssl); /* free the SSL object and its BIO's */
    m_ssl_client.ssl = NULL;
}

// 传进client hello，传出server helo
int TLS_Svr::ClientHello(char *&svr_helo, int &svr_len, char *cli_helo, int cli_len)
{
    m_write_len = 0;
    int n;
    while (cli_len > 0)
    {
        n = BIO_write(m_ssl_client.rbio, cli_helo, cli_len);
        // printf("ClientHello BIO_write len = %d...\n", n);
        if (n <= 0)
            return -1; /* assume bio write failure is unrecoverable */

        cli_helo += n;
        cli_len -= n;

        if (!SSL_is_init_finished(m_ssl_client.ssl))
        {
            int n = SSL_do_handshake(m_ssl_client.ssl);
            do
            {
                char buf[BUFFER_SIZE] = {0};
                n = BIO_read(m_ssl_client.wbio, buf, sizeof(buf));
                // printf("BIO_read len = %d...\n", n);
                if (n > 0)
                {
                    if (m_write_len + n > m_max_buff_len)
                    {
                        // buff大小*2扩展
                        m_write_buff = (char *)realloc(m_write_buff, m_max_buff_len * 2);
                        m_max_buff_len *= 2;
                        // 按实际大小扩展
                        // m_write_buff = (char*)realloc(m_write_buff, m_write_len+n);
                        // m_max_buff_len = m_write_len+n;
                    }
                    memcpy(m_write_buff + m_write_len, buf, n);
                    m_write_len += n;
                }
                else if (!BIO_should_retry(m_ssl_client.wbio))
                    return -1;
            } while (n > 0);
        }
    }
    if (m_write_len < m_max_buff_len)
        m_write_buff[m_write_len] = '\0';
    svr_helo = m_write_buff;
    svr_len = m_write_len;
    return 1;
}

int TLS_Svr::ChangeCipher(char *&svr_cc, int &svr_len, char *cli_cc, int len)
{
    m_write_len = 0;
    int n;
    while (len > 0)
    {
        n = BIO_write(m_ssl_client.rbio, cli_cc, len);
        // printf("ChangeCipher BIO_write len = %d...\n", n);
        if (n <= 0)
            return -1; /* assume bio write failure is unrecoverable */

        cli_cc += n;
        len -= n;

        if (!SSL_is_init_finished(m_ssl_client.ssl))
        {
            int n = SSL_do_handshake(m_ssl_client.ssl);
            do
            {
                char buf[BUFFER_SIZE] = {0};
                n = BIO_read(m_ssl_client.wbio, buf, sizeof(buf));
                // printf("BIO_read len = %d...\n", n);
                if (n > 0)
                {
                    if (m_write_len + n > m_max_buff_len)
                    {
                        // buff大小*2扩展
                        m_write_buff = (char *)realloc(m_write_buff, m_max_buff_len * 2);
                        m_max_buff_len *= 2;
                        // 按实际大小扩展
                        // m_write_buff = (char*)realloc(m_write_buff, m_write_len+n);
                        // m_max_buff_len = m_write_len+n;
                    }
                    memcpy(m_write_buff + m_write_len, buf, n);
                    m_write_len += n;
                }
                else if (!BIO_should_retry(m_ssl_client.wbio))
                    return -1;
            } while (n > 0);
        }
    }
    if (m_write_len < m_max_buff_len)
        m_write_buff[m_write_len] = '\0';
    svr_cc = m_write_buff;
    svr_len = m_write_len;
    return 1;
}

int TLS_Svr::Decrypt(char *&o_buf, int &o_len, char *i_dat, int i_len)
{
    int n;
    int len = i_len;
    while (len > 0)
    {
        n = BIO_write(m_ssl_client.rbio, i_dat, len);
        // printf("BIO_write len = %d...\n", n);
        if (n <= 0)
            return -1; /* assume bio write failure is unrecoverable */

        i_dat += n;
        len -= n;
    }
    m_write_len = 0;
    do
    {
        char buf[BUFFER_SIZE] = {0};
        n = SSL_read(m_ssl_client.ssl, buf, sizeof(buf));
        // printf("Decrypt = %d\n", n);
        if (n > 0)
        {
            if (m_write_len + n > m_max_buff_len)
            {
                // buff大小*2扩展
                m_write_buff = (char *)realloc(m_write_buff, m_max_buff_len * 2);
                m_max_buff_len *= 2;
                // 按实际大小扩展
                // m_write_buff = (char*)realloc(m_write_buff, m_write_len+n);
                // m_max_buff_len = m_write_len+n;
            }
            memcpy(m_write_buff + m_write_len, buf, n);
            m_write_len += n;
        }
        else
        {
            int ret = SSL_get_error(m_ssl_client.ssl, n);
            if (ret == SSL_ERROR_WANT_READ)
            {
                if (m_write_len < m_max_buff_len)
                    m_write_buff[m_write_len] = '\0';
                o_buf = m_write_buff;
                o_len = m_write_len;
                return 1;
            }
            printf("Decrypt SSL_read error: %d\n", ret);
            return -1; /* assume bio write failure is unrecoverable */
        }

    } while (n > 0);
    if (m_write_len < m_max_buff_len)
        m_write_buff[m_write_len] = '\0';
    o_buf = m_write_buff;
    o_len = m_write_len;
    return 1;
}

int TLS_Svr::Encrypt(char *&o_buf, int &o_len, char *i_dat, int i_len)
{
    int n;
    int len = i_len;
    while (len > 0)
    {
        n = SSL_write(m_ssl_client.ssl, i_dat, len);
        // printf("SSL_write len = %d...\n", n);
        if (n <= 0)
        {
            int ret = SSL_get_error(m_ssl_client.ssl, n);
            printf("Encrypt SSL_write error: %d\n", ret);
            return -1; /* assume bio write failure is unrecoverable */
        }
        i_dat += n;
        len -= n;
    }
    m_write_len = 0;
    do
    {
        char buf[BUFFER_SIZE] = {0};
        n = BIO_read(m_ssl_client.wbio, buf, sizeof(buf));
        // printf("EncryptBytes = %d\n", n);
        if (n > 0)
        {
            if (m_write_len + n > m_max_buff_len)
            {
                // buff大小*2扩展
                m_write_buff = (char *)realloc(m_write_buff, m_max_buff_len * 2);
                m_max_buff_len *= 2;
                // 按实际大小扩展
                // m_write_buff = (char*)realloc(m_write_buff, m_write_len+n);
                // m_max_buff_len = m_write_len+n;
            }
            memcpy(m_write_buff + m_write_len, buf, n);
            m_write_len += n;
        }
        else if (!BIO_should_retry(m_ssl_client.wbio))
        {
            perror("!BIO_should_retry wbio");
            return -1;
        }
    } while (n > 0);
    if (m_write_len < m_max_buff_len)
        m_write_buff[m_write_len] = '\0';
    o_buf = m_write_buff;
    o_len = m_write_len;
    return 1;
}