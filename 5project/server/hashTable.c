#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include "hashTable.h"
#include "../mq.h"
PubHash **pubHash;
SubHash **subHash;
/* 哈希映射采用 数据长度%10作为索引 冲突则开放地址法 */
// 订阅者
void createSubHash()
{
    subHash = (SubHash **)malloc(sizeof(SubHash *) * MAXCOUNT);
    if (subHash == NULL)
    {
        perror("subHash = (SubHash **)malloc(sizeof(SubHash *) * MAXCOUNT);");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < MAXCOUNT; i++)
    {
        subHash[i] = NULL;
    }
}
// 插入订阅者数据
void insertSub(SubHash **subHash, User *user)
{
    SubHash *temp = (SubHash *)malloc(sizeof(SubHash));
    memset(temp, 0, sizeof(SubHash));
    strcpy(temp->topic, user->topic);
    temp->next = NULL;
    temp->pid = user->pid;
    int index = strlen(user->topic) % MAXCOUNT; // 先确定索引
    printf("插入订阅者哈希表\n");
    SubHash **ptr;
    /*
    找到为空的位置 直接退出循环插如数据 此处使用二级指针作为迭代器遍历为了减少判断,
   找到指针数据为空的变量位置直接赋值即可
     也可以使用一级指针则需要判断第一个数组元素是不是空
     */
    for (ptr = subHash + index; *ptr; ptr = subHash + (index + 1) % MAXCOUNT)
    {
        // 判断是不是对应主题
        if (strcmp((*ptr)->topic, temp->topic) == 0)
        {
            // 放到该链表数据末尾
            while ((*ptr) != NULL)
            {
                ptr = &((*ptr)->next);
            }
            break;
        }
    }
    *ptr = temp;
}
// 发布者
void createPubHash()
{
    pubHash = (PubHash **)malloc(sizeof(PubHash *) * MAXCOUNT);
    for (int i = 0; i < MAXCOUNT; i++)
    {
        pubHash[i] = NULL;
    }
}
// 插入发布者数据
void insertPub(PubHash **pubHash, User *user)
{
    PubHash *temp = (PubHash *)malloc(sizeof(PubHash));
    memset(temp, 0, sizeof(PubHash));
    strcpy(temp->topic, user->topic);
    strcpy(temp->content, user->content);
    temp->next = NULL;
    int index = strlen(user->topic) % MAXCOUNT; // 先确定索引
    printf("插入发布者哈希表\n");
    PubHash **ptr;
    /*
    找到为空的位置 直接退出循环插如数据 此处使用二级指针作为迭代器遍历为了减少判断,
   找到指针数据为空的变量位置直接赋值即可
     也可以使用一级指针则需要判断第一个数组元素是不是空
     */
    for (ptr = pubHash + index; *ptr; ptr = pubHash + (index + 1) % MAXCOUNT)
    {
        // 判断是不是对应主题
        if (strcmp((*ptr)->topic, temp->topic) == 0)
        {
            // 放到该链表数据末尾
            while ((*ptr) != NULL)
            {
                ptr = &((*ptr)->next);
            }
            break;
        }
    }
    *ptr = temp;
}
/* 有用户订阅时 遍历发布者哈希表 将内容数据全部发送给订阅进程 */
void doSubscribe(PubHash **pubHash, User *user)
{
    printf("订阅逻辑\n");
    insertSub(subHash, user);
    PubHash **ptr = pubHash;
    int index = strlen(user->topic) % MAXCOUNT;
    for (ptr = pubHash + index; *ptr; ptr = pubHash + (index + 1) % MAXCOUNT) // 遍历发布端哈希表 将数据发送给订阅端
    {
        // 判断是不是对应主题 是的话此链表所有内容发送给用户进程
        if (strcmp((*ptr)->topic, user->topic) == 0)
        {
            while ((*ptr) != NULL)
            {
                printf("订阅逻辑:发布端要发布的内容%s\n", user->content);
                printf("订阅逻辑:发布端要发布的消息类型%ld\n", user->pid);
                mqSend(mqId, (*ptr)->content, user->pid);
                ptr = &((*ptr)->next);
            }
        }
    }
}
// 有用户发布时遍历订阅者哈希表查找该主题的订阅进程 并发送内容
void doPublish(SubHash **subHash, User *user)
{
    printf("发布逻辑\n");
    insertPub(pubHash, user);
    SubHash **ptr = subHash;
    int index = strlen(user->topic) % MAXCOUNT;
    for (ptr = subHash + index; *ptr; ptr = subHash + (index + 1) % MAXCOUNT)
    {
        // 判断是不是对应主题 是的话此链表所有内容发送给用户进程
        if (strcmp((*ptr)->topic, user->topic) == 0)
        {
            while ((*ptr) != NULL)
            {
                //     sleep(1);
                printf("发布逻辑:发布端要发布的内容%s\n", user->content);
                printf("发布逻辑:发布端要发布的消息类型%ld\n", (*ptr)->pid);
                mqSend(mqId, user->content, (*ptr)->pid);
                ptr = &((*ptr)->next);
            }
        }
    }
}