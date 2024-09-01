#include "sem.h"
#include "global.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/types.h>
/*
信号量0:控制唯一访问共享内存
信号量1:控制生产者,共享内存满时不能生产
信号量2:控制消费者,共享内存空时不能消费
 */
int createSem(int psemCount, short* parray)
{
    key_t key = ftok(SEMPATH, SEMPROID);
    if (key == -1)
    {
        DEBUG_INFO("%s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    
    int semSetId = semget(key, psemCount, IPC_CREAT | 0666);
    if (semSetId == -1)
    {
        DEBUG_INFO("%s", strerror(errno));
        exit(EXIT_FAILURE);
    }

    union sem s;
    s.array = parray;
    int ret = semctl(semSetId, 0, SETALL, s);
    if (ret == -1)
    {
        DEBUG_INFO("%s", strerror(errno));
        exit(EXIT_FAILURE);
    }
    return semSetId;
}
// 抢占具体哪个资源的信号量 传参:集合id 信号量id
void semP(int semSetId, int psemId)
{
    struct sembuf buf;
    buf.sem_num = psemId;
    buf.sem_op = -1;
    //  buf.sem_flg = SEM_UNDO; 这个标志不要设置 进程结束会自动回滚...
    int ret = semop(semSetId, &buf, 1);
    if (ret == -1)
    {
        DEBUG_INFO("%s", strerror(errno));
        exit(EXIT_FAILURE);
    }
}
/* 释放信号量资源 */
void semV(int semSetId, int psemId)
{
    struct sembuf buf;
    buf.sem_num = psemId;
    buf.sem_op = 1;
    //   buf.sem_flg = SEM_UNDO;
    int ret = semop(semSetId, &buf, 1);
    if (ret == -1)
    {
        DEBUG_INFO("%s", strerror(errno));
        exit(EXIT_FAILURE);
    }
}
void delSem(int semSetId)
{
    int ret = semctl(semSetId, 0, IPC_RMID);
    if (ret == -1)
    {
        DEBUG_INFO("%s", strerror(errno));
        exit(EXIT_FAILURE);
    }
}