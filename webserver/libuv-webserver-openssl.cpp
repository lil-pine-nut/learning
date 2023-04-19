/**
 * @file https://github.com/springmeyer/libuv-webserver
 */
#include <signal.h>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <unistd.h> // _SC_NPROCESSORS_ONLN on OS X
#include "uv.h"
#include "http_parser.h"

// stl
#include <string>
#include <sstream>
#include <string>
#include <iostream>
#include <string.h>
using namespace std;

#include "modify-mdtransform.h"
#include "OpenSSL_Server.h"

enum HttpsStage
{
    HTTPS_HELLO,
    HTTPS_CIPHER,
    HTTPS_DATA,
};
string g_server_port;

// #define DEBUG

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
    else
        assert(0);
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

#ifdef DEBUG
#define CHECK(status, msg)                                     \
    if (status != 0)                                           \
    {                                                          \
        fprintf(stderr, "%s: %s\n", msg, uv_err_name(status)); \
        fprintf(stderr, "%s: %s\n", msg, uv_strerror(status)); \
        exit(1);                                               \
    }
#define UVERR(err, msg) fprintf(stderr, "%s: %s\n", msg, uv_err_name(err))
#define LOG_ERROR(msg) puts(msg);
#define LOG(msg) puts(msg);
#define LOGF(...) printf(__VA_ARGS__);
#else
#define CHECK(status, msg)
#define UVERR(err, msg)
#define LOG_ERROR(msg)
#define LOG(msg)
#define LOGF(...)
#endif
static int request_num = 1;
static uv_loop_t *uv_loop;
static uv_tcp_t server;
static http_parser_settings parser_settings;
void after_write(uv_write_t *req, int status);
void after_write2(uv_write_t *req, int status);

struct client_t
{
    uv_tcp_t handle;
    http_parser parser;
    uv_write_t write_req;
    int request_num;
    std::string path;
    int https_stage;
    OpenSSL_Server openssl_server;
};

void on_close(uv_handle_t *handle)
{
    client_t *client = (client_t *)handle->data;
    LOGF("[ %5d ] connection closed\n\n", client->request_num);
    delete client;
}

void alloc_cb(uv_handle_t * /*handle*/, size_t suggested_size, uv_buf_t *buf)
{
    *buf = uv_buf_init((char *)malloc(suggested_size), suggested_size);
}

void on_read(uv_stream_t *tcp, ssize_t nread, const uv_buf_t *buf)
{
    ssize_t parsed;
    LOGF("on read: %ld\n", nread);
    client_t *client = (client_t *)tcp->data;
    if (nread >= 0)
    {
        uv_buf_t uv_write_buf;
        switch (client->https_stage)
        {
        case HTTPS_HELLO:

            if (client->openssl_server.SSLHandshake(buf->base, nread, uv_write_buf.base, uv_write_buf.len) > 0)
            {
                LOGF("[ %5d ] SSLHandshake hello len: %d \n", client->request_num, uv_write_buf.len);
                int r = uv_write(&client->write_req,
                                 (uv_stream_t *)&client->handle,
                                 &uv_write_buf,
                                 1,
                                 after_write2);
                CHECK(r, "write buff");
            }
            else
            {
                LOG_ERROR("Openssl SSLHandshake hello ERROR");
                uv_close((uv_handle_t *)&client->handle, on_close);
            }
            client->https_stage = HTTPS_CIPHER;
            break;
        case HTTPS_CIPHER:
            if (client->openssl_server.SSLHandshake(buf->base, nread, uv_write_buf.base, uv_write_buf.len) > 0)
            {
                LOGF("[ %5d ] SSLHandshake cipher len: %d \n", client->request_num, uv_write_buf.len);
                int r = uv_write(&client->write_req,
                                 (uv_stream_t *)&client->handle,
                                 &uv_write_buf,
                                 1,
                                 after_write2);
                CHECK(r, "write buff");
            }
            else
            {
                LOG_ERROR("SSLHandshake cipher ERROR");
                uv_close((uv_handle_t *)&client->handle, on_close);
            }
            client->https_stage = HTTPS_DATA;
            break;
        case HTTPS_DATA:

            if (client->openssl_server.Decrypt(buf->base, nread, uv_write_buf.base, uv_write_buf.len) > 0)
            {
                LOGF("[ %5d ] Decrypt len: %d \n", client->request_num, uv_write_buf.len);
                parsed = (ssize_t)http_parser_execute(
                    &client->parser, &parser_settings, uv_write_buf.base, uv_write_buf.len);
                if (client->parser.upgrade)
                {
                    LOG_ERROR("parse error: cannot handle http upgrade");
                    uv_close((uv_handle_t *)&client->handle, on_close);
                }
                else if (parsed < uv_write_buf.len)
                {
                    LOG_ERROR("parse error");
                    uv_close((uv_handle_t *)&client->handle, on_close);
                }
            }
            else
            {
                LOG_ERROR("SSLHandshake ERROR");
                uv_close((uv_handle_t *)&client->handle, on_close);
            }
            break;
        default:
            break;
        }
    }
    else
    {
        if (nread != UV_EOF)
        {
            UVERR(nread, "read");
        }
        uv_close((uv_handle_t *)&client->handle, on_close);
    }
    free(buf->base);
}

