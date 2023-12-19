#ifndef GLOBAL_HEAD_H
#define GLOBAL_HEAD_H
#include <netinet/in.h>
typedef struct
{
    char message[1024];      // 消息内容
    int type;                // 消息类型 0上线 1聊天 2下线
    char user_name[64];      // 用户名
    int user_id;             // 用户id
    struct sockaddr_in addr; // 地址信息
    char time[64];           // 记录消息时间
} Packet;

#endif