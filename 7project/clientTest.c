#include "tcpSocket.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#define SERVERIP   "192.168.2.4"
#define SERVERPORT 1
void dosome(int sfd, char *buf, long size)
{
    fgets(buf, sizeof(buf), stdin);
    buf[strlen(buf) - 1] = 0;
    size = mySend(sfd, buf, strlen(buf), MSG_NOSIGNAL);
    printf("%ld\n", size);
    if (size <= 0)
    {
        printf("%ld\n", size);
        printf("%s\n", strerror(errno));
    }
    size = myRecv(sfd, buf, size, MSG_NOSIGNAL);
    if (size <= 0)
    {
        printf("%ld\n", size);
        printf("%s\n", strerror(errno));
    }
    fprintf(stderr, "收到服务器的消息是:%s\n", buf);
}
int main(int argc, char const *argv[])
{
    int sfd = myCreateTcpSocket();
    char buf[10] = {0};
    myConnect(sfd, SERVERIP, SERVERPORT, NULL);
    size_t size;
    while (1)
    {
        for (int i = 0; i < 10000; i++)
        {
            // sleep(1);
            sprintf(buf, "%d", i);
            size = mySend(sfd, buf, strlen(buf), MSG_NOSIGNAL);
            if (size <= 0)
            {
                printf("%s\n", strerror(errno));
            }
            size = myRecv(sfd, buf, size, MSG_NOSIGNAL);
            if (size <= 0)
            {
                printf("%s\n", strerror(errno));
            }
            fprintf(stderr, "收到服务器的消息是:%s\n", buf);
        }
        break;
    }
    return 0;
}
