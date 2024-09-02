#include "threadPool.h"
#include <bits/pthreadtypes.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* 判断线程是否存在*/
int isAlive(pthread_t thread)
{
    // 尝试向线程发送信号 0
    int result = pthread_kill(thread, 0);
    // 判断返回值
    if (result == ESRCH)
        return 0;
    return 1;
}
ThreadPool* creatThreadPool(int minThreadCount_, int maxThreadCount_)
{
    ThreadPool* pool = (ThreadPool*)malloc(sizeof(ThreadPool));
    if (!pool)
    {
        DEBUG_INFO("%s", strerror(errno));
        return NULL;
    }
    pool->taskQueue = (ThreadpoolTask*)malloc(sizeof(ThreadpoolTask) * maxThreadCount_);

    pool->minThreadCount = minThreadCount_;
    pool->maxThreadCount = maxThreadCount_;
    pool->busyThreadCount = 0;
    pool->liveThreadCount = minThreadCount_;
    pool->waitExitThreadCount = 0;

    pool->queueHead = 0;
    pool->queueTail = 0;
    pool->curTaskCount = 0;
    pool->maxTaskCount = maxThreadCount_;
    pool->shutDown = 0;

    pool->tids = (pthread_t*)calloc(maxThreadCount_, sizeof(pthread_t)); //全部置为0
    if (!pool->tids)
    {
        free(pool);
        DEBUG_INFO("%s", strerror(errno));
        return NULL;
    }
    if (pthread_mutex_init(&pool->mutex, NULL) || pthread_mutex_init(&pool->busyThreadMutex, NULL) ||
        pthread_cond_init(&pool->notEmptyCond, NULL) || pthread_cond_init(&pool->notFullCond, NULL))
    {
        free(pool);
        free(pool->tids);
        DEBUG_INFO("%s", strerror(errno));
        return NULL;
    }
    for (int i = 0; i < minThreadCount_; i++)
    {
        pthread_create(&(pool->tids)[i], NULL, workFun, (void*)pool);
        pthread_detach(pool->tids[i]);
    }
    pthread_create(&pool->managerTid, NULL, managerFun, (void*)pool);
    pthread_detach(pool->managerTid);
    return pool;
}

void* workFun(void* arg)
{
    ThreadPool* pool = (ThreadPool*)arg;
    while (1)
    {
        usleep(2000000); // 1秒:1000000 微秒  0.7秒: 700000
        pthread_mutex_lock(&pool->mutex);
        while (pool->curTaskCount == 0 && !pool->shutDown)
        {
            pthread_cond_wait(&pool->notEmptyCond, &pool->mutex);

            /* 这个逻辑实际就是哪怕有任务 当前线程也不管了直接退出交给后面线程处理
            并且必须保证线程至少是min个*/
            if (pool->waitExitThreadCount > 0 && pool->liveThreadCount > pool->minThreadCount)
            {
                pool->liveThreadCount--;
                pool->waitExitThreadCount--;
                printf("负载较低当前工作线程%ld自行销毁,剩余线程:%d\n", pthread_self(), pool->liveThreadCount);
                pthread_mutex_unlock(&pool->mutex);
                pthread_exit(NULL);
            }
        }
        /*如果收到结束命令则直接关闭*/
        if (pool->shutDown == 1)
        {
            pthread_mutex_unlock(&pool->mutex);
            pthread_exit(NULL);
        }

        //取任务结构体中的函数指针和参数
        void* (*fun)(void*) = (pool->taskQueue)[pool->queueHead].function;
        void* arg = (pool->taskQueue)[pool->queueHead].arg;

        //先更新任务数量
        pool->queueHead = (pool->queueHead + 1) % pool->maxTaskCount;
        pool->curTaskCount--;
        // printf("剩余任务数量:%d\n", pool->curTaskCount);

        //先释放锁在执行函数效率更高  ,放锁+唤醒生产者
        pthread_mutex_unlock(&pool->mutex);
        pthread_cond_signal(&pool->notFullCond);

        //忙线程+1
        pthread_mutex_lock(&pool->busyThreadMutex);
        pool->busyThreadCount++;
        pthread_mutex_unlock(&pool->busyThreadMutex);

        fun(arg);

        //执行完之后忙碌线程减少
        // printf("子线程%ld已执行任务\n", pthread_self());
        pthread_mutex_lock(&pool->busyThreadMutex);
        pool->busyThreadCount--;
        pthread_mutex_unlock(&pool->busyThreadMutex);
    }
    return NULL;
}

