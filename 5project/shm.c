#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include "shm.h"
#include "sem.h"
// 共享内存存放用户请求信息
/*
此函数实现 信号量集合 环形共享内存等创建
信号量集和:
信号量0:控制唯一访问共享内存 初始值1
信号量1:控制生产者,共享内存满时不能生产 信号量初始值 是 shmMaxCount
信号量2:控制消费者,共享内存空时不能消费 初始值 0
 */
Shm *shm = NULL;
void createShm(int pmaxBlockCount, int pblockSize)
{
    key_t key = ftok(SHMPATH, SHMPROJID);
    if (-1 == key)
    {
        perror("ftok:");
        exit(EXIT_FAILURE);
    }
    int shmid = shmget(key, sizeof(Shm) + pmaxBlockCount * pblockSize, IPC_CREAT | 0666);
    if (-1 == shmid)
    {
        perror("shmget");
        exit(EXIT_FAILURE);
    }
    shm = (Shm *)shmat(shmid, NULL, 0); // 映射共享内存
    if (shm == (void *)-1)
    {
        perror("(Shm *)shmat(shmid, NULL, 0)");
        exit(EXIT_FAILURE);
    }
    if (!(shm->maxBlockCount == pmaxBlockCount)) // 说明共享内存没有初始化则进行初始化
    {
        //     printf("创建共享内存并赋值\n");
        shm->maxBlockCount = pmaxBlockCount;
        shm->curBlockCount = 0;
        shm->shmHead = 0;
        shm->shmTail = 0;
        shm->blockSize = pblockSize;
        short arr[3] = {1, pmaxBlockCount, 0};
        shm->semSetId = createSem(3, arr);
        // shm->shmData = (char *)(shm + 1); // 数据区首地址是偏移一个头部内存大小后的位置
        // printf("首地址:%p\n", shm);
        // printf("数据首地址:%p\n", shm->shmData);
    }
}
void putShm(Shm *shm, void *data)
{
    printf("客户端准备生产共享内存\n");
    semP(shm->semSetId, 1); // 生产者锁
    semP(shm->semSetId, 0); // 互斥锁
    // 计算偏移量
    int offset = shm->shmTail * shm->blockSize; // 偏移量是一个字节!!!
    memcpy((char *)(shm + 1) + offset, (char *)data, shm->blockSize);
    shm->curBlockCount--;
    shm->shmTail = (shm->shmTail + 1) % shm->maxBlockCount;
    printf("客户端已经生产共享内存\n");
    semV(shm->semSetId, 2); // 消费者锁
    semV(shm->semSetId, 0); // 放互斥锁
}
void getShm(Shm *shm, char *para)
{
    printf("addTask准备消费共享内存\n");
    semP(shm->semSetId, 2); // 消费者锁
    semP(shm->semSetId, 0); // 互斥锁
    // 计算偏移量
    int offset = shm->shmHead * shm->blockSize;
    memcpy(para, (char *)(shm + 1) + offset, shm->blockSize);
    shm->curBlockCount++;
    shm->shmHead = (shm->shmHead + 1) % shm->maxBlockCount;
    printf("addTask已经消费共享内存\n");
    semV(shm->semSetId, 1); // 通知生产者
    semV(shm->semSetId, 0); // 放互斥锁
}