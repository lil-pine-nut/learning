#ifndef __MY__HTTP_SERVER__H__
#define __MY__HTTP_SERVER__H__

#include <event2/event.h>
#include <evhttp.h>
#include <sys/stat.h>

static bool closesign = false;
//获取Content-Type
const char *get_file_type(char *name);
//发送目录html
int send_dir(struct evbuffer *bev, const char *dirname);
// http头
int send_header(struct evhttp_request *request, const char *filename, long filelen, const char *connest);
//传送文件
int send_file_to_http(const char *filename, struct evbuffer *bev);
//回调函数
void HttpGenericCallback(struct evhttp_request *request, void *arg);

class Myhttpserver
{
public:
	Myhttpserver();
	~Myhttpserver();

private:
	struct event_base *base;
	struct evhttp *http;
	char errmsg[512];

public:
	//初始化
	bool inithttp();
	//打开服务器
	bool start(unsigned int port);
	//调用回调
	void set_gencb(void (*cb)(struct evhttp_request *, void *));
	//循环处理
	void dispatch();
	static void stop(struct event_base *base);
	void free();
	char *geterrmsg();
};

#endif