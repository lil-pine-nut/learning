#include "calc.nsmap"
#include "soapH.h"
#include "HttpService.h"

/////////////////////////////////////////////////////////////////////////
///宏与全局变量的定义
#define BACKLOG (100)
#define MAX_THR (10)
#define MAX_QUEUE (1000)

pthread_mutex_t queue_cs;     //队列锁
pthread_cond_t queue_cv;      //条件变量
SOAP_SOCKET queue[MAX_QUEUE]; //数组队列
int head = 0, tail = 0;       //队列头队列尾初始化
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
void *process_queue(void *); //线程入口函数
int enqueue(SOAP_SOCKET);    //入队列函数
SOAP_SOCKET dequeue(void);   //出队列函数

//////////////////////////////////////////////////////////////////////////
//线程入口函数
void *process_queue(void *args)
{
    int thread_id = *(int *)args;
    HttpService serv;
    serv.Init();
    int socket = -1;
    for (;;)
    {
        socket = dequeue();
        if (!soap_valid_socket(socket))
        {
            break;
        }
        serv.SetSocket(socket);
        serv.serve();
        close(socket);
        fprintf(stderr, "Thread [%d] execute task successful \n", thread_id);
    }
    fprintf(stderr, "Thread [%d] get kill.\n", thread_id);
    return NULL;
}

//入队列操作
int enqueue(SOAP_SOCKET sock)
{
    int status = SOAP_OK;
    int next;
    pthread_mutex_lock(&queue_cs);
    next = tail + 1;
    if (next >= MAX_QUEUE)
        next = 0;
    if (next == head)
        status = SOAP_EOM;
    else
    {
        queue[tail] = sock;
        tail = next;
    }
    pthread_cond_signal(&queue_cv);
    pthread_mutex_unlock(&queue_cs);
    return status;
}

//出队列操作
SOAP_SOCKET dequeue()
{
    SOAP_SOCKET sock;
    pthread_mutex_lock(&queue_cs);
    while (head == tail)
    {
        pthread_cond_wait(&queue_cv, &queue_cs);
    }
    sock = queue[head++];
    if (head >= MAX_QUEUE)
    {
        head = 0;
    }
    pthread_mutex_unlock(&queue_cs);
    return sock;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//主函数
int main(int argc, char **argv)
{
    struct soap ServerSoap;
    //初始话运行时环境
    soap_init(&ServerSoap);
    //如果没有参数，当作CGI程序处理
    if (argc < 2)
    {
        // //CGI 风格服务请求，单线程
        // soap_serve(&ServerSoap);
        // //清除序列化的类的实例
        // soap_destroy(&ServerSoap);
        // //清除序列化的数据
        // soap_end(&ServerSoap);

        std::cerr << "Input fomat: Execute-Bin <Port>" << std::endl;
        return 0;
    }
    else
    {
        pthread_t tid[MAX_THR];
        int i, port = atoi(argv[1]);
        SOAP_SOCKET m, s;
        //锁和条件变量初始化
        pthread_mutex_init(&queue_cs, NULL);
        pthread_cond_init(&queue_cv, NULL);
        //绑定服务端口
        m = soap_bind(&ServerSoap, NULL, port, BACKLOG);
        //循环直至服务套接字合法
        while (!soap_valid_socket(soap_bind(&ServerSoap, NULL, port, BACKLOG)))
        {
            fprintf(stderr, "Bind port error! \n");
#ifdef _WIN32
            Sleep(1000);
#else
            sleep(1);
#endif
        }
        fprintf(stderr, "socket connection successful %d\n", m);

        //生成服务线程
        for (i = 0; i < MAX_THR; i++)
        {
            fprintf(stderr, "Starting thread %d\n", i);
            pthread_create(&tid[i], NULL, (void *(*)(void *))process_queue, (void *)&i);
        }

        for (;;)
        {
            //接受客户端的连接
            s = soap_accept(&ServerSoap);
            if (!soap_valid_socket(s))
            {
                if (ServerSoap.errnum)
                {
                    soap_print_fault(&ServerSoap, stderr);
                    continue;
                }
                else
                {
                    fprintf(stderr, "Server timed out \n");
                    break;
                }
            }
            //客户端的IP地址
            fprintf(stderr, "Accepted connection from IP= %d.%d.%d.%d socket = %d \n",
                    ((ServerSoap.ip) >> 24) && 0xFF, ((ServerSoap.ip) >> 16) & 0xFF, ((ServerSoap.ip) >> 8) & 0xFF, (ServerSoap.ip) & 0xFF, (ServerSoap.socket));
            //请求的套接字进入队列，如果队列已满则循环等待
            while (enqueue(s) == SOAP_EOM)
#ifdef _WIN32
                Sleep(1000000);
#else
                sleep(1000);
#endif
        }
        //服务结束后的清理工作
        for (i = 0; i < MAX_THR; i++)
        {
            while (enqueue(SOAP_INVALID_SOCKET) == SOAP_EOM)
            {
#ifdef _WIN32
                Sleep(1000000);
#else
                sleep(1000);
#endif
            }
        }
        for (i = 0; i < MAX_THR; i++)
        {
            fprintf(stderr, "Waiting for thread %d toterminate ..", i);
            pthread_join(tid[i], NULL);
            fprintf(stderr, "terminated ");
        }
        pthread_mutex_destroy(&queue_cs);
        pthread_cond_destroy(&queue_cv);
    }
    //分离运行时的环境
    soap_done(&ServerSoap);
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