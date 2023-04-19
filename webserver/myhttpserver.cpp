#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>
#include <time.h>
#include <signal.h>
#include <ctype.h>
#include <errno.h>
#include <iostream>
#include "myhttpserver.h"

using namespace std;
/*
charset=iso-8859-1	西欧的编码，说明网站采用的编码是英文；
charset=gb2312		说明网站采用的编码是简体中文；
charset=utf-8			代表世界通用的语言编码；
						可以用到中文、韩文、日文等世界上所有语言编码上
charset=euc-kr		说明网站采用的编码是韩文；
charset=big5			说明网站采用的编码是繁体中文；

以下是依据传递进来的文件名，使用后缀判断是何种文件类型
将对应的文件类型按照http定义的关键字发送回去
*/
const char *get_file_type(const char *name)
{
	char *dot;
	char tmpname[128] = {0};
	strncpy(tmpname, name, strlen(name));
	dot = strrchr(tmpname, '.'); //自右向左查找‘.’字符;如不存在返回NULL

	if (dot == (char *)0)
		return "text/plain; charset=utf-8";
	if (strcmp(dot, ".html") == 0 || strcmp(dot, ".htm") == 0)
		return "text/html; charset=utf-8";
	if (strcmp(dot, ".jpg") == 0 || strcmp(dot, ".jpeg") == 0)
		return "image/jpeg";
	if (strcmp(dot, ".gif") == 0)
		return "image/gif";
	if (strcmp(dot, ".png") == 0)
		return "image/png";
	if (strcmp(dot, ".css") == 0)
		return "text/css";
	if (strcmp(dot, ".au") == 0)
		return "audio/basic";
	if (strcmp(dot, ".wav") == 0)
		return "audio/wav";
	if (strcmp(dot, ".avi") == 0)
		return "video/x-msvideo";
	if (strcmp(dot, ".mov") == 0 || strcmp(dot, ".qt") == 0)
		return "video/quicktime";
	if (strcmp(dot, ".mpeg") == 0 || strcmp(dot, ".mpe") == 0)
		return "video/mpeg";
	if (strcmp(dot, ".vrml") == 0 || strcmp(dot, ".wrl") == 0)
		return "model/vrml";
	if (strcmp(dot, ".midi") == 0 || strcmp(dot, ".mid") == 0)
		return "audio/midi";
	if (strcmp(dot, ".mp3") == 0)
		return "audio/mpeg";
	if (strcmp(dot, ".ogg") == 0)
		return "application/ogg";
	if (strcmp(dot, ".pac") == 0)
		return "application/x-ns-proxy-autoconfig";

	return "text/plain; charset=utf-8";
}

int send_header(struct evhttp_request *request, const char *filename, long filelen, const char *connest)
{

	// 文件类型
	evhttp_add_header(request->output_headers, "Content-Type", get_file_type(filename));
	// 文件大小
	if (filelen > 0)
	{
		char charsize[50] = {0};
		sprintf(charsize, "%ld", filelen);
		evhttp_add_header(request->output_headers, "Content-Length", charsize);
	}
	// Connection: close/keep-alive
	evhttp_add_header(request->output_headers, "Connection", connest);

	return 0;
}

