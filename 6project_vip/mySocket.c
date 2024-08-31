#include "mySocket.h"
#include <sys/socket.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "global.h"
int createMySocket()
{
    int socketFd = socket(AF_INET, SOCK_DGRAM, 0);
    if (socketFd < 0)
    {
        perror("int socketFd = socket(AF_INET, SOCK_DGRAM, 0);");
        return -1;
    }
    return socketFd;
}
int mySendTo(int pfd, void *pdata, char *pip, short pport, size_t pDataSize, struct sockaddr_in *ppeerAddr)
{
    size_t sendByte;
    if (pip && !ppeerAddr)
    {
        struct in_addr peerIp;
        int ret = inet_aton(pip, &peerIp); // 主机ip->网络ip
        if (ret != 1)
        {
            perror("int ret= inet_aton(ip,&peerIp);");
            return -1;
        }
        /* 填充 发送地址 ip+port */
        struct sockaddr_in peerAddr;
        peerAddr.sin_family = AF_INET;
        peerAddr.sin_port = htons(pport); // 主机port->网络port
        peerAddr.sin_addr = peerIp;
        sendByte = sendto(pfd, pdata, pDataSize, 0, (const struct sockaddr *)&peerAddr, sizeof(peerAddr));
    }
    else if (!pip && ppeerAddr)
    {
        Packet *packet = (Packet *)pdata;
        printf("服务器发送:姓名:%s 内容:%s 消息类型:%d\n", packet->user_name, packet->message, packet->type);
        sendByte = sendto(pfd, pdata, pDataSize, 0, (const struct sockaddr *)ppeerAddr, sizeof(struct sockaddr_in));
    }
    else
    {
        return -1;
    }
    return sendByte;
}
int myBind(int pfd, char *pip, short pport, struct sockaddr_in *paddr)
{
    // 两种绑定方式 看传的什么参数 地址结构体还是ip+port
    if (pip && !paddr)
    {
        struct sockaddr_in myAddr;
        struct in_addr myIp;
        inet_aton(pip, &myIp);
        myAddr.sin_family = AF_INET;
        myAddr.sin_port = htons(pport);
        myAddr.sin_addr = myIp;
        if (bind(pfd, (const struct sockaddr *)&myAddr, sizeof(myAddr)) == -1)
        {
            perror("bind(pfd, (const struct sockaddr *)&myAddr, sizeof(myAddr))");
            return -1;
        }
    }
    else if (paddr)
    {
        if (bind(pfd, (const struct sockaddr *)paddr, sizeof(struct sockaddr)) == -1)
        {
            printf("debug\n");
            perror("bind(pfd, (const struct sockaddr *)&paddr, sizeof(struct sockaddr))");
            return -1;
        }
    }
    else
    {
        return -1;
    }
    return 1;
}
// 输出参数 peerAddr 保存对端地址信息
int myRecvFrom(int pfd, void *buf, size_t bufSize, struct sockaddr_in *peerAddr)
{
    socklen_t len = sizeof(struct sockaddr);
    ssize_t recvByte = recvfrom(pfd, buf, bufSize, 0, (struct sockaddr *)peerAddr, &len);
    return recvByte;
}