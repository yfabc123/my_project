#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "../mq.h"
#include "client.h"
#include "../shm.h"
#include <sys/ipc.h>
#include <sys/msg.h>
void clientRun()
{
    createShm(2, sizeof(User));
    createMq();
    int number;
    printf("请输入0(发布)或者1(订阅):\n");
    scanf("%d", &number);
    int ch;
    // 缓冲区b没清理导致fgets 获取了换行符
    while ((ch = getchar()) != '\n')
        ;
    if (number == 0)
    {
        publish();
    }
    else if (number == 1)
    {
        subscribe();
    }
    else
    {
        printf("其他\n");
    }
}
void publish()
{
    User pub;
    memset(&pub, 0, sizeof(User));
    pub.action = 0;
    printf("请输入主题:");
    fgets(pub.topic, sizeof(pub.topic), stdin);
    pub.topic[strlen(pub.topic) - 1] = 0;
    printf("\n");
    printf("请输入内容:");
    fgets(pub.content, sizeof(pub.content), stdin);
    pub.content[strlen(pub.content) - 1] = 0;
    putShm(shm, &pub);
}
// 订阅之后持续等待消息 启一个子线程接收,
void subscribe()
{
    User sub;
    memset(&sub, 0, sizeof(User));
    sub.action = 1;
    printf("请确定订阅的的主题:\n");
    fgets(sub.topic, sizeof(sub.topic), stdin);
    sub.topic[strlen(sub.topic) - 1] = 0; // 去掉换行符 便于后续strcmp
    sub.pid = pthread_self();
    putShm(shm, &sub);
    while (1)
    {
        //  sleep(1);
        mqRecv(mqId, sub.pid);
    }
}