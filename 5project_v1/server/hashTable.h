#ifndef HASHTABLE_HEAD_H
#define HASHTABLE_HEAD_H
#include "../global.h"
#include <bits/pthreadtypes.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define HASH_COUNT 10 // 哈希表数组最多10个

//  发布者哈希结构
typedef struct pubNode
{
    char topic[64]; // 索引
    char content[64];
    struct pubNode* next;
} PubHash;

// 订阅者哈希结构
typedef struct subNode
{
    char topic[64]; // 索引
    pthread_t pid;
    struct subNode* next;
} SubHash;

extern pthread_mutex_t pubMurex;
extern pthread_mutex_t subMurex;

extern PubHash** pubHash;
extern SubHash** subHash;
extern void createSubHash();
extern void createPubHash();
extern void insertSub( User* user);
extern void insertPub( User* user);
extern void doSubscribe( User* user);
extern void doPublish( User* user);
#endif
