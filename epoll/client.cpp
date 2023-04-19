//客户端也用Epoll实现，控制STDIN_FILENO,STDOUT_FILENO和sockfd三个描述符，程序如下所示:
#include <netinet/in.h>
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
#include <time.h>

#define IPADDRESS "127.0.0.1"
#define SERV_PORT 8888
#define MAXSIZE 1024
#define FDSIZE 1024
#define EPOLLEVENTS 20

//修改事件
static void modify_event(int epollfd, int fd, int state);

int main(int argc, char *argv[])
{
    int sockfd = 0;
    struct sockaddr_in servaddr;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    inet_pton(AF_INET, IPADDRESS, &servaddr.sin_addr);
    int ret = connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr));
    if (-1 == ret)
    {
        perror("connect server error: ");
        exit(1);
    }

    //处理链接
    int write_ret = write(sockfd, "hello ...", strlen("hello ..."));
    int epollfd;
    struct epoll_event events[EPOLLEVENTS];
    char buf[MAXSIZE];
    epollfd = epoll_create(FDSIZE);
    if (-1 == epollfd)
    {
        perror("epoll create error:");
        exit(1);
    }

    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = STDIN_FILENO;
    if (-1 == epoll_ctl(epollfd, EPOLL_CTL_ADD, STDIN_FILENO, &ev))
    {
        perror("add event error: ");
        return 0;
    }
    ev.events = EPOLLIN;
    ev.data.fd = sockfd;
    if (-1 == epoll_ctl(epollfd, EPOLL_CTL_ADD, sockfd, &ev))
    {
        perror("add event error: ");
        return 0;
    }

    printf("add event ok\n");
    int nfds, tmp_fd;
    int bytes;
    for (;;)
    {
        printf("begin epoll_wait\n");
        nfds = epoll_wait(epollfd, events, EPOLLEVENTS, -1);
        for (int i = 0; i < nfds; i++)
        {
            tmp_fd = events[i].data.fd;
            if (events[i].events & EPOLLIN)
            {
                bytes = read(tmp_fd, buf, MAXSIZE);
                if (bytes > 0)
                {
                    buf[bytes] = '\0';
                    if (tmp_fd == STDIN_FILENO)
                    {
                        modify_event(epollfd, sockfd, EPOLLOUT);
                    }
                    else
                    {
                        printf("套接字:%d, 收到字符:%s\n", tmp_fd, buf);
                    }
                }
                else
                {
                    perror("read error: ");
                    close(tmp_fd);
                    exit(1);
                }
            }
            else if (events[i].events & EPOLLOUT)
            {
                int nwrite = 0;
                nwrite = write(tmp_fd, buf, strlen(buf));
                if (-1 == nwrite)
                {
                    perror("write error:");
                    close(tmp_fd);
                }
                else
                {
                    if (tmp_fd == STDOUT_FILENO)
                    {
                        modify_event(epollfd, tmp_fd, EPOLLIN);
                    }
                    else
                    {
                        modify_event(epollfd, tmp_fd, EPOLLIN);
                    }
                }
                memset(buf, 0, MAXSIZE);
            }
        }
    }
    close(epollfd);
    close(sockfd);
    return 0;
}

//修改事件
static void modify_event(int epollfd, int fd, int state)
{
    // printf("Begin to modify event.\n");
    struct epoll_event ev;
    ev.events = state;
    ev.data.fd = fd;
    if (-1 == epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &ev))
    {
        perror("modify event error: ");
        return;
    }
    // printf("modify event ok\n");
    return;
}