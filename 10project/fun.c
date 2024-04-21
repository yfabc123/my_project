#include "fun.h"
#include <hiredis/hiredis.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
char user_id[1024];
/* 登陆 */
void login(redisContext* context)
{
    memset(user_id, 0, 1024);
    // 以某个账号登陆连接到 Redis 服务器
    printf("请输入你的id:");
    fgets(user_id, 1024, stdin);
    user_id[strcspn(user_id, "\n")] = 0;
    //插入用户集合
    redisReply* reply = redisCommand(context, "SADD user_id %s", user_id);
    if (reply == NULL)
        printf("发送失败\n");
    //创建当前用户自己的好友集合
    reply = redisCommand(context, "SADD %s_friends", user_id);
    if (reply == NULL)
        printf("发送失败\n");

    // 订阅自己的id用来接收发给自己的消息
    reply = redisCommand(context, "SUBSCRIBE %s", user_id);
    if (reply == NULL || reply->type == REDIS_REPLY_ERROR)
        printf("bug\n");
    freeReplyObject(reply);
}

/* 添加好友 */
void addFriend(redisContext* context)
{
    // 以某个账号登陆连接到 Redis 服务器
    printf("请输入你要添加的好友id:");
    char input[1024];
    memset(input, 0, sizeof(input));
    fgets(input, 1024, stdin);
    input[strcspn(input, "\n")] = 0;

    //需要先查找好友是否存在
    redisReply* reply = redisCommand(context, "SISMEMBER user_id %s", input);
    if (reply == NULL || reply->type == REDIS_REPLY_ERROR)
        printf("异常\n");
    if (reply->integer == 0)
        printf("该用户不存在\n");
    else /* 好友集合表示为 用户id_friends 同时创建一个文件频道  */
    {
        reply = redisCommand(context, "SADD %s_friends %s", user_id, input); //把b放在a的好友中
        freeReplyObject(reply);
        reply = redisCommand(context, "SADD %s_friends %s", input, user_id); //把a也加到b的好友中
        freeReplyObject(reply);
    }
}

/* 发送信息 确定发送给哪个好友 */
void sendMsg(redisContext* context)
{
    printf("请输入你要发送的好友以及内容(空格隔开):");
    char frindId[1024] = {0};
    char content[1024] = {0};
    scanf("%s %s", frindId, content);
    redisReply* reply = redisCommand(context, "PUBLISH %s %s", frindId, content);
    if (reply == NULL)
        printf("发送失败\n");
    freeReplyObject(reply);
}

/* 发送文件完毕后告知好友 好友收到消息后进行判断是不是文件类型
是文件类型则调用下载接收内存中的文件数据 */
void sendFile(redisContext* context)
{
    printf("请输入你要发送的好友以及文件");
    char frindId[1024] = {0};
    char fileName[1024] = {0};
    scanf("%s %s", frindId, fileName);
    FILE* file = fopen(fileName, "rb");
    if (!file)
    {
        printf("打开文件异常");
        return;
    }
    char content[1024];
    size_t rsize = fread(content, 1, rsize, file);

    redisReply* reply = NULL;
    while (rsize > 0)
    {
        reply = redisCommand(context, "APPEND %s %b", fileName, content, rsize); //%b是二进制 同时增加一个长度字段
        if (!reply)
        {
            printf("redis接收文件异常 可能重名了\n");
            break;
        }
        rsize = fread(content, 1, rsize, file);
        freeReplyObject(reply);
    }
    /*发完文件在发送一个通知  发给对端的文件格式：file:1.txt 需要截取后面的字符串 */
    reply = redisCommand(context, "PUBLISH %s file:%s", frindId, fileName);
    freeReplyObject(reply);
}
/* 接收和发送不能使用同一个连接 */
void recvMsg(redisContext* context)
{
    while (1)
    {
        redisReply* msg = NULL;
        redisGetReply(context, (void**)&msg); //
        if (msg == NULL)
        {
            printf("bug\n");
            break;
        }
        if (msg->type == 2)
        {
            // printf("%s", msg->element[0]->str);//message
            // printf("%s", msg->element[1]->str);//频道表示
            printf("收到内容：%s\n", msg->element[2]->str); //收到内容
        }
        if (strstr(msg->element[2]->str, "file:"))
        {
            char* target = strstr(msg->element[2]->str, "file:");
            char filename[1024] = {0};
            /* 获取到文件名 然后进行拷贝文件到本地 */
            strcpy(filename, target + 5);
            FILE* file = fopen(filename, "ab");
            //SUBSCRIBE 命令结果集好像存在结构体数组中其他命令存在字符串？
            msg = redisCommand(context, "GET %s", filename);
            fwrite(msg->str, 1, msg->len,file);
        }
        freeReplyObject(msg);
    }
}
/* 查看好友列表 显示是否在线 */
void listFriend(redisContext* context)
{
}