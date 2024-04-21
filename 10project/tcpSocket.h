#ifndef TCPSOCKET_HEAD_H
#define TCPSOCKET_HEAD_H
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/socket.h>
extern int myCreateTcpSocket();
extern int myConnect(int paraTcpSocket, char *pararIp, short paraPort, struct sockaddr_in *paraPeerAddr);
extern int myBind(int paraTcpSocket, char *pararIp, short paraPort, struct sockaddr_in *paraMyAddr);
extern int myListen(int paraTcpSocket, int paraQueueLength);
extern int myAccept(int paraTcpSocket, struct sockaddr_in *paraPeeAddr);
extern int mySend(int paraTcpSocket, void *buf, size_t bufSize, int flag);
extern int myRecv(int paraTcpSocket, void *buf, size_t bufSize, int flag);
extern int setNonblocking(int paraTcpSocket);
extern int myUpgradeRecv(int paraTcpSocket, void *paraData, size_t paraMaxByte, int paraFlag);

#endif