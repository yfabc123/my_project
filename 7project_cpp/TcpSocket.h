#ifndef TcpSocket_HEAD_H
#define TcpSocket_HEAD_H
// 基本是操作系统api 就不改写了
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
class TcpSocket
{
public:
    int myCreateTcpSocket();
    int myConnect(int paraTcpSocket, const char *pararIp, short paraPort, struct sockaddr_in *paraPeerAddr);
    int myBind(int paraTcpSocket, const char *pararIp, short paraPort, struct sockaddr_in *paraMyAddr);
    int myListen(int paraTcpSocket, int paraQueueLength);
    int myAccept(int paraTcpSocket, struct sockaddr_in *paraPeeAddr);
    int mySend(int paraTcpSocket, void *buf, size_t bufSize, int flag);
    int myRecv(int paraTcpSocket, void *buf, size_t bufSize, int flag);
    int setNonblocking(int paraTcpSocket);
    int myUpgradeRecv(int paraTcpSocket, void *paraData, size_t paraMaxByte, int paraFlag);
};

#endif