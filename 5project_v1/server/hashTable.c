#include "hashTable.h"
#include "../mq.h"
#include "threadPool.h"
#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
SubHash** subHash;
PubHash** pubHash;
pthread_mutex_t pubMurex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t subMurex = PTHREAD_MUTEX_INITIALIZER;
/* 哈希映射采用 数据长度%10作为索引 冲突则开放地址法 */
// 订阅者

void createSubHash()
{
    subHash = (SubHash**)calloc(HASH_COUNT, sizeof(SubHash*));
    if (subHash == NULL)
    {
        DEBUG_INFO("%s", strerror(errno));
        exit(EXIT_FAILURE);
    }
}
// 插入订阅者数据
void insertSub(User* user)
{
    SubHash* temp = (SubHash*)calloc(1, sizeof(SubHash));

    strcpy(temp->topic, user->topic);
    temp->next = NULL;
    temp->pid = user->pid;

    int index = strlen(user->topic) % HASH_COUNT; // 先确定索引
    pthread_mutex_lock(&subMurex);

    for (int i = 0; i < HASH_COUNT; i++, index = (index + 1) % HASH_COUNT)
    {
        if (subHash[index] == NULL)
        {
            subHash[index] = temp;
            break;
        }
        else if (strcmp(subHash[index]->topic, user->topic) == 0)
        {
            temp->next = subHash[index];
            subHash[index] = temp;
            break;
        }
    }
    pthread_mutex_unlock(&subMurex);
}
// 发布者
void createPubHash()
{
    pubHash = (PubHash**)calloc(HASH_COUNT, sizeof(PubHash*));
    if (pubHash == NULL)
    {
        DEBUG_INFO("%s", strerror(errno));
        exit(EXIT_FAILURE);
    }
}
// 插入发布者数据
void insertPub(User* user)
{
    PubHash* temp = (PubHash*)malloc(sizeof(PubHash));
    memset(temp, 0, sizeof(PubHash));
    strcpy(temp->topic, user->topic);
    strcpy(temp->content, user->content);
    temp->next = NULL;
    int index = strlen(user->topic) % HASH_COUNT; // 先确定索引
    printf("插入发布者哈希表\n");

    pthread_mutex_lock(&pubMurex);
    for (int i = 0; i < HASH_COUNT; i++, index = (index + 1) % HASH_COUNT)
    {
        if (pubHash[index] == NULL)
        {
            pubHash[index] = temp;
            break;
        }
        else if (strcmp(pubHash[index]->topic, user->topic) == 0)
        {
            temp->next = pubHash[index];
            pubHash[index] = temp;
            break;
        }
    }
    pthread_mutex_unlock(&pubMurex);
}
/* 有用户订阅时 先把订阅信息插入哈希表 然后遍历发布者哈希表 将内容数据全部发送给订阅进程 */
void doSubscribe(User* user)
{
    DEBUG_INFO("进入订阅逻辑");
    insertSub(user);
    int index = strlen(user->topic) % HASH_COUNT;
    for (int i = 0; i < HASH_COUNT; i++, index = (index + 1) % HASH_COUNT)
    {
        DEBUG_INFO("发送历史数据给上线订阅者");

        if (!pubHash[index]) //说明没有这个主题
            return;
        if (!strcmp(pubHash[index]->topic, user->topic))
        {
            PubHash* ptr = pubHash[index];
            while (ptr)
            {
                DEBUG_INFO("发送历史数据给上线订阅者");
                mqSend(mqId, ptr->content, user->pid);
                ptr = ptr->next;
            }
        }
    }
}
// 有用户发布时遍历订阅者哈希表查找该主题的订阅进程 并发送内容
void doPublish(User* user)
{
    DEBUG_INFO("进入发布逻辑");
    insertPub(user);
    int index = strlen(user->topic) % HASH_COUNT;

    for (int i = 0; i < HASH_COUNT; i++, index = (index + 1) % HASH_COUNT)
    {
        if (!subHash[index]) //说明没有人订阅当前主题
            return;
        if (!strcmp(subHash[index]->topic, user->topic))
        {
            SubHash* ptr = subHash[index];
            while (ptr)
            {
                printf("发布逻辑:发布端要发布的内容%s\n", user->content);
                printf("发布逻辑:发布端要发布的消息类型%ld\n", ptr->pid);
                mqSend(mqId, user->content, ptr->pid);
                ptr = ptr->next;
            }
        }
    }
}