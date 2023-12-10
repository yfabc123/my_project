#ifndef THREADPOOL_HEAD_H
#define THREADPOOL_HEAD_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
// 线程池+任务队列 控制结构体
typedef struct
{
    void *(*function)(void *);
    void *arg;
} Task;
typedef struct
{
    pthread_t *tids;          // 线程池id集合
    int threadCount;          // 线程池个数
    Task *taskQueue;          // 任务队列数组
    int curTaskCount;         // 当前任务数
    int maxTaskCount;         // 任务队列最大值
    int taskQueueHead;        // 队列头
    int taskQueueTail;        // 队列尾
    pthread_mutex_t mutex;    // 互斥锁
    pthread_cond_t fullCond;  // 队列空条件变量
    pthread_cond_t emptyCond; // 队列满条件变量
} ThreadPool;
extern ThreadPool *createTpool(int pthreadCount, int pmaxTaskCount);
extern void *threadFunction(void *para);
extern void addTask(ThreadPool *ptPool);
extern void *doPublishSubscribe(void *arg);
#endif