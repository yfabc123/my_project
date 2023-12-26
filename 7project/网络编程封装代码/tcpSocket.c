#include "tcpSocket.h"
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
// 连接对端地址 注意成功返回的是0!!直接返回底层函数返回值  
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
//绑定我方地址
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
//设置最大连接队列
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

//设置为MSG_NOSIGNAL  忽略长度为0时的断开信号!
int mySend(int paraTcpSocket, void *buf, size_t bufSize, int flag)
{
    size_t size = send(paraTcpSocket, buf, bufSize, flag);
    if (-1 == size)
    {
        perror("send(paraTcpSocket, buf, bufSize, flag)");
    }
    return size;
}
//设置为MSG_NOSIGNAL  忽略长度为0时的断开信号!

int myRecv(int paraTcpSocket, void *buf, size_t bufSize, int flag)
{
    size_t size = recv(paraTcpSocket, buf, bufSize, flag);
    if (size <= 0)
    {
        perror("recv(paraTcpSocket, buf, bufSize, flag)");
    }
    return size;
}