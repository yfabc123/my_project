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
    char *recvBuf = NULL;
    int len; // 保存长度信息
    myBind(sfd, SERVERIP, SERVERPORT, NULL);
    myListen(sfd, 3);
    struct sockaddr_in peerAddr;
    int newSfd = myAccept(sfd, &peerAddr);
    size_t size;
    while (1)
    {
        size = myRecv(newSfd, &len, sizeof(int), MSG_NOSIGNAL);
        printf("%d\n", len);
        recvBuf = (char *)malloc(sizeof(int) + len); // 申请总的字节存放数据
        int cur = 0;                                 // 当前读取字节数
        while (cur < len)
        {
            size = myRecv(newSfd, recvBuf + sizeof(int) + cur, len - cur, MSG_NOSIGNAL); // 假设每次接收100字节 返回值是实际读取字节
            cur = cur + size;
            printf("cur:%ld a:%d\n", cur, len);
            if (len == cur)
            {
                printf("已读完一份数据\n");
            }
            if (size == 0)
            {
                break;
            }
            sleep(1);
        }
        printf("长度%d,内容:%s\n", len, recvBuf + sizeof(int));
        if (size == 0)
        {
            memset(recvBuf, 0, sizeof(recvBuf));
            newSfd = myAccept(sfd, &peerAddr);
        }
        free(recvBuf);
        recvBuf = NULL;
        sleep(1);
    }
    return 0;
}
