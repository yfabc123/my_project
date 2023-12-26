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
        size_t len = sizeof(buf);
        char *message = malloc(len + sizeof(int));
        ((int *)message)[0] = len;
        memcpy((char *)message + sizeof(int), buf, len);
        mySend(sfd, message, len + sizeof(int), 0);
        //  printf("发送给您内容%s\n", buf);
        free(message);
        sleep(1);
    }
    return 0;
}
