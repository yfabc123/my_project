#ifndef SHM_HEAD_H
#define SHM_HEAD_H
#define SHMPATH "../"
#define SHMPROJID 1
#include <stdio.h>
#include <pthread.h>
typedef struct
{
    int maxBlockCount; // 共享内存最大数量
    int curBlockCount; // 当前使用内存块数量
    int shmHead;       // 队列头
    int shmTail;       // 队列尾
    int semSetId;      // 信号量 保护并发安全性
    int blockSize;     // 块大小
   // char *shmData;     // 指向数据区
} Shm;


extern Shm *shm; // 注意定义....
extern void createShm(int pshmMaxCount, int pblockSize);
extern void putShm(Shm *shm, void *data);
extern void getShm(Shm *shm, char *para);
#endif