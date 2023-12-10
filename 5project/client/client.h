#ifndef CLIENT_HEAD_H
#define CLIENT_HEAD_H
#include <pthread.h>
typedef struct
{
    int action;       // 0发布 1订阅
    char topic[64];   // 主题
    char content[64]; // 内容
    pthread_t pid;   // 后面作为消息队列的消息类型
} User;

extern void clientRun();
extern void publish();
extern void subscribe();
#endif