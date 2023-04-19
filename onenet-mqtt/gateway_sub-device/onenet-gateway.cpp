#include <stdio.h>
#include <time.h>

// onenet_studio_sdk
#include "tm_data.h"
#include "data_types.h"
#include "tm_api.h"
// #include "tm_user.h"
#include "tm_subdev.h"

#include "cJSON.h"

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <unistd.h>
#include <signal.h>

// epoll
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <unistd.h>
#include <sys/types.h>

#define IPADDRESS "127.0.0.1"
#define PORT 8787
#define MAXSIZE 1024
#define LISTENQ 5
#define FDSIZE 1000
#define EPOLLEVENTS 100

using namespace std;

struct onenet_device_info
{
    int sub_device_fd;
    string DeviceName;
    string ProductID;
    string SecKey;
};
typedef map<string, onenet_device_info> sub_device_map;
typedef map<int, onenet_device_info> sub_device_fd_map;
sub_device_map g_sub_device_map;
sub_device_fd_map g_sub_device_fd_map;

char g_event_buff[MAXSIZE];
int g_event_buff_size = 0;
bool g_run = true;
int g_event_fd = -1;
int g_sub_device_fd;

static void sigterm(int sig)
{
    g_run = false;
}

int my_subdev_props_get(const int8_t *product_id, const int8_t *dev_name, const int8_t *props_list, int8_t **props_data)
{
    printf("my_subdev_props_get: [%s]\n", props_data);
    return 0;
}

int my_subdev_props_set(const int8_t *product_id, const int8_t *dev_name, int8_t *props_data)
{
    sub_device_map::iterator it = g_sub_device_map.find((char *)dev_name);
    if (it != g_sub_device_map.end())
    {
        g_event_buff_size = sprintf(g_event_buff, "%s", props_data);
        write(g_event_fd, "event_fd", 8);
        g_sub_device_fd = it->second.sub_device_fd;
    }
    // g_event_buff_size =  sprintf(g_event_buff, "%s:%s", dev_name, props_data);
    // g_event_buff_size =  strlen(g_event_buff);
    // printf("my_subdev_props_set: product_id:%s - dev_name - %s - [%s]\n", product_id, dev_name, props_data);
    // printf("g_event_buff:%s\n", g_event_buff);
    // write(g_event_fd, "event_fd", 8);
    return 0;
}

int my_subdev_service_invoke(const int8_t *product_id, const int8_t *dev_name, const int8_t *svc_id, int8_t *in_data, int8_t **out_data)
{
    printf("my_subdev_service_invoke: [%s]\n", in_data);
    return 0;
}

int my_subdev_topo(int8_t *topo_data)
{

    printf("my_subdev_topo:[%s]\n", topo_data);
    return 0;
}

