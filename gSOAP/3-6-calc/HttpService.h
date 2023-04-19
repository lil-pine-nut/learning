#ifndef __HTTP__SERVICE__H__
#define __HTTP__SERVICE__H__

#include "soapcalcService.h"

class HttpService : public calcService
{
public:
    HttpService(/* args */);
    ~HttpService();

    bool Init(int socket = -1);
    bool SetSocket(int sock);

private:
    //如果是非static函数，将会报错：无法将‘HttpService::HttpGetFun’从类型‘int (HttpService::)(soap*)’转换到类型‘int (*)(soap*)’
    static int HttpPostFun(struct soap *soap);
    static int HttpGetFun(struct soap *soap);

    int HttpGetIndex(struct soap *soap);

    void ExecuteAdd(struct soap *soap);
};

#endif