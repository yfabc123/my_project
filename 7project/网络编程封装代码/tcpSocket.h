#ifndef TCPSOCKET_HEAD_H
#define TCPSOCKET_HEAD_H
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>


extern int myCreateTcpSocket();
extern int myConnect(int paraTcpSocket, char *pararIp, short paraPort, struct sockaddr_in *paraPeerAddr);
extern int myBind(int paraTcpSocket, char *pararIp, short paraPort,struct sockaddr_in *paraMyAddr);
extern int myListen(int paraTcpSocket, int paraQueueLength);
extern int myAccept(int paraTcpSocket,struct sockaddr_in *paraPeeAddr);
extern int mySend(int paraTcpSocket, void *buf, size_t bufSize, int flag);
extern int myRecv(int paraTcpSocket, void *buf, size_t bufSize, int flag);
#endif