struct render_baton
{
    render_baton(client_t *_client) : client(_client),
                                      request(),
                                      result(),
                                      response_code("200 OK"),
                                      content_type("text/plain; charset=utf-8"),
                                      error(false),
                                      buff(NULL)

    {
        request.data = this;
    }
    ~render_baton()
    {
        if (buff)
            delete[] buff;
    }
    client_t *client;
    uv_work_t request;
    std::string result;
    std::string split_result;
    std::string response_code;
    std::string content_type;
    bool error;
    char *buff;
};

void after_write(uv_write_t *req, int status)
{
    CHECK(status, "after_write");
    if (!uv_is_closing((uv_handle_t *)req->handle))
    {
        render_baton *closure = static_cast<render_baton *>(req->data);
        delete closure;
        uv_close((uv_handle_t *)req->handle, on_close);
    }
}

void after_write2(uv_write_t *req, int status)
{
    CHECK(status, "after_write");
}

bool endswith(std::string const &value, std::string const &search)
{
    if (value.length() >= search.length())
    {
        return (0 == value.compare(value.length() - search.length(), search.length(), search));
    }
    else
    {
        return false;
    }
}

bool endswith2(std::string const &value, const char *search)
{
    int len = value.size() - strlen(search);
    if (len >= 0)
    {
        const char *pos1 = value.c_str() + len;
        const char *pos2 = search;
        while (*pos1 != '\0')
        {
            if (*pos1++ != *pos2++)
                return false;
        }
        return true;
    }
    return false;
}

