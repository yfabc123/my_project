#ifndef THREADPOOL_HEAD_H
#define THREADPOOL_HEAD_H
#include <pthread.h>

#define WAIT_TASK_COUNT 3   //待处理任务(意味消费线程不够了)
#define MAX_TASK_COUNT  100 //最大线程数量
#define MIN_TASK_COUNT  1   //最小(初始化线程数)
#define CHANAGE_COUNT   2   //每次销毁创建线程个数

#define MIN(A, B) ((A) < (B) ? (A) : (B))
#ifndef DEBUG_INFO
#define DEBUG_INFO(arg) \
    fprintf(stderr, "[%s,%s,%d]:%s\n", __FILE__, __FUNCTION__, __LINE__, arg)
#endif

//线程池任务结构体
typedef struct
{
    void* (*function)(void*);
    void* arg;
} ThreadpoolTask;

// 线程池
typedef struct
{
    pthread_mutex_t mutex;           //整个结构体锁
    pthread_mutex_t busyThreadMutex; //更新忙线程数量锁,和整体锁分离管理效率更高
    pthread_cond_t notFullCond;      //队列满信号
    pthread_cond_t notEmptyCond;     //队列空信号

    pthread_t* tids;           //子线程id数组
    pthread_t managerTid;      //管理者线程
    ThreadpoolTask* taskQueue; //任务队列数组

    int minThreadCount;      //最小线程数(起始线程数)
    int maxThreadCount;      //最大任务数
    int busyThreadCount;     //忙碌线程数
    int liveThreadCount;     //当前线程
    int waitExitThreadCount; //要销毁的线程数

    int queueHead;    // 队列头
    int queueTail;    // 队列尾
    int curTaskCount; // 当前任务数量
    int maxTaskCount; // 最大任务数:(和最大线程池保持一致)

    int shutDown; //判断是否关闭线程池

} ThreadPool;
extern void* workFun(void* arg);                                                  //工作线程
extern void* managerFun(void* arg);                                               //管理员线程
extern ThreadPool* creatThreadPool(int minThreadCount_, int maxThreadCount_);     //线程池初始化
extern int isAlive(pthread_t thread);                                             //探测指定线程是否存活
extern int threadPoolAdd(ThreadPool* pool, void* function(void* arg), void* arg); //创建任务
extern void threadPoolDestroy(ThreadPool* pool);                                  //等待所有子线程销毁
extern int threadPoolFree(ThreadPool* pool);                                      //销毁线程池

#endif