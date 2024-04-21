#include "fun.h"
#include "global.h"
#include "tcpSocket.h"
#include <errno.h>
#include <hiredis/hiredis.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <unistd.h>
int main()
{
    redisContext* context = redisConnect("127.0.0.1", 6379);
    if (context == NULL || context->err)
    {
        if (context)
            printf("连接redis报错 %s\n", context->errstr);
        else
            printf("其他问题\n");
        redisFree(context);
        exit(EXIT_FAILURE);
    }
    // 发送 FLUSHDB 命令清空当前数据库中的所有键
    redisReply* reply = redisCommand(context, "FLUSHDB");
    if (reply == NULL)
    {
        printf("异常\n");
        redisFree(context);
        return 1;
    }
    int mainSocket = myCreateTcpSocket();
    setNonblocking(mainSocket);
    int len; // 保存长度信息
    myBind(mainSocket, SERVERIP, SERVERPORT, NULL);
    myListen(mainSocket, 10);
    struct sockaddr_in peerAddr;
    int newSfd;
    int ret = 0;
    size_t epfd = epoll_create(1); // 创建主线程自己的epoll实例
    struct epoll_event event;
    event.events = EPOLLIN | EPOLLET; // 主套接字边缘模式
    event.data.fd = mainSocket;
    epoll_ctl(epfd, EPOLL_CTL_ADD, mainSocket, &event);
    struct epoll_event readSockets[10];
    int waitRet;
    User packet; //保存读取到的数据报
    int i = 0;   // 记录把套接字分配给指定线程
    while (1)
    {
        printf("等待连接状态\n");
        waitRet = epoll_wait(epfd, readSockets, 10, -1); //
        if (waitRet < 0)
        {
            printf("%s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        else
        {
            printf("有事件发生进行业务处理\n");
            memset(&packet, 0, sizeof(User));
            for (int i = 0; i < waitRet; i++)
            {
                if (readSockets[i].data.fd == mainSocket)
                {
                    newSfd = myAccept(mainSocket, &peerAddr);
                    event.events = EPOLLIN;
                    event.data.fd = newSfd;
                    setNonblocking(newSfd);
                    epoll_ctl(epfd, EPOLL_CTL_ADD, newSfd, &event);
                    continue;
                }
                if (readSockets[i].events & EPOLLIN)
                {
                    int tempsfd = readSockets[i].data.fd;
                    ret = myUpgradeRecv(tempsfd, &packet, sizeof(packet), 0);
                    /*客户端关闭连接 更新redis中用户状态 先根据套接字找到用户id 然后从在线清单中删除 */
                    if (ret == 0)
                    {
                        printf("有连接断开\n");
                        redisReply* reply = redisCommand(context, "HGET  socket_userId %d", tempsfd);
                        if (reply == NULL)
                            printf("调用redis命令失败\n");
                        char temp[1024] = {0};
                        if (!reply->str)
                        {
                            printf("响应字符串为空\n");
                            continue;
                        }
                        strcpy(temp, reply->str);
                        printf("%s", temp);
                        freeReplyObject(reply);
                        reply = redisCommand(context, "SREM online_user  %s", temp);
                        if (reply == NULL)
                            printf("调用redis命令失败\n");
                        freeReplyObject(reply);
                        close(tempsfd);
                        epoll_ctl(epfd, EPOLL_CTL_DEL, tempsfd, &event);
                        continue;
                    }
                    //用户上线 更新状态
                    if (packet.type == 1)
                    {
                        printf("用户上线\n");
                        redisReply* reply = redisCommand(context, "SADD online_user  %d", packet.userId);
                        if (reply == NULL)
                            printf("调用redis命令失败\n");
                        freeReplyObject(reply);
                        //建立socket 和userid 关系便于快速找到接收方
                        reply = redisCommand(context, "HSET  socket_userId %d %d", tempsfd, packet.userId);
                        if (reply == NULL)
                            printf("调用redis命令失败\n");
                        freeReplyObject(reply);
                        reply = redisCommand(context, "HSET  userId_socket %d %d", packet.userId, tempsfd);
                        if (reply == NULL)
                            printf("调用redis命令失败\n");
                        freeReplyObject(reply);
                    }
                    else if (packet.type == 2)
                    {
                        printf("有用户申请好友\n");
                        redisReply* reply = redisCommand(context, "SISMEMBER online_user %d", packet.friendId);
                        printf("type:%d\ninteger:%lld\n len:%ld\n str:%s\n elements:%zu",
                               reply->type, reply->integer, reply->len, reply->str, reply->elements);
                        if (reply == NULL)
                        {
                            printf("调用redis命令失败\n");
                            freeReplyObject(reply);
                        }
                        else if (reply->integer == 0) //回复客户端用户没上线
                        {
                            printf("没有该好友信息\n");
                            packet.friendId = 0; // 0表示服务回复
                            strcpy(packet.content, "用户没上线");
                            mySend(tempsfd, &packet, sizeof(packet), 0);
                            freeReplyObject(reply);
                        }
                        else
                        { //需要添加一个好友列表
                            printf("创建好友列表\n");
                            reply = redisCommand(context, "SADD %dFriendList %d", packet.userId, packet.friendId);
                            freeReplyObject(reply);
                            reply = redisCommand(context, "SADD %dFriendList %d", packet.friendId, packet.userId);
                            freeReplyObject(reply);
                        }
                    } //如何把消息转发给指定用户
                    else if (packet.type == 3 || packet.type == 4)
                    {
                        printf("发送消息或者文件给好友\n");
                        //确认输入用户是不是当前用户好友
                        redisReply* reply = redisCommand(context, "SMEMBERS  %dFriendList", packet.userId);
                        if (reply == NULL)
                            printf("调用redis命令失败\n");
                        int i = 0;
                        int len = reply->elements;
                        for (; i < len; i++)
                        {
                            if (atoi(reply->element[i]->str) == packet.friendId) //说明好友确实存在
                            {
                                freeReplyObject(reply);
                                break;
                            }
                        }
                        if (i == len) //说明好友不存在
                        {
                            strcpy(packet.content, "当前用户非好友无法发送\n");
                            mySend(tempsfd, &packet, sizeof(packet), 0);
                            freeReplyObject(reply);
                            continue;
                        }
                        reply = redisCommand(context, "HGET  userId_socket %d", packet.friendId);
                        if (reply == NULL)
                            printf("调用redis命令失败\n");
                        mySend(atoi(reply->str), &packet, sizeof(packet), 0);
                        freeReplyObject(reply);
                    }
                    else if (packet.type == 5)
                    {
                        printf("查看好友状态\n");
                        redisReply* reply1 = redisCommand(context, "SMEMBERS %dFriendList", packet.userId);
                        redisReply* reply2 = redisCommand(context, "SMEMBERS online_user");
                        memset(packet.content, 0, sizeof(packet.content));
                        for (int i = 0; i < reply1->elements; i++)
                        {
                            int j = 0;
                            for (; j < reply2->elements; j++)
                            {
                                if (strcmp(reply1->element[i]->str, reply2->element[j]->str) == 0)
                                {
                                    printf("有好友在线\n");

                                    char temp[1024] = {0};
                                    strcpy(temp, reply1->element[i]->str);
                                    strcat(temp, " online\n");
                                    strcat(packet.content, temp);
                                    break;
                                }
                            }
                            if (j == reply2->elements)
                            {
                                printf("有好友离线了");
                                char temp[1024] = {0};
                                strcpy(temp, reply1->element[i]->str);
                                strcat(temp, " not online\n");
                                strcat(packet.content, temp);
                            }
                        }
                        freeReplyObject(reply1);
                        freeReplyObject(reply2);
                        mySend(tempsfd, &packet, sizeof(packet), 0);
                    }
                }
            }
        }
    }
    return 0;
}