#ifndef _GLOBAL_H
#define _GLOBAL_H
#include <stdio.h>
#define SERVERIP   "192.168.2.9"
#define SERVERPORT 2
typedef struct
{
    int userId;
    int friendId;
    int type; // 1登陆 2添加好友 3发送消息 4发送文件 5查看好友列表（显示所有好友离线还是下线）
    char fileName[64];
    char content[1024];
    size_t fileSize; //文件大小

} User;
#endif