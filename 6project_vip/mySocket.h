#ifndef MYSOCKET_HEAD_H
#define MYSOCKET_HEAD_H
#include <sys/socket.h>
#include <netinet/in.h>
// 封装  套接字创建等流程 省的每次重新写
extern int createMySocket();
extern int mySendTo(int pfd, void *pdata, char *pip, short pport, size_t pDataSize, struct sockaddr_in *ppeerAddr);
extern int myBind(int pfd, char *pip, short pport, struct sockaddr_in *addr);
extern int myRecvFrom(int pfd, void *buf, size_t bufSize, struct sockaddr_in *peerAddr);
#endif