int send_dir(struct evbuffer *bev, const char *dirname)
{
	char encoded_name[1024];
	char path[1024];
	char timestr[64];
	struct stat sb;
	struct dirent **dirinfo;

	char buf[4096] = {0};
	sprintf(buf, "<html><head><meta charset=\"utf-8\"><title>%s</title></head>", dirname);
	sprintf(buf + strlen(buf), "<body><h1>当前目录：%s</h1><table>", dirname);
	//添加目录内容
	int num = scandir(dirname, &dirinfo, NULL, alphasort);
	cerr << "num:" << num << ", dirname:" << dirname << endl;
	for (int i = 0; i < num; ++i)
	{
		// 编码
		strcpy(encoded_name, dirinfo[i]->d_name);

		sprintf(path, "%s%s", dirname, dirinfo[i]->d_name);
		printf("############# path = %s\n", path);
		if (lstat(path, &sb) < 0)
		{
			sprintf(buf + strlen(buf),
					"<tr><td><a href=\"%s\">%s</a></td></tr>\n",
					encoded_name, dirinfo[i]->d_name);
		}
		else
		{
			strftime(timestr, sizeof(timestr),
					 "  %d  %b   %Y  %H:%M", localtime(&sb.st_mtime));
			if (S_ISDIR(sb.st_mode))
			{
				sprintf(buf + strlen(buf),
						"<tr><td><a href=\"%s/\">%s/</a></td><td>%s</td><td>%ld</td></tr>\n",
						encoded_name, dirinfo[i]->d_name, timestr, sb.st_size);
			}
			else
			{
				sprintf(buf + strlen(buf),
						"<tr><td><a href=\"%s\">%s</a></td><td>%s</td><td>%ld</td></tr>\n",
						encoded_name, dirinfo[i]->d_name, timestr, sb.st_size);
			}
		}
		// bufferevent_write(bev, buf, strlen(buf));
		evbuffer_add_printf(bev, buf);
		printf(buf);
		memset(buf, 0, sizeof(buf));
	}
	sprintf(buf + strlen(buf), "</table></body></html>");
	// bufferevent_write(bev, buf, strlen(buf));
	evbuffer_add_printf(bev, buf);
	printf(buf);
	printf("################# Dir Read OK !!!!!!!!!!!!!!\n");

	return 0;
}

