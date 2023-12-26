#include "tcpSocket.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#define SERVERIP "192.168.2.4"
#define SERVERPORT 1
int main(int argc, char const *argv[])
{
    int sfd = myCreateTcpSocket();
    char buf[1024] = {0};
    myConnect(sfd, SERVERIP, SERVERPORT, NULL);
    while (1)
    {
        myRecv(sfd, buf, sizeof(buf), 0);
        printf("客户端收到的消息是%s\n", buf);
        fgets(buf, sizeof(buf), stdin);
        mySend(sfd, buf, sizeof(buf), 0);
    }
    return 0;
}
