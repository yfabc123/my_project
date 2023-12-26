#include "global.h"
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

pthread_mutex_t insertMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t updateMutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t freeMutex = PTHREAD_MUTEX_INITIALIZER;

SocketInfo *socketInfo[HASH_COUNT];
// 初始化哈希表函数
void initHash()
{
    for (int i = 0; i < HASH_COUNT; i++)
    {
        socketInfo[i] = NULL;
    }
}
int insertHash(int paraSfd, char *paraFileName)
{
    pthread_mutex_lock(&insertMutex);
    int index = paraSfd % HASH_COUNT;
    for (int i = 0; i < HASH_COUNT; i++)
    {
        index = (index + i) % HASH_COUNT;
        if (socketInfo[index] == NULL)
        {
            socketInfo[index] = (SocketInfo *)malloc(sizeof(SocketInfo));
            memset(socketInfo[index], 0, sizeof(SocketInfo));
            socketInfo[index]->offset = 0;

            strcpy(socketInfo[index]->fileName, paraFileName);
            socketInfo[index]->sfd = paraSfd;
            pthread_mutex_unlock(&insertMutex);
            return 1;
        }
    }
    pthread_mutex_unlock(&insertMutex);
    return -1;
}
// 用来获取自己套接字索引 发送文件的读取偏移量
int findHash(int paraSfd)
{
    int index = paraSfd % HASH_COUNT;
    for (int i = 0; i < HASH_COUNT; i++)
    {
        index = (index + i) % HASH_COUNT;
        if (socketInfo[index] && (socketInfo[index]->sfd == paraSfd))
        {
            return index;
        }
    }
    return -1; // 没找到
}
int updateHash(int paraSfd, int paraOffset)
{
    int index = findHash(paraSfd);
    if (-1 == index)
    {
        DEBUG_INFO("数据都没有还想更新??");
        return -1;
    }
    pthread_mutex_lock(&updateMutex);
    socketInfo[index]->offset = paraOffset;
    pthread_mutex_unlock(&updateMutex);
    return 1;
}
int freeHash(int paraSfd)
{
    int index = findHash(paraSfd);
    if (-1 == index)
    {
        DEBUG_INFO("未曾插入哈希");
        return -1;
    }
    pthread_mutex_lock(&freeMutex);
    free(socketInfo[index]);
    socketInfo[index] = NULL;
    pthread_mutex_unlock(&freeMutex);
    return 1;
}