bool ReadDevicesConfig(const char *decives_json, onenet_device_info &gateway_info, vector<onenet_device_info> &sub_devices_infos)
{
    FILE *file = fopen(decives_json, "rb");
    if (!file)
    {
        cerr << "fopen " << decives_json << " Failed!" << endl;
        return false;
    }
    string messages;
    fseek(file, 0, SEEK_END);
    unsigned size = ftell(file);
    fseek(file, 0, SEEK_SET);
    messages.resize(size);
    fread(&messages[0], size, 1, file);
    fclose(file);

    cJSON *cjson_parse = NULL;
    /* 解析整段JSO数据 */
    cjson_parse = cJSON_Parse(messages.c_str());
    if (cjson_parse == NULL)
    {
        printf("parse fail.\n");
        return false;
    }

    /* 依次根据名称提取JSON数据（键值对） */
    cJSON *cjson_description = NULL;
    cjson_description = cJSON_GetObjectItem(cjson_parse, "description");
    printf("description: %s\n", cjson_description->valuestring);

    /* 解析嵌套json数据 */
    cJSON *cjson_gateway = NULL;
    cJSON *cjson_DeviceName = NULL;
    cJSON *cjson_ProductID = NULL;
    cJSON *cjson_SecKey = NULL;
    cjson_gateway = cJSON_GetObjectItem(cjson_parse, "gateway");
    cjson_DeviceName = cJSON_GetObjectItem(cjson_gateway, "DeviceName");
    cjson_ProductID = cJSON_GetObjectItem(cjson_gateway, "ProductID");
    cjson_SecKey = cJSON_GetObjectItem(cjson_gateway, "SecKey");
    printf("DeviceName: %s\n", cjson_DeviceName->valuestring);
    printf("ProductID: %s\n", cjson_ProductID->valuestring);
    printf("SecKey: %s\n", cjson_SecKey->valuestring);
    gateway_info.DeviceName = cjson_DeviceName->valuestring;
    gateway_info.ProductID = cjson_ProductID->valuestring;
    gateway_info.SecKey = cjson_SecKey->valuestring;

    /* 解析数组 */
    cJSON *cjson_devices = NULL;
    cjson_devices = cJSON_GetObjectItem(cjson_parse, "sub-devices");
    int devices_array_size = cJSON_GetArraySize(cjson_devices);

    cJSON *cjson_devices_array = NULL;
    for (int i = 0; i < devices_array_size; i++)
    {
        cjson_devices_array = cJSON_GetArrayItem(cjson_devices, i);
        cjson_DeviceName = cJSON_GetObjectItem(cjson_devices_array, "DeviceName");
        cjson_ProductID = cJSON_GetObjectItem(cjson_devices_array, "ProductID");
        cjson_SecKey = cJSON_GetObjectItem(cjson_devices_array, "SecKey");
        printf("DeviceName: %s\n", cjson_DeviceName->valuestring);
        printf("ProductID: %s\n", cjson_ProductID->valuestring);
        printf("SecKey: %s\n", cjson_SecKey->valuestring);

        onenet_device_info tmp_device_info;
        tmp_device_info.DeviceName = cjson_DeviceName->valuestring;
        tmp_device_info.ProductID = cjson_ProductID->valuestring;
        tmp_device_info.SecKey = cjson_SecKey->valuestring;
        sub_devices_infos.push_back(tmp_device_info);
    }

    cJSON_Delete(cjson_parse);

    return true;
}

