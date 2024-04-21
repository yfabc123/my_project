#include <hiredis/hiredis.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void* recvMsg(void* arg)
{
    redisContext* context = redisConnect("127.0.0.1", 6379);
    // 订阅自己的id用来接收发给自己的消息
    redisReply* reply = redisCommand(context, "SUBSCRIBE a");
    if (reply == NULL || reply->type == REDIS_REPLY_ERROR)
    {
        printf("bug\n");
        freeReplyObject(reply);
        redisFree(context);
    }
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
        freeReplyObject(msg);
    }
    return NULL;
}
int main()
{

    redisContext* context = redisConnect("127.0.0.1", 6379);
    if (context == NULL || context->err)
    {
        if (context)
        {
            printf("连接redis报错 %s\n", context->errstr);
            redisFree(context);
        }
        else
            printf("其他\n");
        return 1;
    }
    pthread_t tid;
    pthread_create(&tid, NULL, recvMsg, NULL);
    // 发布消息
    char input[1024];
    while (1)
    {
        printf("发送内容:");
        memset(input, 0, sizeof(input));
        fgets(input, 1024, stdin);
        input[strcspn(input, "\n")] = 0;
        redisReply* reply = redisCommand(context, "PUBLISH b %s", input);
        if (reply == NULL)
            printf("发送失败\n");
        else
        {
            printf("发送成功\n");
            freeReplyObject(reply);
        }
    }
    // 断开与 Redis 的连接
    redisFree(context);
    return 0;
}
