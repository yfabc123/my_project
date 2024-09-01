#include "client.h"
#include "../global.h"
#include "../mq.h"
#include "../shm.h"
#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
void clientRun()
{
    createShm(2, sizeof(User));
    createMq();
    int number;
    printf("请输入0(发布)或者1(订阅):");
    scanf("%d", &number);
    int ch;
    // 缓冲区b没清理导致fgets 获取了换行符
    getchar();
    if (number == 0)
        publish();
    else if (number == 1)
        subscribe();
    else
        printf("其他\n");
}
void publish()
{
    User pub;
    while (1)
    {
        memset(&pub, 0, sizeof(User));
        pub.action = 0;
        printf("请输入主题:\n");
        fgets(pub.topic, sizeof(pub.topic), stdin);
        pub.topic[strcspn(pub.topic, "\n")] = 0;
        printf("请输入内容:\n");
        fgets(pub.content, sizeof(pub.content), stdin);
        pub.topic[strcspn(pub.content, "\n")] = 0;
        putShm(shm, &pub);
    }
}
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
        mqRecv(mqId, sub.pid);
    }
}