#ifndef GLOBAL_HEAD_H
#define GLOBAL_HEAD_H
#include <iostream>
#include <mutex>
#include <vector>
using namespace std;
#define THREAD_CONNECTION 2 // 每个线程最大连接数
#define HASH_COUNT        4 // 哈希表容量

#define SUB_THREAD_COUNT 2 // 子线程个数

#define DEBUG_INFO(arg) \
    fprintf(stderr, "[%s,%s,%d]:%s\n", __FILE__, __FUNCTION__, __LINE__, arg)

#define SERVERIP         "192.168.2.4"
#define SERVERPORT       1111
#define SERVER_FILE_PATH "/mnt/hgfs/filesharing/serverDir/"
#define CLIENT_FILE_PATH "/mnt/hgfs/filesharing/clientDir/"

struct Packet
{
    size_t contentLen;  // 实际数据大小这个很关键 因为可能发送的是二进制 不是字符串数据
    int type;           // 业务类型 区分上传还是下载 还是失败
    char content[4096]; //
    char fileName[64];
};
struct SocketInfo
{
    char fileName[64]; // 文件名
    size_t offset;   // 文件偏移量
    int sfd;
};

class Global
{
public:
    static mutex insertMutex;
    static mutex updateMutex;
    static mutex freeMutex;
    static SocketInfo *socketInfo[HASH_COUNT];
    void initHash();
    int insertHash(int paraSfd, string paraFileName);
    int findHash(int parSfd);
    int updateHash(int paraIndex, int paraOffset);
    int freeHash(int paraSfd);
};
#endif