void render(uv_work_t *req)
{
    render_baton *closure = static_cast<render_baton *>(req->data);
    client_t *client = (client_t *)closure->client;
    LOGF("[ %5d ] render\n", client->request_num);
    std::string filepath(".");
    filepath = "." + UrlDecode(client->path);
    std::string index_path = (filepath + "index.html");
    bool has_index = (access(index_path.c_str(), R_OK) != -1);
    if (/*!has_index &&*/ filepath[filepath.size() - 1] == '/')
    {
        uv_fs_t scandir_req;
        int r = uv_fs_scandir(uv_loop, &scandir_req, filepath.c_str(), 0, NULL);
        uv_dirent_t dent;
        closure->content_type = "text/html; charset=utf-8";
        closure->result = "<html><body><ul>";
        while (UV_EOF != uv_fs_scandir_next(&scandir_req, &dent))
        {
            std::string name = dent.name;
            if (dent.type == UV_DIRENT_DIR)
            {
                name += "/";
            }
            closure->result += "<li><a href='";
            closure->result += name;
            closure->result += "'>";
            closure->result += name;
            closure->result += "</a></li>";
            closure->result += "\n";
        }
        closure->result += "</ul></body></html>";
        uv_fs_req_cleanup(&scandir_req);
    }
    else
    {
        std::string file_to_open = filepath;
        if (has_index)
        {
            file_to_open = index_path;
        }
        bool exists = (access(file_to_open.c_str(), R_OK) != -1);
        if (!exists)
        {
            closure->result = "no access";
            closure->response_code = "404 Not Found";
            return;
        }
        FILE *f = fopen(file_to_open.c_str(), "rb");
        if (f)
        {
            std::fseek(f, 0, SEEK_END);
            unsigned size = std::ftell(f);
            std::fseek(f, 0, SEEK_SET);
            closure->result.resize(size);
            std::fread(&closure->result[0], size, 1, f);
            fclose(f);
            if (endswith2(file_to_open, ".html") || endswith2(file_to_open, ".htm"))
            {
                closure->content_type = "text/html; charset=utf-8";
            }
            else if (endswith2(file_to_open, ".css"))
            {
                closure->content_type = "text/css";
            }
            else if (endswith2(file_to_open, ".js"))
            {
                closure->content_type = "application/javascript";
            }
            else if (endswith2(file_to_open, ".jpg") || endswith2(file_to_open, ".jpeg"))
            {
                closure->content_type = "image/jpeg";
            }
            else if (endswith2(file_to_open, ".gif"))
            {
                closure->content_type = "image/gif";
            }
            else if (endswith2(file_to_open, ".png"))
            {
                closure->content_type = "image/png";
            }
            else if (endswith2(file_to_open, ".au"))
            {
                closure->content_type = "audio/basic";
            }
            else if (endswith2(file_to_open, ".wav"))
            {
                closure->content_type = "audio/wav";
            }
            else if (endswith2(file_to_open, ".midi") || endswith2(file_to_open, ".mid"))
            {
                closure->content_type = "audio/midi";
            }
            else if (endswith2(file_to_open, ".mp3"))
            {
                closure->content_type = "audio/mpeg";
            }
            else if (endswith2(file_to_open, ".avi"))
            {
                closure->content_type = "video/x-msvideo";
            }
            else if (endswith2(file_to_open, ".mov") || endswith2(file_to_open, ".qt"))
            {
                closure->content_type = "video/quicktime";
            }
            else if (endswith2(file_to_open, ".mpeg") || endswith2(file_to_open, ".mpe"))
            {
                closure->content_type = "video/mpeg";
            }
            else if (endswith2(file_to_open, ".vrml") || endswith2(file_to_open, ".wrl"))
            {
                closure->content_type = "model/vrml";
            }
            else if (endswith2(file_to_open, ".ogg"))
            {
                closure->content_type = "application/ogg";
            }
            else if (endswith2(file_to_open, ".pac"))
            {
                closure->content_type = "application/x-ns-proxy-autoconfig";
            }
            else if (endswith2(file_to_open, ".md"))
            {
                closure->content_type = "text/html; charset=utf-8";

                // 因为要跟代码兼容，把装载文件改为了装载字符串
                // 装载 Markdown 文件的内容
                MarkdownTransform transformer(closure->result);

                //使用getTableOfContens()获取MD文件HTML格式目录
                std::string table = transformer.getTableOfContents();

                //使用getContens()获取转换后的HTML内容
                std::string contents = transformer.getContents();

                //写入HTML文件的头尾信息 , 把IP修改为自己服务器的IP
                std::string head =
                    "<!DOCTYPE html><html><head>\
                    <meta charset=\"utf-8\">\
                    <title>Markdown</title>\
                    <link rel=\"stylesheet\" href=\"https://127.0.0.1:" +
                    g_server_port + "/resources/github-markdown.css\">\
                    </head><body><article class=\"markdown-body\">";
                std::string end = "</article></body></html>";

                closure->result = head + table + contents + end;
            }
        }
        else
        {
            closure->result = "failed to open";
            closure->response_code = "404 Not Found";
        }
    }
}