void* managerFun(void* arg)
{
    ThreadPool* pool = (ThreadPool*)arg;
    while (!pool->shutDown)
    {

        usleep(1000000); //定时检查线程池状态 1秒:1000000 微秒  0.7秒: 700000
        // printf("管理员线程%ld准备抢锁\n", pthread_self());
        //管理员线程获取 当前任务数,忙碌线程数,存活线程数 判断是需要增加还是删除线程?
        pthread_mutex_lock(&pool->mutex);
        int taskNum = pool->curTaskCount;
        int allThread = pool->liveThreadCount;
        pthread_mutex_unlock(&pool->mutex);

        pthread_mutex_lock(&pool->busyThreadMutex);
        int busyThread = pool->busyThreadCount;
        pthread_mutex_unlock(&pool->busyThreadMutex);
        printf("------管理员线程统计:当前存活线程数:%d,当前忙碌线程:%d,待处理任务:%d--------------\n", allThread, pool->busyThreadCount, taskNum);

        //启新线程条件:当待处理任务超过3个并且总线程比最大线程少的时候进行扩容
        if (taskNum >= WAIT_TASK_COUNT && pool->maxThreadCount > allThread)
        {
            //遍历线程id数组 找出空缺位置存放本次创建的线程id
            int needThreadNum = MIN(WAIT_TASK_COUNT, pool->maxThreadCount - allThread);
            pthread_mutex_lock(&pool->mutex);

            for (int i = 0; i < pool->maxThreadCount && needThreadNum; i++)
            {
                if (!pool->tids[i] || !isAlive(pool->tids[i]))
                {
                    DEBUG_INFO("负载较高 待处理任务超过WAIT_TASK_COUNT需要创建线程");
                    pthread_create(&pool->tids[i], NULL, workFun, (void*)pool);
                    pthread_detach(pool->tids[i]);
                    needThreadNum--;
                    pool->liveThreadCount++;
                }
            }
            pthread_mutex_unlock(&pool->mutex);
        }

        //关闭线程条件: 存活线程超过忙碌线程两倍还多 但是关闭的线程至少比最少线程要多
        if (busyThread * 2 <= allThread && allThread > MIN_TASK_COUNT)
        {
            // DEBUG_INFO("存活线程超过忙碌线程两倍需要销毁线程");
            pthread_mutex_lock(&pool->mutex);
            pool->waitExitThreadCount = MIN(CHANAGE_COUNT, allThread - MIN_TASK_COUNT);
            pthread_mutex_unlock(&pool->mutex);

            //为什么有这个操作?因为当前有很多线程都是阻塞的并且没有任务需要处理 将其唤醒自行销毁
            for (int i = 0; i < pool->waitExitThreadCount; i++)
                pthread_cond_signal(&pool->notEmptyCond);
        }
    }
    return NULL;
}

int threadPoolFree(ThreadPool* pool)
{
    if (!pool)
    {
        DEBUG_INFO("线程池为空无法销毁");
        return -1;
    }
    free(pool->taskQueue);
    free(pool->tids);

    //待确认 除非能保证所有线程已经销毁了否则不能执行下面操作
    pthread_mutex_unlock(&(pool->mutex));
    pthread_mutex_destroy(&(pool->mutex));
    pthread_mutex_unlock(&(pool->busyThreadMutex));
    pthread_mutex_destroy(&(pool->busyThreadMutex));
    pthread_cond_destroy(&(pool->notEmptyCond));
    pthread_cond_destroy(&(pool->notFullCond));
    free(pool);
    pool = NULL;
    return 1;
}

/* 往任务队列添加任务函数 1表示成功*/
int threadPoolAdd(ThreadPool* pool, void* function(void* arg), void* arg)
{
    pthread_mutex_lock(&(pool->mutex));
    while (pool->curTaskCount == pool->maxTaskCount && !pool->shutDown)
        pthread_cond_wait(&pool->notFullCond, &pool->mutex);

    //判断是否需要销毁线程池
    if (!pool->shutDown)
    {
        //填充任务并唤醒消费线程
        pool->taskQueue[pool->queueTail].function = function;
        pool->taskQueue[pool->queueTail].arg = arg;
        pool->queueTail = (pool->queueTail + 1) % pool->maxTaskCount;
        pool->curTaskCount++;
        pthread_mutex_unlock(&(pool->mutex));
        pthread_cond_signal(&(pool->notEmptyCond));
    }
    else
        pthread_mutex_unlock(&(pool->mutex));

    return 1;
}

/*回收所有线程*/
void threadPoolDestroy(ThreadPool* pool)
{
    pool->shutDown = 1;
    //由于创建线程全部设置为分离模式 所以回收所有线程时检查是否在执行的线程
#if 0
    //回收管理员线程
    pthread_join(pool->managerTid, NULL);
    //回收其余线程  
    for (int i = 0; i < pool->maxThreadCount; i++)
    {
        if (pool->tids[i] || isAlive(pool->tids[i]))
        {
            pthread_join(pool->tids[i], NULL);
            pool->tids[i] = 0;
        }
    }
#endif
    //等所有线程处理完后结束
    while (pool->busyThreadCount != 0 || pool->curTaskCount != 0)
    {
        sleep(3);
    }
    threadPoolFree(pool);
}
