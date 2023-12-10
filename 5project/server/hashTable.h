#ifndef HASHTABLE_HEAD_H
#define HASHTABLE_HEAD_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#define MAXCOUNT 10 // 哈希表数组最多10个
typedef struct
{
    int action;       // 0发布 1订阅
    char topic[64];   // 主题
    char content[64]; // 内容
    pthread_t pid;   // 后面作为消息队列的消息类型
} User;

//  发布者哈希结构
typedef struct pubNode
{
    char topic[64]; // 索引
    char content[64];
    struct pubNode *next;
} PubHash;

// 订阅者哈希结构
typedef struct subNode
{
    char topic[64]; // 索引
    pthread_t pid;
    struct subNode *next;
} SubHash;

extern PubHash **pubHash;
extern SubHash **subHash;
extern void createSubHash();
extern void createPubHash();
extern void insertSub(SubHash **subHash, User *user);
extern void insertPub(PubHash **pubHash, User *user);
extern void doSubscribe(PubHash **pubHash, User *user);
extern void doPublish(SubHash **subHash, User *user);
#endif
