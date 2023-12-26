#include "tcpSocket.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#define DEBUG_TCP_SOCKET_INFO(arg) \
    fprintf(stderr, "[%s,%s,%d]:%s\n", __FILE__, __FUNCTION__, __LINE__, arg)
int myCreateTcpSocket()
{
    int sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (-1 == sfd)
    {
        perror("socket(AF_INET, SOCK_STREAM, 0);");
        return -1;
    }
    return sfd;
}
// 连接对端地址 注意成功返回的是0!!直接返回底层函数返回值  第四个参数不是输出参数!!!是可选参数
int myConnect(int paraTcpSocket, char *pararIp, short paraPort, struct sockaddr_in *paraPeerAddr)
{
    int ret; // 默认连接失败
    if (pararIp && !paraPeerAddr)
    {
        struct sockaddr_in peerAddr;
        struct in_addr inAddr;
        inet_aton(pararIp, &inAddr);
        peerAddr.sin_family = AF_INET;
        peerAddr.sin_addr = inAddr;
        peerAddr.sin_port = htons(paraPort);
        ret = connect(paraTcpSocket, (const struct sockaddr *)&peerAddr, sizeof(struct sockaddr));
    }
    else if (!pararIp && paraPeerAddr)
    {
        ret = connect(paraTcpSocket, (const struct sockaddr *)paraPeerAddr, sizeof(struct sockaddr));
    }
    else
    {
        ret = -1;
    }
    if (ret == -1)
    {
        perror("bind");
    }
    return ret;
}
// 绑定我方地址
int myBind(int paraTcpSocket, char *pararIp, short paraPort, struct sockaddr_in *paraMyAddr)
{
    int ret; // 默认连接失败
    if (pararIp && !paraMyAddr)
    {
        struct sockaddr_in myAddr;
        struct in_addr inAddr;
        inet_aton(pararIp, &inAddr);
        myAddr.sin_family = AF_INET;
        myAddr.sin_addr = inAddr;
        myAddr.sin_port = htons(paraPort);
        ret = bind(paraTcpSocket, (struct sockaddr *)&myAddr, sizeof(struct sockaddr));
    }
    else if (!pararIp && paraMyAddr)
    {
        ret = bind(paraTcpSocket, (struct sockaddr *)paraMyAddr, sizeof(struct sockaddr));
    }
    else
    {
        ret = -1;
    }
    if (ret == -1)
    {
        perror("bind");
    }
    return ret;
}
// 设置最大连接队列
int myListen(int paraTcpSocket, int paraQueueLength)
{
    if (listen(paraTcpSocket, paraQueueLength) == -1)
    {
        perror("listen(paraTcpSocket, paraQueueLength)");
        return -1;
    }
    return 0;
}
// paraPeerSockaddr 输出参数保存对端地址
int myAccept(int paraTcpSocket, struct sockaddr_in *paraPeeAddr)
{
    socklen_t len = sizeof(struct sockaddr);
    int newSocket = accept(paraTcpSocket, (struct sockaddr *)paraPeeAddr, &len);
    if (newSocket == -1)
    {
        perror("accept(paraTcpSocket, paraPeeAddr, &len)");
    }
    return newSocket;
}
// 设置为MSG_NOSIGNAL  忽略长度为0时的断开信号!
int mySend(int paraTcpSocket, void *buf, size_t bufSize, int flag)
{
    size_t size = send(paraTcpSocket, buf, bufSize, flag);
    if (-1 == size)
    {
        perror("send(paraTcpSocket, buf, bufSize, flag)");
        // return -1;
    }
    return size;
}
// 设置为MSG_NOSIGNAL  忽略长度为0时的断开信号!
int myRecv(int paraTcpSocket, void *buf, size_t bufSize, int flag)
{
    size_t size = recv(paraTcpSocket, buf, bufSize, flag);
    if (size <= 0)
    {
        perror("recv(paraTcpSocket, buf, bufSize, flag)");
        // return -1;
    }
    return size;
}
/*
设置为非阻塞(多路复用一般结合非阻塞io实现并发) */
int setNonblocking(int paraTcpSocket)
{
    int flags = fcntl(paraTcpSocket, F_GETFL, 0);
    if (-1 == flags)
    {
        perror("fcntl(paraTcpSocket, F_GETFL, 0)");
        return -1;
    }
    if (fcntl(paraTcpSocket, F_SETFL, flags | O_NONBLOCK) == -1)
    {
        perror("fcntl(paraTcpSocket, F_SETFL, flags | O_NONBLOCK)");
        return -1;
    }
    return 1;
}
/* 升级版 myRecv  采用定长接收方式 我丢!坑了我一天 本来采用数据长度+数据结果数据长度 粘包了,只能使用定长接收
参数1:tcp
参数2:接收数据的地址
参数3:接收数据的长度
参数4:recv的flag
返回值:
-2:其他失败情况 直接挂掉
-1:本次读取失败,下次继续
0:对端关闭连接 直接返回
1:正确接收数据
 */
int myUpgradeRecv(int paraTcpSocket, void *paraData, size_t paraMaxByte, int paraFlag)
{
    size_t curByte = 0; // 起始字节
    size_t rByte = 0;   // 记录每次接收的字节
    while (curByte < paraMaxByte)
    {
        rByte = myRecv(paraTcpSocket, (char *)paraData + curByte, paraMaxByte - curByte, paraFlag);
        if (rByte == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
        {
            DEBUG_TCP_SOCKET_INFO("非阻塞模式下EAGAIN或者EWOULDBLOCK下次继续接收");
            return -1;
        }
        else if (rByte == 0)
        {
            DEBUG_TCP_SOCKET_INFO("对端关闭连接");
            return 0;
        }
        else if (rByte == -1) // 其他错误情况
        {
            DEBUG_TCP_SOCKET_INFO(strerror(errno));
            return -2;
        }
        curByte += rByte;
    }
    return 1;
}