void after_render(uv_work_t *req)
{
    render_baton *closure = static_cast<render_baton *>(req->data);
    client_t *client = (client_t *)closure->client;

    LOGF("[ %5d ] after render\n", client->request_num);

    std::ostringstream rep;
    rep << "HTTP/1.1 " << closure->response_code << "\r\n"
        << "Content-Type: " << closure->content_type << "\r\n"
        << "Connection: keep-alive\r\n"
        << "Content-Length: " << closure->result.size() << "\r\n"
        << "Access-Control-Allow-Origin: *"
        << "\r\n"
        << "\r\n";
    uv_buf_t resbuf;
    // 因为无论如何都需要加密，所以就不需下面的判断了
    // if(closure->result.size()+rep.str().size() > 65536)    //当大于65536，需要使用一个持久化内存来保存数据发送
    // {
    //     closure->buff = new char[closure->result.size()+rep.str().size()];
    //     memcpy(closure->buff, rep.str().c_str(), rep.str().size());
    //     memcpy(closure->buff+rep.str().size(), closure->result.c_str(), closure->result.size());
    //     // resbuf.base = closure->buff;
    //     // resbuf.len  = closure->result.size()+rep.str().size();
    //     client->write_req.data = closure;

    //     if(client->openssl_server.Encrypt(closure->buff, closure->result.size()+rep.str().size(), resbuf.base, resbuf.len) > 0)
    //     {
    //         LOGF("[ %5d ] Decrypt len: %d \n", client->request_num, resbuf.len);
    //         // https://github.com/joyent/libuv/issues/344
    //         int r = uv_write(&client->write_req,
    //                         (uv_stream_t *)&client->handle,
    //                         &resbuf,
    //                         1,
    //                         after_write);
    //         CHECK(r, "write buff");
    //         return;
    //     }
    //     else
    //     {
    //         LOG_ERROR("Openssl Encrypt ERROR");
    //         after_write((uv_write_t *)&client->handle, 0);
    //     }

    // }
    rep << closure->result;
    // std::string res = rep.str();
    // resbuf.base = (char *)res.c_str();
    // resbuf.len = res.size();

    // 这里打印的信息太多了，先注释掉
    // LOGF("res = %s ]\n", res.c_str());
    client->write_req.data = closure;

    if (client->openssl_server.Encrypt((char *)rep.str().c_str(), rep.str().size(), resbuf.base, resbuf.len) > 0)
    {
        LOGF("[ %5d ] Decrypt len: %d \n", client->request_num, resbuf.len);
        // https://github.com/joyent/libuv/issues/344
        int r = uv_write(&client->write_req,
                         (uv_stream_t *)&client->handle,
                         &resbuf,
                         1,
                         after_write);
        CHECK(r, "write buff");
    }
    else
    {

        LOG_ERROR("Openssl Encrypt ERROR");
        after_write((uv_write_t *)&client->handle, 0);
    }
}

int on_message_begin(http_parser * /*parser*/)
{
    LOGF("\n***MESSAGE BEGIN***\n");
    return 0;
}

int on_headers_complete(http_parser * /*parser*/)
{
    LOGF("\n***HEADERS COMPLETE***\n");
    return 0;
}

int on_url(http_parser *parser, const char *url, size_t length)
{
    client_t *client = (client_t *)parser->data;
    LOGF("[ %5d ] on_url\n", client->request_num);
    LOGF("Url: %.*s\n", (int)length, url);
    // TODO - use https://github.com/bnoordhuis/uriparser2 instead?
    struct http_parser_url u;
    int result = http_parser_parse_url(url, length, 0, &u);
    if (result)
    {
        fprintf(stderr, "\n\n*** failed to parse URL %s ***\n\n", url);
        return -1;
    }
    else
    {
        if ((u.field_set & (1 << UF_PATH)))
        {
            const char *data = url + u.field_data[UF_PATH].off;
            client->path = std::string(data, u.field_data[UF_PATH].len);
        }
        else
        {
            fprintf(stderr, "\n\n*** failed to parse PATH in URL %s ***\n\n", url);
            return -1;
        }
    }
    return 0;
}

