#include "tcpSocket.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <pthread.h>
#define SERVERIP "192.168.2.4"
#define SERVERPORT 1
void *doSubThread(void *arg)
{
    intptr_t sfd = (intptr_t)arg;
    char sendRecvBuf[1024] = {0};
    strcpy(sendRecvBuf, "hello");
    int size;
    while (1)
    {
        size = mySend(sfd, sendRecvBuf, strlen(sendRecvBuf), MSG_NOSIGNAL);
        if (size == 0)
        {
            printf("客户端退出,子线程结束\n");
            break;
        }
        size = myRecv(sfd, sendRecvBuf, sizeof(sendRecvBuf), MSG_NOSIGNAL);
        if (size == 0)
        {
            printf("客户端退出,子线程结束\n");
            break;
        }
    }
    pthread_exit(NULL);
}
int main(int argc, char const *argv[])
{
    int sfd = myCreateTcpSocket();
    char *recvBuf = NULL;
    int len; // 保存长度信息
    myBind(sfd, SERVERIP, SERVERPORT, NULL);
    myListen(sfd, 3);
    struct sockaddr_in peerAddr;
    int ret = 0;
    while (1)
    {
        intptr_t newSfd = myAccept(sfd, &peerAddr);
        size_t size;
        pthread_t tid;
        ret = pthread_create(&tid, NULL, doSubThread, (void *)newSfd);
        if (ret != 0)
        {
            perror("pthread_create(&tid, NULL, doSubThread, (void *)newSfd)");
        }
        pthread_detach(tid);
    }
    return 0;
}
