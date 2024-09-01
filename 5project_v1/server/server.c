#include "server.h"
#include "../global.h"
#include "../mq.h"
#include "../shm.h"
#include "hashTable.h"
#include "threadPool.h"

void runServer()
{
    createShm(10, sizeof(User));
    ThreadPool* pool = creatThreadPool(MIN_TASK_COUNT, MAX_TASK_COUNT);
    createMq();
    createPubHash();
    createSubHash();
    while (1)
    {
        User* data = (User*)malloc(sizeof(User)); // 接收用户数据
        getShm(shm, data);
        threadPoolAdd(pool, doPublishSubscribe, data);
    }
}

void* doPublishSubscribe(void* arg)
{
    // 根据用户参数判断调用哪个函数处理
    User* user = (User*)arg;
    if (user->action == 0)
        doPublish(user);
    else if (user->action == 1)
        doSubscribe(user);
    free(user);
    return NULL;
}