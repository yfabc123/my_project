#ifndef GLOBAL_HEAD_H
#define GLOBAL_HEAD_H
#include <pthread.h>
#include <stdio.h>
#define THREAD_CONNECTION 2 // 每个线程最大连接数
#define HASH_COUNT        4 // 哈希表容量
#define SUB_THREAD_COUNT 2 // 子线程个数
#define DEBUG_INFO(arg) \
    fprintf(stderr, "[%s,%s,%d]:%s\n", __FILE__, __FUNCTION__, __LINE__, arg)
#define SERVERIP   "192.168.2.4"
#define SERVERPORT 8888
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
extern pthread_mutex_t insertMutex;
extern pthread_mutex_t updateMutex;
extern pthread_mutex_t freeMutex;
extern SocketInfo *socketInfo[HASH_COUNT];
extern void initHash();
extern int insertHash(int paraSfd, char *paraFileName);
extern int findHash(int parSfd);
extern int updateHash(int paraIndex, int paraOffset);
extern int freeHash(int paraSfd);
// extern char *getFileName(char *path);
#endif
