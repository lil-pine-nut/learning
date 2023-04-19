#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

#define IPADDRESS "127.0.0.1"
#define PORT 8888
#define MAXSIZE 1024
#define LISTENQ 5
#define FDSIZE 1000
#define EPOLLEVENTS 100

bool g_run = true;

static void sigterm(int sig)
{
    g_run = false;
}

int main(int argc, char *argv[])
{
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

    // SO_REUSEADDR让端口释放后立即就可以被再次使用
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

    while (g_run)
    {
        nfds = epoll_wait(epollfd, events, EPOLLEVENTS, -1);
        printf("nfds=%d\n", nfds);
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
                    int sprintf_len = sprintf(buf, "server的文件描述符:%d-收到字符:%d", tmp_fd, bytes);
                    bytes = write(tmp_fd, buf, sprintf_len);
                    printf("给:%d, 发送了:%s, 发送的字节:%d\n", tmp_fd, buf, bytes);
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

    close(listenfd);
    close(epollfd);
    return 0;
}