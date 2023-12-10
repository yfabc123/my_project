#include "threadPool.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include "../shm.h"
#include "hashTable.h"
ThreadPool *createTpool(int pthreadCount, int pmaxTaskCount)
{
    ThreadPool *tPool = (ThreadPool *)malloc(sizeof(ThreadPool));
    if (!tPool)
    {
        perror("ThreadPool malloc:\n");
        exit(EXIT_FAILURE);
    }
    tPool->tids = (pthread_t *)malloc(sizeof(pthread_t));
    if (!tPool->tids)
    {
        perror("tPool->tids malloc");
        exit(EXIT_FAILURE);
    }
    tPool->taskQueue = (Task *)malloc(sizeof(Task));
    if (!tPool->taskQueue)
    {
        perror("tPool->taskQueue malloc");
        exit(EXIT_FAILURE);
    }
    tPool->threadCount = pthreadCount;
    tPool->curTaskCount = 0;
    tPool->maxTaskCount = pmaxTaskCount;
    tPool->taskQueueHead = 0;
    tPool->taskQueueTail = 0;
    pthread_mutex_init(&tPool->mutex, NULL);
    pthread_cond_init(&tPool->fullCond, NULL);
    pthread_cond_init(&tPool->emptyCond, NULL);
    for (int i = 0; i < pthreadCount; i++)
    {
        int ret = pthread_create(tPool->tids + i, NULL, threadFunction, tPool);
        if (ret != 0)
        {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
        pthread_detach(tPool->tids[i]);
    }
    return tPool;
}
// 参数存放的是结构体?
void *threadFunction(void *para)
{
    ThreadPool *tPool = (ThreadPool *)para;
    while (1)
    {
       // sleep(1);
        printf("线程池%ld准备消费任务\n", pthread_self());
        pthread_mutex_lock(&tPool->mutex);
        while (tPool->curTaskCount == 0)
        {
            pthread_cond_wait(&tPool->emptyCond, &tPool->mutex); // 等待任务队列不为空的信号
        }
        // 拆解任务结构体获取待执行函数和参数 注意从任务队列尾部获取任务
        void *(*fun)(void *) = (tPool->taskQueue)[tPool->taskQueueHead].function;
        void *funarg = (tPool->taskQueue)[tPool->taskQueueHead].arg;
        tPool->curTaskCount--;
        tPool->taskQueueHead = (tPool->taskQueueHead + 1) % tPool->maxTaskCount;
        fun(funarg);  // 执行函数
        free(funarg); // 注意free参数
        printf("线程池%ld已经消费任务\n", pthread_self());
        pthread_cond_signal(&tPool->fullCond); // 唤醒生产者
        pthread_mutex_unlock(&tPool->mutex);   // 放锁
    }
}
// 对外作为消费者 对内又是生产者 将共享内存数据放到任务结构体中作为参数
void addTask(ThreadPool *ptPool)
{
    char data[shm->blockSize]; // 接收用户数据
    Task task;
    while (1)
    {
      //  sleep(1);
        memset(data, 0, shm->blockSize);
        memset(&task, 0, shm->blockSize);
        getShm(shm, data); // 拿到用户数据 然后装填任务结构体函数+参数
        printf("addTask 准备生产任务\n");
        pthread_mutex_lock(&ptPool->mutex);
        while (ptPool->curTaskCount == ptPool->maxTaskCount)
        {
            pthread_cond_wait(&ptPool->fullCond, &ptPool->mutex);
        }
        // 开始构造任务数据
        task.function = doPublishSubscribe; // 这个变量不用担心覆盖问题所有任务结构体函数指针 都一样
        task.arg = malloc(shm->blockSize);  // 此处每次重新申请空间存放用户数据,消费者线程在参数接收处理后需要把空间释放!
        memcpy(task.arg, data, shm->blockSize);
        (ptPool->taskQueue)[ptPool->taskQueueTail] = task; // 放进任务队列
        ptPool->curTaskCount++;
        ptPool->taskQueueTail = (ptPool->taskQueueTail + 1) % ptPool->maxTaskCount;
        pthread_cond_signal(&ptPool->emptyCond); // 释放队列空信号 通知消费者
        pthread_mutex_unlock(&ptPool->mutex);
        printf("addTask已经生产任务\n");
    }
}
void *doPublishSubscribe(void *arg)
{
    // 根据用户参数判断调用哪个函数处理
    User *user = (User *)arg;
    if (user->action == 0)
    {
        printf("线程准备执行发布逻辑\n");
        doPublish(subHash, user);
    }
    else if (user->action == 1)
    {
        printf("线程准备执行订阅逻辑\n");
        doSubscribe(pubHash, user);
    }
}