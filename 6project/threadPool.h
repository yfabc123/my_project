#ifndef THREADPOOL_HEAD_H
#define THREADPOOL_HEAD_H
#include <pthread.h>
/*
为什么用void *类型任务结构体 实现通用性 这次封装下次直接调用了
 */
typedef struct
{
    void *(*function)(void *);
    void *arg;
} Task;
// 线程池
typedef struct
{
    Task *tasks;           // 任务队列数组
    pthread_t *tids;       // 子线程数组
    int maxTaskCount;      // 最大任务数量
    int curTaskCount;      // 当前任务数量
    int queueHead;         // 队列头
    int queueTail;         // 队列尾
    pthread_mutex_t mutex; // 互斥锁
    pthread_cond_t fullCond;
    pthread_cond_t emptyCond;
} TPool;
extern TPool *tpool;

extern void initTpool(int pmaxTaskCount, int pmaxThreadCount);
extern void *threadTask(void *arg);
#endif