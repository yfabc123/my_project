#ifndef _GLOBAL_H
#define _GLOBAL_H
#include <pthread.h>
#define DEBUG_INFO(arg, ...) \
    fprintf(stderr, "[%s,%s,%d]:" arg "\n", __FILE__, __FUNCTION__, __LINE__, ##__VA_ARGS__)

//共享内存 数据块
typedef struct
{
    int action;       // 0发布 1订阅
    char topic[64];   // 主题
    char content[64]; // 内容
    pthread_t pid;    // 后面作为消息队列的消息类型
} User;

//共享内数据区域的头信息
typedef struct
{
    int maxBlockCount; // 共享内存数据块数量
    int shmHead;       // 队列头
    int shmTail;       // 队列尾
    int semSetId;      // 信号量 保护并发安全性
    int blockSize;     // 单个数据块大小
} Shm;

#endif
