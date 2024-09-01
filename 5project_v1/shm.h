#ifndef SHM_HEAD_H
#define SHM_HEAD_H
#define SHMPATH   "../"
#define SHMPROJID 1
#include <pthread.h>
#include <stdio.h>

#include "global.h"


extern Shm* shm; // 注意定义....
extern void createShm(int pshmMaxCount, int pblockSize);
extern void putShm(Shm* shm, void* data);
extern void getShm(Shm* shm, void* para);
#endif