int main(int argc, char **argv)
{
    if (argc <= 1)
    {
        cerr << "usage: " << argv[0] << " <devices-json-file>" << endl;
        return -1;
    }

    onenet_device_info gateway_info;
    vector<onenet_device_info> sub_devices_infos;
    if (!ReadDevicesConfig(argv[1], gateway_info, sub_devices_infos))
    {
        cerr << "ReadDevicesConfig Failed!" << endl;
        return -1;
    }

    struct tm_downlink_tbl_t downlink_tbl = {0};

    // downlink_tbl.prop_tbl = tm_prop_list;
    // downlink_tbl.prop_tbl_size = tm_prop_list_size;
    // downlink_tbl.svc_tbl = tm_svc_list;
    // downlink_tbl.svc_tbl_size = tm_svc_list_size;

    if (0 == tm_init(&downlink_tbl))
    {
        printf("tm init ok\n");
    }
    else
    {
        printf("tm_init failed\n");
        return -1;
    }

    // tm_login 需要时间长一些
    if (0 == tm_login((int8_t *)gateway_info.ProductID.c_str(), (int8_t *)gateway_info.DeviceName.c_str(), (int8_t *)gateway_info.SecKey.c_str(), 5000))
    {
        printf("tm api login ok\n");
    }
    else
    {
        printf("tm api login failed\n");
        return -1;
    }

    struct tm_subdev_cbs subdev_cbs = {0};
    subdev_cbs.subdev_props_get = my_subdev_props_get;
    subdev_cbs.subdev_props_set = my_subdev_props_set;
    subdev_cbs.subdev_service_invoke = my_subdev_service_invoke;
    subdev_cbs.subdev_topo = my_subdev_topo;

    tm_subdev_init(subdev_cbs);

    sub_device_map tmp_sub_device_map;
    // 将指定子设备绑定到当前网关设备
    int reuslt;
    for (size_t i = 0; i < sub_devices_infos.size(); i++)
    {
        tmp_sub_device_map.insert(sub_device_map::value_type(sub_devices_infos[i].DeviceName, sub_devices_infos[i]));
        reuslt = tm_subdev_add((int8_t *)sub_devices_infos[i].ProductID.c_str(), (int8_t *)sub_devices_infos[i].DeviceName.c_str(), (int8_t *)sub_devices_infos[i].SecKey.c_str(), 100);
        printf("tm_subdev_add  reuslt: %d\n", reuslt);
    }

    // 子设备登录平台
    for (size_t i = 0; i < sub_devices_infos.size(); i++)
    {
        reuslt = tm_subdev_login((int8_t *)sub_devices_infos[i].ProductID.c_str(), (int8_t *)sub_devices_infos[i].DeviceName.c_str(), 100);
        printf("tm_subdev_add  reuslt: %d\n", reuslt);
    }

    // 从平台获取当前网关绑定的子设备信息
    reuslt = tm_subdev_topo_get(100);
    printf("tm_subdev_topo_get  reuslt: %d\n", reuslt);

    /*** epoll ***/
    // socket
    int listenfd;
    struct sockaddr_in servaddr;
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == listenfd)
    {
        perror("Create Socket Error:");
        exit(1);
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    inet_pton(AF_INET, IPADDRESS, &servaddr.sin_addr);
    servaddr.sin_port = htons(PORT);
    unsigned int value = 1;
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (void *)&value, sizeof(value));
    if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1)
    {
        perror("Bind Error: ");
        exit(1);
    }
    if (-1 == listen(listenfd, LISTENQ))
    {
        perror("socket listen error: ");
        exit(1);
    }

    // IO多路复用epoll
    int epollfd;
    struct epoll_event events[EPOLLEVENTS];
    int ret;
    char buf[MAXSIZE];
    memset(buf, 0, MAXSIZE);

    // 创建一个描述符
    epollfd = epoll_create(FDSIZE);

    // 添加 eventfd 用以接收到平台应用下发消息打断epoll_wait，发送消息到指定设备
    g_event_fd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (g_event_fd < 0)
    {
        perror("event_fd create fail.");
    }
    struct epoll_event event_eventfd;
    event_eventfd.events = EPOLLIN;
    event_eventfd.data.fd = g_event_fd;
    ret = epoll_ctl(epollfd, EPOLL_CTL_ADD, g_event_fd, &event_eventfd);
    if (ret < 0)
    {
        perror("epoll_ctl failed:");
    }

    // 添加监听描述符事件
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = listenfd;
    ret = epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &ev);
    if (-1 == ret)
    {
        perror("add_event error: ");
    }
    else
    {
        printf("Add_event OK\n");
    }

    signal(SIGINT, sigterm);
    signal(SIGTERM, sigterm);

    int nfds, tmp_fd;
    int bytes = 0;
    g_event_buff[0] = '\0';

    while (g_run)
    {
        if (0 != tm_step(200))
        {
            printf("step failed\n");
            break;
        }

        nfds = epoll_wait(epollfd, events, EPOLLEVENTS, 100);
        for (int i = 0; i < nfds; ++i)
        {
            tmp_fd = events[i].data.fd;

            if ((tmp_fd == listenfd) && (events[i].events & EPOLLIN))
            {
                int clifd;
                struct sockaddr_in cliaddr;
                socklen_t cliaddrlen;
                clifd = accept(listenfd, (struct sockaddr *)&cliaddr, &cliaddrlen);
                if (-1 == clifd)
                {
                    perror("Accept Error: ");
                    continue;
                }
                else
                {
                    printf("Accept a new client:%s:%d\n",
                           inet_ntoa(cliaddr.sin_addr), cliaddr.sin_port);
                    //添加一个客户描述符和事件
                    ev.events = EPOLLIN;
                    ev.data.fd = clifd;
                    int ret = epoll_ctl(epollfd, EPOLL_CTL_ADD, clifd, &ev);
                    if (-1 == ret)
                    {
                        perror("add_event error: ");
                    }
                    else
                    {
                        printf("Add_event OK\n");
                    }
                }
            }
            else if (events[i].events & EPOLLIN)
            {
                bytes = read(tmp_fd, buf, MAXSIZE);
                if (bytes > 0)
                {
                    buf[bytes] = '\0';
                    printf("套接字:%d, 收到字符:%s\n", tmp_fd, buf);
                    if (g_event_buff_size > 0 && strncmp(buf, "event_fd", 8) == 0)
                    {
                        printf("g_event_buff:%s\n", g_event_buff);
                        int ret = write(g_sub_device_fd, g_event_buff, g_event_buff_size);
                        cerr << "g_sub_device_fd:" << g_sub_device_fd << ", write ret:" << ret << endl;
                        g_event_buff_size = 0;
                    }
                    else if (strncmp(buf, "add_sub_devices:", 16) == 0)
                    {
                        printf("add_sub_devices:...\n");
                        if (g_sub_device_map.find(buf + 16) == g_sub_device_map.end() && tmp_sub_device_map.find(buf + 16) != tmp_sub_device_map.end())
                        {
                            printf("g_sub_device_map.insert...\n");
                            tmp_sub_device_map[buf + 16].sub_device_fd = tmp_fd;
                            g_sub_device_map.insert(sub_device_map::value_type(buf + 16, tmp_sub_device_map[buf + 16]));
                            g_sub_device_fd_map.insert(sub_device_fd_map::value_type(tmp_fd, tmp_sub_device_map[buf + 16]));
                        }
                    }
                    else if (strncmp(buf, "sub_devices_post:", 17) == 0)
                    {
                        sub_device_fd_map::iterator it = g_sub_device_fd_map.find(tmp_fd);
                        if (it != g_sub_device_fd_map.end())
                        {
                            printf("sub_devices_post:...\n");
                            int reuslt = tm_subdev_post_data((int8_t *)it->second.ProductID.c_str(), (int8_t *)it->second.DeviceName.c_str(),
                                                             (int8_t *)buf + 17, 0, 100);
                            printf("tm_subdev_post_data  reuslt: %d\n", reuslt);
                        }
                    }
                }
                else
                {
                    printf("套接字:%d,被关闭，注销事件。\n", tmp_fd);
                    ev.data.fd = tmp_fd;
                    ev.events = EPOLLOUT | EPOLLIN | EPOLLERR | EPOLLHUP;
                    if (epoll_ctl(epollfd, EPOLL_CTL_DEL, tmp_fd, &ev) == -1)
                    {
                        perror("对方关闭时，这边注销事件出错..\n");
                        printf("对套接字:%d,注销所有事件失败...\n", tmp_fd);
                        close(tmp_fd);
                        continue;
                    }
                    printf("关闭该套接字..\n");
                    close(tmp_fd);
                }
            }
            else if (events[i].events & EPOLLOUT)
            {
                printf("套接字：%d,有发送事件...现在将该套接字发送事件删除。\n", tmp_fd);
                send(tmp_fd, buf, bytes, 0);
                ev.data.fd = tmp_fd;
                ev.events = EPOLLIN | EPOLLERR | EPOLLHUP;
                if (epoll_ctl(epollfd, EPOLL_CTL_MOD, tmp_fd, &ev) == -1)
                {
                    perror("可以对套接字发送数据时，这边删除发送事件出错..\n");
                    printf("对套接字:%d,注册发送事件失败...\n", tmp_fd);
                    continue;
                }
            }
        }
    }
    tm_logout(3000);

    close(listenfd);
    close(epollfd);
    return 0;
}