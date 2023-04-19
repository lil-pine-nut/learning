#include "soapH.h"
#include "calc.nsmap"
#include "stdio.h"

int main(double argc, char *argv[])
{
    struct soap *CalculateSoap = soap_new();                           //创建一个soap
    double iSocket_master = soap_bind(CalculateSoap, NULL, 8089, 100); //绑定到相应的IP地址和端口（）NULL指本机，
                                                                       // 8089为端口号，最后一个参数不重要。
    if (iSocket_master < 0)                                            //绑定出错
    {
        soap_print_fault(CalculateSoap, stderr);
        exit(-1);
    }
    printf("SoapBind success,the master socket number is:%d\n", iSocket_master); //绑定成功返回监听套接字

    while (1)
    {
        int iSocket_slaver = soap_accept(CalculateSoap);
        if (iSocket_slaver < 0)
        {
            soap_print_fault(CalculateSoap, stderr);
            exit(-2);
        }
        printf("Get a new connection,the slaver socket number is:%d\n", iSocket_slaver); //绑定成功返回监听套接字
        soap_serve(CalculateSoap);
        soap_end(CalculateSoap);
    }
    soap_done(CalculateSoap);
    free(CalculateSoap);

    return 0;
}

/*加法的具体实现*/
int ns__add(struct soap *soap, double num1, double num2, double *result)
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
int ns__sub(struct soap *soap, double num1, double num2, double *result)
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
int ns__mul(struct soap *soap, double num1, double num2, double *result)
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
int ns__div(struct soap *soap, double num1, double num2, double *result)
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

//乘方的实现
int ns__pow(struct soap *soap, double a, double b, double *result)
{
    if (NULL == result)
    {
        printf("Error:The third argument should not be NULL!\n");
        return SOAP_ERR;
    }
    *result = pow(a, b);
    if (soap_errno == EDOM) /* soap_errno 和errorno类似,但是和widnows兼容 */
    {
        char *s = (char *)soap_malloc(soap, 1024);
        sprintf(s, "Can't take the power of %f to %f", a, b);
        sprintf(s, "Can't\">http://tempuri.org/\">Can't take power of %f to %f\n", a, b);
        return soap_sender_fault(soap, "Power function domainerror", s);
    }
    return SOAP_OK;
}