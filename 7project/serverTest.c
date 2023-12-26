#include "tcpSocket.h"
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <unistd.h>
#define SERVERIP   "192.168.2.4"
#define SERVERPORT 1
#define MAXCONNECT 3
int main(int argc, char const *argv[])
{
    int mainSocket = myCreateTcpSocket();
    setNonblocking(mainSocket);
    int len; // 保存长度信息
    myBind(mainSocket, SERVERIP, SERVERPORT, NULL);
    myListen(mainSocket, MAXCONNECT);
    struct sockaddr_in peerAddr;
    int ret = 0;
    intptr_t newSfd;
    size_t size;
    char buf[1024] = {0};
    int epfd = epoll_create(1); // 创建epoll实例
    struct epoll_event event;
    event.events = EPOLLIN | EPOLLET; // 默认是水平触发
    event.data.fd = mainSocket;
    epoll_ctl(epfd, EPOLL_CTL_ADD, mainSocket, &event);
    struct epoll_event readSockets[MAXCONNECT + 1]; // 最大连接数+1  因为还有个主套接字
    // memset(readSockets, 0, sizeof(readSockets));
    int waitRet;
    /*
    监听思路:
    epoll实现监听的大概逻辑就是首先把主线程的套接字(用户和客户端交互的套接字),加入监听队列
    后续如果检测到那么说明有新客户连接 则需要调用 accept获取到与客户交互的套接字同时把这个
    套接字放入监听队列 然后继续监听  也就是如果检测到主套接字变化说明有人建立连接 如果检测到
    非主套接字变化,说明客户端发来了交互信息 则需要处理  也就是两套处理逻辑
     */
    printf("主套接字:%d\n", mainSocket);
    while (1)
    {
        fprintf(stderr, "等待连接\n");
        waitRet = epoll_wait(epfd, readSockets, MAXCONNECT, -1); //
        if (waitRet < 0)
        {
            perror("waitRet = epoll_wait(epfd, readSockets, 1000, 0);");
            exit(EXIT_FAILURE);
        }
        else if (waitRet > 0) // 当检测到可读时 可能是主套接字也可能是子套接字 两种处理逻辑
        {
            printf("waitRet:%d\n", waitRet);
            for (int i = 0; i < waitRet; i++)
            {
                if (readSockets[i].data.fd == mainSocket) // 优先级 ==高于&&高于&
                {
                    fprintf(stderr, "有客户端连接\n");
                    newSfd = myAccept(mainSocket, &peerAddr);
                    setNonblocking(newSfd);
                    event.data.fd = newSfd;
                    event.events = EPOLLIN | EPOLLET; // 默认是水平触发
                    epoll_ctl(epfd, EPOLL_CTL_ADD, newSfd, &event);
                    continue; // 剩下不用执行 继续
                }
                else if ((readSockets[i].events & EPOLLIN))
                {
                    fprintf(stderr, "检测到可读\n");
                    size = myRecv(readSockets[i].data.fd, buf, sizeof(buf), MSG_NOSIGNAL);
                    printf("数据长度:%ld\n", size);
                    if (size == 0) // 客户端断开连接了
                    {
                        fprintf(stderr, "客户端断开\n");
                        close(readSockets[i].data.fd); // 关闭当前套接字
                        continue;
                    }
                    else if (size == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) // 表示无数据可读并不是出问题
                    {
                        fprintf(stderr, "客户端没发送数据继续\n");
                        continue;
                    }
                    else
                    {
                        printf("%s\n", strerror(errno));
                    }
                    fprintf(stderr, "收到的内容:%s\n", buf);
                    size = mySend(readSockets[i].data.fd, buf, size, MSG_NOSIGNAL);
                    if (size == 0) // 客户端断开连接了
                    {
                        fprintf(stderr, "客户端下线\n");
                        close(readSockets[i].data.fd); // 关闭当前套接字
                    }
                    else if (size == -1 && (errno == EAGAIN || errno == EWOULDBLOCK)) // 表示无数据可发送
                    {
                        fprintf(stderr, "\n");
                    }
                    else
                    {
                        printf("%s\n", strerror(errno));
                    }
                    continue;
                }
            }
        }
        fprintf(stderr, "----------------------------------------------------\n");
        // sleep(1);
    }
    return 0;
}
