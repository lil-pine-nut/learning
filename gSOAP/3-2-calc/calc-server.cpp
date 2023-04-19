// #define _CRT_SECURE_NO_WARNINGS   //一定要添加上
#include "calc.nsmap"
#include "soapcalcService.h"
#include "iostream" //控件问提只能写“”

using namespace std;

//很重要
int http_get(struct soap *soap)
{
    FILE *fd = NULL;
    fd = fopen(CALC_WSDL_PATH, "rb"); // open WSDL file to copy

    if (!fd)
    {
        return 404; // return HTTP not found error
    }
    soap->http_content = "text/xml"; // HTTP header with text /xml content
    soap_response(soap, SOAP_FILE);
    for (;;)
    {
        size_t r = fread(soap->tmpbuf, 1, sizeof(soap->tmpbuf), fd);
        if (!r)
        {
            break;
        }
        if (soap_send_raw(soap, soap->tmpbuf, r))
        {
            break; // cannot send, but little we can do about that
        }
    }
    fclose(fd);
    soap_end_send(soap);
    return SOAP_OK;
}

int main(int argc, char *argv[])
{
    calcService cal;
    cal.fget = http_get;
    while (1)
    {
        if (cal.run(8089))
        {
            cal.soap_stream_fault(std::cerr);
        }
    }
    return 0;
}

//自动生成了calcService类，自己重写add等函数
/*加法的具体实现*/
int calcService::add(double num1, double num2, double *result)
{
    if (NULL == result)
    {
        printf("Error:The third argument should not be NULL!\n");
        return SOAP_ERR;
    }
    else
    {
        (*result) = num1 + num2;
        return SOAP_OK;
    }
    return SOAP_OK;
}

/*减法的具体实现*/
int calcService::sub(double num1, double num2, double *result)
{
    if (NULL == result)
    {
        printf("Error:The third argument should not be NULL!\n");
        return SOAP_ERR;
    }
    else
    {
        (*result) = num1 - num2;
        return SOAP_OK;
    }
    return SOAP_OK;
}

/*乘法的具体实现*/
int calcService::mul(double num1, double num2, double *result)
{
    if (NULL == result)
    {
        printf("Error:The third argument should not be NULL!\n");
        return SOAP_ERR;
    }
    else
    {
        (*result) = num1 * num2;
        return SOAP_OK;
    }
    return SOAP_OK;
}

/*除法的具体实现*/
int calcService::div(double num1, double num2, double *result)
{
    if (NULL == result || 0 == num2)
    {
        return soap_senderfault("Square root of negative value", "I can only compute the square root of a non-negative value");
        return SOAP_ERR;
    }
    else
    {
        (*result) = num1 / num2;
        return SOAP_OK;
    }
    return SOAP_OK;
}

int calcService::pow(double num1, double num2, double *result)
{
    if (NULL == result || 0 == num2)
    {
        printf("Error:The second argument is 0 or The third argument is NULL!\n");
        return SOAP_ERR;
    }
    else
    {
        (*result) = num1 / num2;
        return SOAP_OK;
    }
    return SOAP_OK;
}