int on_header_field(http_parser * /*parser*/, const char *at, size_t length)
{
    LOGF("Header field: %.*s\n", (int)length, at);
    return 0;
}

int on_header_value(http_parser * /*parser*/, const char *at, size_t length)
{
    LOGF("Header value: %.*s\n", (int)length, at);
    return 0;
}

int on_body(http_parser * /*parser*/, const char *at, size_t length)
{
    LOGF("Body: %.*s\n", (int)length, at);
    return 0;
}

int on_message_complete(http_parser *parser)
{
    client_t *client = (client_t *)parser->data;
    LOGF("[ %5d ] on_message_complete\n", client->request_num);
    render_baton *closure = new render_baton(client);
    int status = uv_queue_work(uv_loop,
                               &closure->request,
                               render,
                               (uv_after_work_cb)after_render);
    CHECK(status, "uv_queue_work");
    assert(status == 0);

    return 0;
}

void on_connect(uv_stream_t *server_handle, int status)
{
    CHECK(status, "connect");
    assert((uv_tcp_t *)server_handle == &server);

    client_t *client = new client_t();
    client->https_stage = HTTPS_HELLO;
    client->request_num = request_num;

    LOGF("[ %5d ] new connection\n", request_num);
    request_num++;

    uv_tcp_init(uv_loop, &client->handle);
    http_parser_init(&client->parser, HTTP_REQUEST);

    client->parser.data = client;
    client->handle.data = client;

    int r = uv_accept(server_handle, (uv_stream_t *)&client->handle);
    CHECK(r, "accept");

    uv_read_start((uv_stream_t *)&client->handle, alloc_cb, on_read);
}

#define MAX_WRITE_HANDLES 1000

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("usage %s <port>\n", argv[0]);
        exit(1);
    }
    g_server_port = argv[1];
    signal(SIGPIPE, SIG_IGN);
    int cores = sysconf(_SC_NPROCESSORS_ONLN);
    printf("number of cores %d\n", cores);
    init_ssl_ctx(SERVER_CRT, SERVER_KEY);
    char cores_string[10];
    sprintf(cores_string, "%d", cores);
    setenv("UV_THREADPOOL_SIZE", cores_string, 1);
    parser_settings.on_url = on_url;
    // notification callbacks
    parser_settings.on_message_begin = on_message_begin;
    parser_settings.on_headers_complete = on_headers_complete;
    parser_settings.on_message_complete = on_message_complete;
    // data callbacks
    parser_settings.on_header_field = on_header_field;
    parser_settings.on_header_value = on_header_value;
    parser_settings.on_body = on_body;
    uv_loop = uv_default_loop();
    int r = uv_tcp_init(uv_loop, &server);
    CHECK(r, "tcp_init");
    r = uv_tcp_keepalive(&server, 1, 60);
    CHECK(r, "tcp_keepalive");
    struct sockaddr_in address;
    r = uv_ip4_addr("0.0.0.0", atoi(argv[1]), &address);
    CHECK(r, "ip4_addr");
    r = uv_tcp_bind(&server, (const struct sockaddr *)&address, 0);
    CHECK(r, "tcp_bind");
    r = uv_listen((uv_stream_t *)&server, MAX_WRITE_HANDLES, on_connect);
    CHECK(r, "uv_listen");
    fprintf(stderr, "listening on port :%d\n", atoi(argv[1]));
    uv_run(uv_loop, UV_RUN_DEFAULT);

    // If uv_run returned, close the default loop before exiting.
    return uv_loop_close(uv_default_loop());
}