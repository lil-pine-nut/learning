#include "OneNET.h"

#include <openssl/hmac.h>
#include <openssl/bio.h>

#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <string>
#include <iostream>
#include <sstream>
#include <assert.h>

using namespace std;

size_t Base64Encode(const char *input, int length, char *&output, bool with_new_line)
{
    BIO *bmem = NULL;
    BIO *b64 = NULL;
    BUF_MEM *bptr = NULL;

    b64 = BIO_new(BIO_f_base64());
    if (!with_new_line)
    {
        BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    }
    bmem = BIO_new(BIO_s_mem());
    b64 = BIO_push(b64, bmem);
    BIO_write(b64, input, length);
    BIO_flush(b64);
    BIO_get_mem_ptr(b64, &bptr);
    size_t size = bptr->length;
    output = (char *)malloc(bptr->length + 1);
    memset(output, 0, bptr->length);
    memcpy(output, bptr->data, bptr->length);
    output[bptr->length + 1] = 0;

    BIO_free_all(b64);

    return size;
}

size_t Base64Decode(const char *input, int length, char *&output, bool with_new_line)
{
    BIO *b64 = NULL;
    BIO *bmem = NULL;
    output = (char *)malloc(length);
    memset(output, 0, length);

    b64 = BIO_new(BIO_f_base64());
    if (!with_new_line)
    {
        BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    }
    bmem = BIO_new_mem_buf(input, length);
    bmem = BIO_push(b64, bmem);
    size_t size = BIO_read(bmem, output, length);
    //   fprintf(stderr, "aaa:%d\r\n", len);
    BIO_free_all(bmem);

    return size;
}

// C++实现对URL的编码和解码（支持ansi和utf8格式） https://blog.csdn.net/qq_37781464/article/details/113977888
unsigned char ToHex(unsigned char x)
{
    return x > 9 ? x + 55 : x + 48;
}

unsigned char FromHex(unsigned char x)
{
    unsigned char y;
    if (x >= 'A' && x <= 'Z')
        y = x - 'A' + 10;
    else if (x >= 'a' && x <= 'z')
        y = x - 'a' + 10;
    else if (x >= '0' && x <= '9')
        y = x - '0';
    return y;
}

std::string UrlEncode(const std::string &str)
{
    std::string strTemp = "";
    size_t length = str.length();
    for (size_t i = 0; i < length; i++)
    {
        if (isalnum((unsigned char)str[i]) ||
            (str[i] == '-') ||
            (str[i] == '_') ||
            (str[i] == '.') ||
            (str[i] == '~'))
            strTemp += str[i];
        else if (str[i] == ' ')
            strTemp += "+";
        else
        {
            strTemp += '%';
            strTemp += ToHex((unsigned char)str[i] >> 4);
            strTemp += ToHex((unsigned char)str[i] % 16);
        }
    }
    return strTemp;
}

std::string UrlDecode(const std::string &str)
{
    std::string strTemp = "";
    size_t length = str.length();
    for (size_t i = 0; i < length; i++)
    {
        if (str[i] == '+')
            strTemp += ' ';
        else if (str[i] == '%')
        {
            assert(i + 2 < length);
            unsigned char high = FromHex((unsigned char)str[++i]);
            unsigned char low = FromHex((unsigned char)str[++i]);
            strTemp += high * 16 + low;
        }
        else
            strTemp += str[i];
    }
    return strTemp;
}

string OneNET::Token()
{
    stringstream StringForSignature;
    StringForSignature << et << "\n"
                       << method << "\n"
                       << res << "\n"
                       << version;
    cerr << StringForSignature << endl;
    char *base64decode;
    size_t Base64Decode_len = Base64Decode(key.c_str(), key.size(), base64decode, false);
    cerr << "Base64Decode len:" << Base64Decode_len << endl;
    cerr << "key.size():" << key.size() << endl;

    // 各种算法得到的摘要的长度
    // 算法	摘要长度（字节）
    // MD2	16
    // MD5	16
    // SHA	20
    // SHA1	20
    // SHA224	28
    // SHA256	32
    // SHA384	48
    // SHA512	64
    char HMAC_buff[128];
    int HMAC_len = 128;
    HMAC(EVP_sha1(), (unsigned char *)base64decode, Base64Decode_len,
         (unsigned char *)StringForSignature.str().c_str(), StringForSignature.str().size(),
         (unsigned char *)HMAC_buff, (unsigned int *)&HMAC_len);
    cerr << "HMAC_len:" << HMAC_len << endl;
    char *base64encode;
    size_t Base64Encode_len = Base64Encode(HMAC_buff, HMAC_len, base64encode, false);
    cerr << "Base64Encode len:" << Base64Encode_len << endl;

    ostringstream Token;
    Token << "version=" << UrlEncode(version) << "&res=" << UrlEncode(res) << "&et="
          << UrlEncode(et) << "&method=" << UrlEncode(method) << "&sign=" << UrlEncode(string(base64encode, Base64Encode_len));
    cerr << Token.str() << endl;
    cerr << UrlEncode(string(base64encode, Base64Encode_len)) << endl;

    free(base64decode);
    free(base64encode);
    return Token.str();
}