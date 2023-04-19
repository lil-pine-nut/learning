#include "HttpService.h"
#include "httpform.h"

#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

HttpService::HttpService(/* args */)
{
}

HttpService::~HttpService()
{
}

bool HttpService::Init(int socket)
{
	soap_set_namespaces(this, namespaces);
	this->user = this; // 因为 HttpGetFun HttpPostFun为static函数
	this->fget = this->HttpGetFun;

	if (soap_register_plugin_arg(this, http_form, (void *)this->HttpPostFun))
	{
		fprintf(stderr, "GSOAP:register post fun Err!\n");
	}

	SetSocket(socket);
	return true;
}

bool HttpService::SetSocket(int sock)
{
	if (sock != -1)
	{
		this->socket = sock;
#ifdef _WIN32
		this->socket_flags = 0;
#else
		this->socket_flags = MSG_NOSIGNAL;
#endif
	}
	else
	{
		return false;
	}

	return true;
}

int HttpService::HttpPostFun(struct soap *soap)
{
	string uri = soap->path;
	HttpService *_this = (HttpService *)soap->user;
	cerr << __PRETTY_FUNCTION__ << " get uri = " << uri << endl;
	if (uri == "/add")
	{
		_this->ExecuteAdd(soap);
	}
	return SOAP_OK;
}

int HttpService::HttpGetFun(struct soap *soap)
{
	HttpService *_this = (HttpService *)soap->user;
	string uri = soap->path;
	cerr << __PRETTY_FUNCTION__ << " get uri = " << uri << endl;

	if (uri == "/")
	{
		return _this->HttpGetIndex(soap);
	}
	// return SOAP_OK; // 刷新界面会响应 /favicon.ico 两次
	return SOAP_GET_METHOD;
}

int HttpService::HttpGetIndex(struct soap *soap)
{
	ifstream ifs;
	ifs.open(CALC_INDEX_HTML);
	char buff[1024] = {'\0'};
	soap_response(soap, SOAP_HTML);

	for (;;)
	{
		ifs.read(buff, sizeof(buff));
		int len = ifs.gcount();
		if (len > 0)
			soap_send_raw(soap, buff, len);
		else
			break;
	}

	soap_end_send(soap);

	return SOAP_OK;
}

//模板函数：将string类型变量转换为常用的数值类型
template <class Type>
Type String2Num(const string &str)
{
	istringstream iss(str);
	Type num;
	iss >> num;
	return num;
}

//模板函数：将Type类型变量转换为string
template <class Type>
string Num2String(const Type &val)
{
	ostringstream oss;
	oss << val;
	return oss.str();
}

void HttpService::ExecuteAdd(struct soap *soap)
{
	HttpService *_this = (HttpService *)soap->user;

	string result, num1, num2;

	char *s = soap_http_get_form(soap); /* data from post */
	while (s)
	{
		char *key = soap_query_key(soap, &s); /* decode next key */
		char *val = soap_query_val(soap, &s); /* decode next value (if any) */

		if (key)
		{
			cerr << "key = " << key << ", val = " << val << endl;
			if (strcmp(key, "num1") == 0)
				num1 = val;
			if (strcmp(key, "num2") == 0)
				num2 = val;
		}
	}

	result = Num2String(atof(num1.c_str()) + atof(num2.c_str()));
	cerr << "result = " << result << endl;
	soap_response(soap, SOAP_HTML);
	string html_str = "<link rel=\"icon\" href=\"data:;base64,=\">  <!-- 不希望产生 /favicon.ico 的请求 -->";
	html_str += result;
	soap_send(soap, html_str.c_str());
	soap_end_send(soap);
}