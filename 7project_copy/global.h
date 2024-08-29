#ifndef GLOBAL_HEAD_H
#define GLOBAL_HEAD_H
#include <pthread.h>
#include <stdio.h>
#define THREAD_CONNECTION 2 // 每个线程最大连接数
#define SUB_THREAD_COUNT  2 // 子线程个数
#define MAX(A, B)         ((A) > (B) ? (A) : (B))
#define DEBUG_INFO(arg) \
    fprintf(stderr, "[%s,%s,%d]:%s\n", __FILE__, __FUNCTION__, __LINE__, arg)
#define SERVERIP         "172.20.10.7"
#define SERVERPORT       8888
#define SERVER_FILE_PATH "/mnt/hgfs/filesharing/serverDir/"
#define CLIENT_FILE_PATH "/mnt/hgfs/filesharing/clientDir/"
typedef struct
{
    size_t contentLen;  // 实际数据大小这个很关键 因为可能发送的是二进制 不是字符串数据
    int type;           // 业务类型 区分上传还是下载 还是失败
    char content[1024]; // 本次发送数据内容
    char fileName[64];
} Packet;
typedef struct
{
    char fileName[1024]; // 文件名
    size_t offset;       // 文件偏移量
    int sfd;
} SocketInfo;
extern pthread_rwlock_t lock;
typedef struct AvlNode
{
    int key;
    int height;
    SocketInfo* data;
    struct AvlNode* left;
    struct AvlNode* right;
} AvlNode;
extern AvlNode* avlTree;
extern int getHeight(AvlNode* root);
extern int getBf(AvlNode* root);
extern AvlNode* create(int key, const char* name);
extern AvlNode* llRotate(AvlNode* root);
extern AvlNode* rrRotate(AvlNode* root);
extern AvlNode* lrRotate(AvlNode* root);
extern AvlNode* rlRotate(AvlNode* root);
extern AvlNode* insert(AvlNode* root, int key, const char* name);
extern AvlNode* find(AvlNode* root, int key);
extern AvlNode* del(AvlNode* root, int key);
#endif
