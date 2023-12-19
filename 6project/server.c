#include "dataBase.h"
#include "server.h"
#include "mySocket.h"
#include "threadPool.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "linkedList.h"
#include <unistd.h>
// 服务器逻辑 当客户端发来请求 直接将客户端参数和待调用函数封装到任务结构体中
void runServer()
{
    connectDB();
    createLinkedList();
    initTpool(3, 3);
    int sfd = createMySocket();
    myBind(sfd, SERVERIP, SERVERPORT, NULL);
    Packet packet;
    while (1)
    {
        myRecvFrom(sfd, &packet, sizeof(packet), &packet.addr);
        printf("服务器收到:姓名:%s 内容:%s 消息类型:%d\n", packet.user_name, packet.message, packet.type);
        addTask(&packet);
    }
}
// 往任务队列添加任务
void addTask(Packet *parapacket)
{
    // 抢锁
    pthread_mutex_lock(&tpool->mutex);
    while (tpool->curTaskCount == tpool->maxTaskCount)
    {
        pthread_cond_wait(&tpool->fullCond, &tpool->mutex);
    }
    Packet *packet = (Packet *)malloc(sizeof(Packet)); // 申请空间然后拷贝 防止指针内容被修改 子线程处理完注意释放!
    memcpy(packet, parapacket, sizeof(Packet));
    printf("服务器装填:姓名:%s 内容:%s 消息类型:%d\n", packet->user_name, packet->message, packet->type);
    (tpool->tasks)[tpool->queueTail].function = doAll;
    (tpool->tasks)[tpool->queueTail].arg = packet;
    tpool->curTaskCount++;
    tpool->queueTail = (tpool->queueTail + 1) % tpool->maxTaskCount;
    printf("服务已经装填任务\n");
    pthread_mutex_unlock(&tpool->mutex);
    pthread_cond_signal(&tpool->emptyCond);
}
void *doAll(void *arg)
{
    Packet *packet = (Packet *)arg;
    getCurtime(packet->time);
    if (packet->type == 0)
    {
        doJoin(arg);
    }
    else if (packet->type == 1)
    {
        doChat(arg);
    }
    else if (packet->type == 2)
    {
        doExit(arg);
    }
}
void *doJoin(void *arg)
{
    int sfd = createMySocket();
    Packet *packet = (Packet *)arg;
    char temp[1024] = {0};
    strcpy(temp, packet->user_name);//提前拷贝姓名

    getHistoryMessage(mysql, sfd, packet);//将参数传递给数据库函数让其进行发送历史消息

    strcpy(packet->user_name, "系统通知");
    strcpy(packet->message, "欢迎");
    strcat(packet->message, temp);
    strcat(packet->message, "加入聊天室");

    insertLinkedList(head, packet);
    sendAll(head, packet, sfd);
    free(packet);
    close(sfd);
}
void *doChat(void *arg)
{
    int sfd = createMySocket();
    Packet *packet = (Packet *)arg;
    sendAll(head, packet, sfd);
    insertMessage(mysql, packet->user_name, packet->message, packet->time);
    free(packet);
    close(sfd);
}
void *doExit(void *arg)
{
    int sfd = createMySocket();
    Packet *packet = (Packet *)arg;
    deleteLinkedList(head, packet);
    sendAll(head, packet, sfd);
    free(packet);
    close(sfd);
}
// 获取时间
void getCurtime(char *paratime)
{
    time_t t;
    int a = 1;
    time(&t);
    char timeBuf[64] = {0};
    struct tm *ptm = localtime(&t);
    sprintf(paratime, "%d-%d-%d %d:%d:%02d", ptm->tm_year + 1900,
            ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
}