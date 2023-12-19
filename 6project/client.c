#include "mySocket.h"
#include "dataBase.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include "./client.h"
#include "client.h"
#include <pthread.h>
/*登陆*/
Packet packet;
int login()
{
    char buf[64] = {0};
    int ret = -1;
    printf("请输入登陆账号,密码,并用空格连接:");
    fgets(buf, sizeof(buf), stdin);
    buf[strlen(buf) - 1] = 0; // 回车干掉
    if (!buf)
    {
        //  printf("输入格式不对\n");
        return -1;
    }
    char *a = strtok(buf, " ");
    char *b = strtok(NULL, " ");
    if (!b)
    {
        //  printf("输入格式不对\n");
        return -1;
    }
    ret = checkUser(mysql, a, b);
    if (ret == 1) // 账密正确更新结构体中姓名字段
    {
        memset(&packet, 0, sizeof(Packet));
        strcpy(packet.user_name, a);
    }
    return ret;
}
/* 注册 */
int registration()
{
    char buf[64] = {0};
    int ret = 0;
    printf("请输入注册账号,密码,并用空格连接:");
    fgets(buf, sizeof(buf), stdin);
    buf[strlen(buf) - 1] = 0; // 回车干掉
    if (!buf)
    {
        printf("输入格式不对\n");
        return -1;
    }
    char *a = strtok(buf, " ");
    char *b = strtok(NULL, " ");
    if (!b)
    {
        printf("输入格式不对\n");
        return -1;
    }
    ret = insertUser(mysql, a, b);
    if (ret == 1) // 账密正确更新结构体中姓名字段
    {
        memset(&packet, 0, sizeof(Packet));
        strcpy(packet.user_name, a);
    }
    return -1;
}
/*
注册登陆整合
统一按照字符传格式处理fgets
 */
int loginRegistration()
{
    connectDB();
    char buf[2] = {0};
    int ch;
    int ret;
    while (1)
    {
        memset(buf, 0, sizeof(buf));
        printf("请选择登陆(0),注册(1)还是退出(2):");
        fgets(buf, sizeof(buf), stdin);
        while ((ch = getchar()) != '\n' && ch != EOF)
            ;
        if (buf[0] == '0')
        {
            ret = login();
            if (ret == 1)
            {
                joinRoom();
            }
        }
        else if (buf[0] == '1')
        {
            ret = registration();
            if (ret == 1)
            {
                joinRoom();
            }
        }
        else if (buf[0] == '2')
        {
            printf("离开");
            break;
        }
        else
        {
            printf("输入格式不对\n");
        }
    }
    return 1;
}
/*
加入房间逻辑即连接udp服务器 客户端启动子线程接收消息?
*/
int joinRoom()
{
    pthread_t tid;
    intptr_t sfd = createMySocket();
    int ret;
    // memset(&packet, 0, sizeof(Packet)); 不能初始化 否则会把名字清除
    // 第一次不发送内容 只发送上线通知 还有姓名,如何获取姓名呢?
    ret = pthread_create(&tid, NULL, doThread, (void *)sfd);
    if (ret != 0)
    {
        perror("pthread_create");
        return -1;
    }
    pthread_detach(tid);
    packet.type = 0;
    ret = mySendTo(sfd, &packet, SERVERIP, SERVERPORT, sizeof(packet), NULL);
    if (ret < 1)
    {
        perror("mySendTo");
        return -1;
    }
    // 死循环发送消息
    packet.type = 1;
    while (1)
    {
        memset(packet.message, 0, sizeof(packet.message)); // 数据区清空
        fgets(packet.message, sizeof(packet.message), stdin);
        packet.message[strlen(packet.message) - 1] = 0;
        if (strcmp(packet.message, "exit") == 0)
        {
            packet.type = 2; // 离开模式
            ret = mySendTo(sfd, &packet, SERVERIP, SERVERPORT, sizeof(packet), NULL);
            close(sfd);
            exit(EXIT_SUCCESS);
        }
        ret = mySendTo(sfd, &packet, SERVERIP, SERVERPORT, sizeof(packet), NULL);
        // printf("发送内容:姓名:%s 内容:%s消息类型:%d\n", packet.user_name, packet.message, packet.type);
        if (ret < 1)
        {
            perror("mySendTo");
            return -1;
        }
    }
}
// 客户端子线程持续接收消息
void *doThread(void *arg)
{
    intptr_t sfd = (intptr_t)arg;
    Packet packet;
    struct sockaddr_in peerAddr;
    while (1)
    {
        //  printf("客户端子线程等待接收消息\n");
        myRecvFrom(sfd, &packet, sizeof(packet), &peerAddr);
        printf("%s(%s):%s\n", packet.user_name, packet.time, packet.message);
    }
}
/*
客户端启动函数
 */
void start()
{
    connectDB();
    loginRegistration();
}