int send_file_to_http(const char *filename, struct evbuffer *bev)
{
	int fd = open(filename, O_RDONLY);
	int ret = 0;
	char buf[4096] = {0};
	//单次上限4096
	// evbuffer_read(bev, fd, 30556);
	while ((ret = read(fd, buf, sizeof(buf))))
	{
		if (ret < 0)
			break;
		//不能传\0
		// evbuffer_add_printf(bev, buf);
		//可以传\0
		evbuffer_add(bev, buf, ret);
		memset(buf, 0, 4096);
	}
	close(fd);
	return 0;
}
//回调函数
void HttpGenericCallback(struct evhttp_request *request, void *arg)
{
	const struct evhttp_uri *evhttp_uri = evhttp_request_get_evhttp_uri(request);
	char url[8192];
	memset(url, 0x00, sizeof(url));
	evhttp_uri_join(const_cast<struct evhttp_uri *>(evhttp_uri), url, 8192);

	printf("accept request url:%s\n", url);

	if (request->kind == EVHTTP_REQUEST)
	{
		// get
		if (request->type == EVHTTP_REQ_GET)
		{
			/*
			http://foo.com/?q=test&s=some+thing
			evhttp_request_uri: 解析HTTP请求中的ur，得到/?q=test&s=some+thing
			evhttp_parse_query: 解析名值对，得到一个evkeyvalq结构，里面包含了key/value的数组.
			*/
			char *decode_uri = strdup((char *)evhttp_request_uri(request));
			struct evkeyvalq http_query;
			evhttp_parse_query(decode_uri, &http_query);
			free(decode_uri);

			// const char *request_value = evhttp_find_header(&http_query, "data");
			//解码
			// strcpy(url, evhttp_decode_uri(url));
			char *pf = &url[1];

			if (strcmp(url, "/") == 0 || strcmp(url, "/.") == 0)
			{
				pf = "./";
			}

			printf("***** http Request Resource Path =  %s, pf = %s\n", url, pf);
			//退出服务器
			if (strcmp(url, "/bye") == 0)
			{
				closesign = true;
				struct event_base *base = (struct event_base *)arg;
				Myhttpserver::stop(base);
				return;
			}
			struct stat sb;
			struct evbuffer *evbuf = evbuffer_new();
			if (!evbuf)
			{
				printf("create evbuffer failed!\n");
				return;
			}
			char dir[256] = {0}, filename[256] = {0};
			snprintf(dir, sizeof(dir), "/data4/lws/learning/learn-network/build/%s", pf);
			//打开文件
			if (stat(dir, &sb) == 0 && (sb.st_mode & S_IFREG))
			{
				send_header(request, pf, sb.st_size, "close");
				send_file_to_http(dir, evbuf);
				evhttp_send_reply(request, HTTP_OK, "OK", evbuf);

				evbuffer_free(evbuf);
				return;
			}
			//打开子文件夹
			else if (stat(dir, &sb) == 0 && (sb.st_mode & __S_IFDIR))
			{
				send_header(request, ".html", 0, "close");
				send_dir(evbuf, dir);
				evhttp_send_reply(request, HTTP_OK, "OK", evbuf);
				evhttp_clear_headers(&http_query);
				evbuffer_free(evbuf);
				return;
			}
			//打开主文件夹
			else
			{
				send_header(request, ".html", 0, "close");
				send_dir(evbuf, "/data4/lws/learning/learn-network/build");
				evhttp_send_reply(request, HTTP_OK, "OK", evbuf);
				evhttp_clear_headers(&http_query);
				evbuffer_free(evbuf);
				return;
			}
		}
		// post
		else if (request->type == EVHTTP_REQ_POST)
		{
			char charadd1[10], charadd2[10];
			struct evkeyvalq http_query_post;
			char decode_post_uri[1024] = {0};
			int buffer_data_len = EVBUFFER_LENGTH(request->input_buffer);
			char *post_data = (char *)malloc(buffer_data_len + 1);
			memset(post_data, 0, buffer_data_len + 1);
			memcpy(post_data, EVBUFFER_DATA(request->input_buffer), buffer_data_len);
			sprintf(decode_post_uri, "/?%s", post_data); //处理数据格式
			evhttp_parse_query(decode_post_uri, &http_query_post);
			strcpy(charadd1, evhttp_find_header(&http_query_post, "add1"));
			strcpy(charadd2, evhttp_find_header(&http_query_post, "add2"));

			int sum = atoi(charadd1) + atoi(charadd2);

			struct evbuffer *evbuf = evbuffer_new();
			if (!evbuf)
			{
				printf("create evbuffer failed!\n");
				return;
			}

			evbuffer_add_printf(evbuf, "<!DOCTYPE html>\
							<html>\
							<head>\
							<meta charset = \"utf-8\">\
							<title>(runoob.com)</title>\
							</head>\
							<body>\
							<p>%d</p>\
							</body>\
							</html>",
								sum);
			evhttp_send_reply(request, HTTP_OK, "OK", evbuf);
			evbuffer_free(evbuf);
		}
	}
}

Myhttpserver::Myhttpserver()
{
}

Myhttpserver::~Myhttpserver()
{
}

bool Myhttpserver::inithttp()
{
	event_init();
	base = event_base_new();
	if (!base)
	{
		sprintf(errmsg, "create event_base failed,error: %s(errno: %d)\n", strerror(errno), errno);
		return false;
	}
	http = evhttp_new(base);
	if (!http)
	{
		sprintf(errmsg, "create evhttp failed,error: %s(errno: %d)\n", strerror(errno), errno);
		return false;
	}
	return true;
}

bool Myhttpserver::start(unsigned int port)
{
	if (evhttp_bind_socket(http, "0.0.0.0", port) != 0)
	{
		sprintf(errmsg, "start evhttp failed,error: %s(errno: %d)\n", strerror(errno), errno);
		return false;
	}
	evhttp_set_timeout(http, 120);
	return true;
}

void Myhttpserver::set_gencb(void (*cb)(struct evhttp_request *, void *))
{
	evhttp_set_gencb(http, cb, base);
}

void Myhttpserver::dispatch()
{
	event_base_dispatch(base);
}

void Myhttpserver::stop(struct event_base *base)
{
	event_base_loopbreak(base);
	event_base_loopexit(base, 0);
	return;
}

void Myhttpserver::free()
{
	evhttp_free(http);
	event_base_free(base);
}

char *Myhttpserver::geterrmsg()
{
	return errmsg;
}
