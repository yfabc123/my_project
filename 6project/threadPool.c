#include "threadPool.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
TPool *tpool = NULL;
void *threadTask(void *arg)
{
    TPool *tpool = (TPool *)arg;
    while (1)
    {
        printf("子线程%ld准备获取任务\n", pthread_self());
        pthread_mutex_lock(&tpool->mutex);
        while (tpool->curTaskCount == 0)
        {
            pthread_cond_wait(&tpool->emptyCond, &tpool->mutex);
        }
        Task task = (tpool->tasks)[tpool->queueHead];
        void *(*fun)(void *) = task.function;
        void *arg1 = task.arg;
        printf("子线程准备执行函数\n");
        tpool->curTaskCount--;
        tpool->queueHead = (tpool->queueHead + 1) % tpool->maxTaskCount;
        printf("子线程%ld已执行任务\n", pthread_self());
        pthread_mutex_unlock(&tpool->mutex);
        pthread_cond_signal(&tpool->fullCond);
        //先释放锁在执行函数效率更高
        fun(arg1);
    }
}
void initTpool(int pmaxTaskCount, int pmaxThreadCount)
{
    tpool = (TPool *)malloc(sizeof(TPool));
    int ret = 0;
    if (!tpool)
    {
        perror("tpool=(TPool*)malloc(sizeof(TPool));");
        return;
    }
    tpool->tasks = (Task *)malloc(sizeof(Task) * pmaxTaskCount);
    if (!tpool->tasks)
    {
        perror("tpool->tasks = (Task *)malloc(sizeof(Task) * pmaxTaskCount);");
        return;
    }
    tpool->tids = (pthread_t *)malloc(sizeof(pthread_t) * pmaxThreadCount);
    for (int i = 0; i < pmaxThreadCount; i++)
    {
        ret = pthread_create(&(tpool->tids)[i], NULL, threadTask, tpool);
        if (ret != 0)
        {
            perror("ret = pthread_create(&(tpool->tids)[i], NULL, threadJob, tpool);");
            return;
        }
    }
    tpool->maxTaskCount = pmaxTaskCount;
    tpool->curTaskCount = 0;
    tpool->queueHead = 0;
    tpool->queueTail = 0;
    pthread_mutex_init(&tpool->mutex, NULL);
    pthread_cond_init(&tpool->fullCond, NULL);
    pthread_cond_init(&tpool->emptyCond, NULL);
    printf("线程池初始化完成\n");
}
