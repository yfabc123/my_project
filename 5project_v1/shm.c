#include "shm.h"
#include "global.h"
#include "sem.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
// 共享内存存放用户请求信息
/*
此函数实现 信号量集合 环形共享内存等创建
信号量集和:
信号量0:控制唯一访问共享内存 初始值1
信号量1:控制生产者,共享内存满时不能生产 信号量初始值 是 shmMaxCount
信号量2:控制消费者,共享内存空时不能消费 初始值 0
 */
Shm* shm = NULL;
void createShm(int pmaxBlockCount, int pblockSize)
{
    key_t key = ftok(SHMPATH, SHMPROJID);
    if (-1 == key)
    {
        DEBUG_INFO("%s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    //共享内存实际分为两部分 头+数据块区域
    int shmid = shmget(key, sizeof(Shm) + pmaxBlockCount * pblockSize, IPC_CREAT | 0666);
    if (-1 == shmid)
    {
        DEBUG_INFO("%s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    shm = (Shm*)shmat(shmid, NULL, 0); // 映射共享内存
    if (shm == (void*)-1)              //这个返回值是(void*)-1
    {
        DEBUG_INFO("%s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    if (!(shm->maxBlockCount == pmaxBlockCount)) // 说明共享内存没有初始化则进行初始化
    {
        // printf("创建共享内存并赋值\n");
        shm->maxBlockCount = pmaxBlockCount;
        shm->shmHead = 0;
        shm->shmTail = 0;
        shm->blockSize = pblockSize;
        short arr[3] = {1, pmaxBlockCount, 0};
        shm->semSetId = createSem(3, arr);
    }
}
void putShm(Shm* shm, void* data)
{
    printf("客户端准备生产共享内存\n");
    semP(shm->semSetId, 1); // 生产者锁
    semP(shm->semSetId, 0); // 互斥锁
    // 计算偏移量
    int offset = shm->shmTail * shm->blockSize; // 偏移量是以字节为单位!!!
    memcpy((char*)(shm + 1) + offset, (char*)data, shm->blockSize);

    shm->shmTail = (shm->shmTail + 1) % shm->maxBlockCount;
    printf("客户端已经生产共享内存\n");
    semV(shm->semSetId, 2); // 消费者锁
    semV(shm->semSetId, 0); // 放互斥锁
}
void getShm(Shm* shm, void* para)
{
    printf("服务器准备消费共享内存\n");
    semP(shm->semSetId, 2); // 消费者锁
    semP(shm->semSetId, 0); // 互斥锁
    // 计算偏移量
    int offset = shm->shmHead * shm->blockSize;
    //注意:头+数据+数据+数据
    memcpy(para, (char*)(shm + 1) + offset, shm->blockSize);
    shm->shmHead = (shm->shmHead + 1) % shm->maxBlockCount;
    printf("服务器已经消费共享内存\n");
    semV(shm->semSetId, 1); // 通知生产者
    semV(shm->semSetId, 0); // 放互斥锁
}