#include "Global.h"
#include <cstring>
mutex Global::insertMutex;
mutex Global::updateMutex;
mutex Global::freeMutex;
SocketInfo *Global::socketInfo[HASH_COUNT];
// 初始化哈希表函数
void Global::initHash()
{
    for (int i = 0; i < HASH_COUNT; i++)
    {
        socketInfo[i] = nullptr;
    }
}
int Global::insertHash(int paraSfd, string paraFileName)
{
    lock_guard<mutex> lock(Global::insertMutex);
    int index = paraSfd % HASH_COUNT;
    for (int i = 0; i < HASH_COUNT; i++)
    {
        index = (index + i) % HASH_COUNT;
        if (socketInfo[index] == nullptr)
        {
            socketInfo[index] = new SocketInfo;
            socketInfo[index]->offset = 0;
            strcpy(socketInfo[index]->fileName, paraFileName.c_str());
            socketInfo[index]->sfd = paraSfd;
            return 1;
        }
    }
    return -1;
}
// 用来获取自己套接字索引 发送文件的读取偏移量
int Global::findHash(int paraSfd)
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
int Global::updateHash(int paraSfd, int paraOffset)
{
    int index = findHash(paraSfd);
    if (-1 == index)
    {
        DEBUG_INFO("数据都没有还想更新??");
        return -1;
    }
    lock_guard<mutex> lock(Global::insertMutex);
    socketInfo[index]->offset = paraOffset;
    return 1;
}
int Global::freeHash(int paraSfd)
{
    int index = findHash(paraSfd);
    if (-1 == index)
    {
        DEBUG_INFO("未曾插入哈希");
        return -1;
    }
    lock_guard<mutex> lock(Global::insertMutex);
    delete socketInfo[index];
    socketInfo[index] = nullptr;
    return 1;
}
