#include "mq.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>
int mqId;
// 创建消息队列
void createMq()
{
    key_t key = ftok(MQPATH, MQPROJID);
    if (key == -1)
    {
        perror("ftok");
        exit(EXIT_FAILURE);
    }
    mqId = msgget(key, IPC_CREAT | 0666);
    if (mqId < 0)
    {
        perror("msgget");
        exit(EXIT_FAILURE);
    }
}
// 根据传进来的用户信息
void mqSend(int mqId, char* pmsgData, long pmsgType)
{
    Msg msg;
    memset(&msg, 0, sizeof(Msg));
    msg.msgType = pmsgType;
    strcpy(msg.msgData, pmsgData);
    msgsnd(mqId, &msg, sizeof(msg.msgData), 0);
    printf("发送端已发送:队列id:%d 消息类型id:%ld 发送内容:%s\n", mqId, msg.msgType, msg.msgData);
}
void mqRecv(int mqId, long pmsgType)
{
    Msg msg;
    memset(&msg, 0, sizeof(msg.msgData));
    msg.msgType = pmsgType;
    // printf("预期收到:队列id:%d 消息类型id:%ld 消息大小:%ld 消息长度:%ld\n", mqId, pmsgType, sizeof(msg.msgData), strlen(msg.msgData));
    msgrcv(mqId, &msg, sizeof(msg.msgData), pmsgType, 0);
    // printf("实际收到:队列id:%d 消息类型id:%ld 消息大小:%ld 消息长度:%ld\n", mqId, pmsgType, sizeof(msg.msgData), strlen(msg.msgData));
    printf("收到的内容:%s\n", msg.msgData);
}