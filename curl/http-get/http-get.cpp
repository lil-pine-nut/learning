#include <iostream>
#include <curl/curl.h>
using namespace std;

// C/C++中libcurl的使用-Http GET方法使用详解: https://blog.csdn.net/cjf_wei/article/details/79118415

/* 不考虑任何异常的情况, 简单的get请求 */
/***
 *  buffer 		接收数据所在的缓冲区
 * 	size		要读取的字节数
 *  count		读写size长度的数据count次
 *  response	用户自定义文件指针
 */
size_t getUrlResponse(char *buffer, size_t size, size_t count, string *response)
{
	size_t recv_size = size * count;
	response->clear();
	response->append(buffer);
	return recv_size;
}

string get(const string &url)
{
	// 请求数据
	string response;
	// easy handle声明
	CURL *handle;
	// 初始化handle
	handle = curl_easy_init();
	// 设置url
	curl_easy_setopt(handle, CURLOPT_URL, url.c_str());
	// 注册回调函数
	curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, &getUrlResponse);
	/* 可以注释 set CURLOPT_WRITEDATA, 同时将回调函数恢复为上文中的原型, 看看会发生什么 */
	// 获取信息
	curl_easy_setopt(handle, CURLOPT_WRITEDATA, &response);
	// 执行请求
	curl_easy_perform(handle);
	// 释放
	curl_easy_cleanup(handle);

	return response;
}
int main()
{
	// 使用前初始化libcurl, 只需初始化一次
	curl_global_init(CURL_GLOBAL_DEFAULT);
	// 执行请求
	cout << get("www.baidu.com");
	// 释放libcurl相关资源
	curl_global_cleanup();
	return 0;
}
