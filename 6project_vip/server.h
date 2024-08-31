#ifndef SERVER_HEAD_H
#define SERVER_HEAD_H

#include <netinet/in.h>
#include <pthread.h>
#include "global.h"
/*
把数据库信息发送给上线用户的结构体 该包含哪些内容
 */
extern void addTask(Packet *parapacket);
extern void *doAll(void *arg);
extern void *doJoin(void *arg);
extern void *doChat(void *arg);
extern void *doExit(void *arg);
extern void getCurtime(char *paratime);
extern void runServer();
#endif