
#include "server.h"
#include "dataBase.h"
#include "global.h"
#include "linkedList.h"
#include "mySocket.h"
#include "threadPool.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
// #include "threadPool_v1.h"
// 服务器逻辑 当客户端发来请求 直接将客户端参数和待调用函数封装到任务结构体中
void runServer()
{
    printf("服务器启动\n");
    connectDB();
    createLinkedList();
    ThreadPool* pool = creatThreadPool(MIN_TASK_COUNT, MAX_TASK_COUNT);

    int sfd = createMySocket();
    myBind(sfd, SERVERIP, SERVERPORT, NULL);

    while (1)
    {
        Packet* packet = (Packet*)calloc(1, sizeof(Packet));

        myRecvFrom(sfd, packet, sizeof(Packet), &packet->addr);
        printf("服务器收到数据\n");
        // printf("服务器收到数据:姓名:%s 内容:%s 消息类型:%d\n", packet->user_name, packet->message, packet->type);
        getCurtime(packet->time);
        if (packet->type == 0)
            threadPoolAdd(pool, doJoin, packet);
        else if (packet->type == 1)
            threadPoolAdd(pool, doChat, packet);
        else
            threadPoolAdd(pool, doExit, packet);
    }
}


void* doJoin(void* arg)
{
    int sfd = createMySocket();

    Packet* packet = (Packet*)arg;
    char temp[1024] = {0};
    strcpy(temp, packet->user_name); // 提前拷贝姓名
    getHistoryMessage(mysql, sfd, packet); // 将参数传递给数据库函数让其进行发送历史消息

    strcpy(packet->user_name, "系统通知");
    strcpy(packet->message, "欢迎");
    strcat(packet->message, temp);
    strcat(packet->message, "加入聊天室");

    insertLinkedList(head, packet);

    sendAll(head, packet, sfd);
    free(packet);
    close(sfd);
    return NULL;
}
void* doChat(void* arg)
{
    int sfd = createMySocket();
    Packet* packet = (Packet*)arg;
    sendAll(head, packet, sfd);
    insertMessage(mysql, packet->user_name, packet->message, packet->time);

    free(packet);
    close(sfd);
    return NULL;
}
void* doExit(void* arg)
{
    int sfd = createMySocket();
    Packet* packet = (Packet*)arg;
    deleteLinkedList(head, packet);
    sendAll(head, packet, sfd);
    free(packet);
    close(sfd);
    return NULL;
}
// 获取时间
void getCurtime(char* paratime)
{
    time_t t;
    int a = 1;
    time(&t);
    char timeBuf[64] = {0};
    struct tm* ptm = localtime(&t);
    sprintf(paratime, "%d-%d-%d %d:%d:%02d", ptm->tm_year + 1900,
            ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
}