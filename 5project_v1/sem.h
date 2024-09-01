#ifndef SEM_HEAD_H
#define SEM_HEAD_H
#define SEMPATH "../"
#define SEMPROID 2
// 信号量结构体参数
union sem
{
    int val;
    short *array;
};
extern int createSem(int psemCount, short *parray);
extern void semP(int semSetId, int psemId);
extern void semV(int semSetId, int psemId);
extern void delSem(int semSetId);
#endif