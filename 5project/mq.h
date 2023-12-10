#ifndef MQ_HEAD_H
#define MQ_HEAD_H
#include <pthread.h>
#define MQPATH "../"
#define MQPROJID 3
typedef struct
{
    long msgType;
    char msgData[64];
} Msg;
extern int mqId; // 全局消息队列 客户端服务器共享
extern void createMq();
extern void mqSend(int mqId, char *pmsgData, long pmsgType);
extern void mqRecv(int mqId, long pmsgType);
#endif
