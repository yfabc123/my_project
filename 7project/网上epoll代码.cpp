#include <iostream>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
//'/0'->'\0'
// bzero() 替换为memset （注意二者参数不一样，bzero将前n个字节设为0，memset将前n 个字节的值设为值 c）
// local_addr 由char* 改为 string
#include <cstdlib>
#include <stdlib.h> //atoi
#include <string.h> //memset
using namespace std;
#define MAXLINE 255 // 读写缓冲
#define OPEN_MAX 100
#define LISTENQ 20 // listen的第二个参数  定义TCP链接未完成队列的大小（linux >2.6 则表示accpet之前的队列）
#define SERV_PORT 5000
#define INFTIM 1000
#define TIMEOUT_MS 500
#define EVENT_MAX_COUNT 20
void setnonblocking(int sock)
{
    int opts;
    opts = fcntl(sock, F_GETFL);
    if (opts < 0)
    {
        perror("fcntl(sock,GETFL)");
        exit(1);
    }
    opts = opts | O_NONBLOCK;
    if (fcntl(sock, F_SETFL, opts) < 0)
    {
        perror("fcntl(sock,SETFL,opts)");
        exit(1);
    }
}
int main(int argc, char *argv[])
{
    int i, maxi, listenfd, connfd, sockfd, epfd, nfds, portnumber;
    ssize_t n;
    char line_buff[MAXLINE];
    if (2 == argc)
    {
        if ((portnumber = atoi(argv[1])) < 0)
        {
            fprintf(stderr, "Usage:%s portnumber/r/n", argv[0]);
            // fprintf()函数根据指定的format(格式)(格式)发送信息(参数)到由stream(流)指定的文件
            // printf 将内容发送到Default的输出设备，通常为本机的显示器，fprintf需要指定输出设备，可以为文件，设备。
            // stderr
            return 1;
        }
    }
    else
    {
        fprintf(stderr, "Usage:%s portnumber/r/n", argv[0]);
        return 1;
    }
    // 声明epoll_event结构体的变量,ev用于注册事件,数组用于回传要处理的事件
    struct epoll_event ev, event_list[EVENT_MAX_COUNT];
    // 生成用于处理accept的epoll专用的文件描述符
    epfd = epoll_create(256); // 生成epoll文件描述符,既在内核申请一空间，存放关注的socket fd上是否发生以及发生事件。size既epoll fd上能关注的最大socket fd数。随你定好了。只要你有空间。
    struct sockaddr_in clientaddr;
    socklen_t clilenaddrLen;
    struct sockaddr_in serveraddr;
    listenfd = socket(AF_INET, SOCK_STREAM, 0); // Unix/Linux“一切皆文件”，创建（套接字）文件，id=listenfd
    if (listenfd < 0)
    {
        printf("socket error,errno %d:%s\r\n", errno, strerror(errno));
    }
    // 把socket设置为非阻塞方式
    // setnonblocking(listenfd);
    // 设置与要处理的事件相关的文件描述符
    ev.data.fd = listenfd;
    // 设置要处理的事件类型
    ev.events = EPOLLIN | EPOLLET; // EPOLLIN ：表示对应的文件描述符可以读，EPOLLET状态变化才通知
    // ev.events=EPOLLIN;
    // 注册epoll事件
    epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev); // epfd epoll实例ID，EPOLL_CTL_ADD添加，listenfd:socket,ev事件（监听listenfd）
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY); /*IP，INADDR_ANY转换过来就是0.0.0.0，泛指本机的意思，也就是表示本机的所有IP*/
    serveraddr.sin_port = htons(portnumber);
    if (0 != bind(listenfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)))
    {
        printf("bind error,errno %d:%s\r\n", errno, strerror(errno));
    }
    if (0 != listen(listenfd, LISTENQ)) // LISTENQ 定义了宏
    {
        printf("listen error,errno %d:%s\r\n", errno, strerror(errno));
    }
    
    maxi = 0;
    for (;;)
    {
        // 等待epoll事件的发生
        nfds = epoll_wait(epfd, event_list, EVENT_MAX_COUNT, TIMEOUT_MS); // epoll_wait(int epfd, struct epoll_event * event_list, int maxevents, int timeout)，返回需要处理的事件数目
        // 处理所发生的所有事件
        for (i = 0; i < nfds; ++i)
        {
            if (event_list[i].data.fd == listenfd) // 如果新监测到一个SOCKET用户连接到了绑定的SOCKET端口，建立新的连接。
            {
                clilenaddrLen = sizeof(struct sockaddr_in);                                // 在调用accept()前，要给addrLen赋值,这样才不会出错,addrLen = sizeof(clientaddr);或addrLen = sizeof(struct sockaddr_in);
                connfd = accept(listenfd, (struct sockaddr *)&clientaddr, &clilenaddrLen); // （accpet详解：https://blog.csdn.net/David_xtd/article/details/7087843）
                if (connfd < 0)
                {
                    // perror("connfd<0:connfd= %d",connfd);
                    printf("connfd<0,accept error,errno %d:%s\r\n", errno, strerror(errno));
                    exit(1);
                }
                // setnonblocking(connfd);
                char *str = inet_ntoa(clientaddr.sin_addr); // 将一个32位网络字节序的二进制IP地址转换成相应的点分十进制的IP地址
                cout << "accapt a connection from " << str << endl;
                // 设置用于读操作的文件描述符
                ev.data.fd = connfd;
                // 设置用于注测的读操作事件
                ev.events = EPOLLIN | EPOLLET;
                // ev.events=EPOLLIN;
                // 注册ev
                epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &ev); // 将accpet的句柄添加进入（增加监听的对象）
            }
            else if (event_list[i].events & EPOLLIN) // 如果是已经连接的用户，并且收到数据，那么进行读入。
            {
                cout << "EPOLLIN" << endl;
                if ((sockfd = event_list[i].data.fd) < 0)
                    continue;
                if ((n = read(sockfd, line_buff, MAXLINE)) < 0) // read时fd中的数据如果小于要读取的数据，就会引起阻塞?
                {
                    // 当read()或者write()返回-1时，一般要判断errno
                    if (errno == ECONNRESET) // 与客户端的Socket被客户端强行被断开，而服务器还企图read
                    {
                        close(sockfd);
                        event_list[i].data.fd = -1;
                    }
                    else
                        std::cout << "readline error" << std::endl;
                }
                else if (n == 0) // 返回的n为0时，说明客户端已经关闭
                {
                    close(sockfd);
                    event_list[i].data.fd = -1;
                }
                line_buff[n] = '\0';
                cout << "read " << line_buff << endl;
                // 设置用于写操作的文件描述符
                ev.data.fd = sockfd;
                // 设置用于注测的写操作事件
                ev.events = EPOLLOUT | EPOLLET; // EPOLLOUT：表示对应的文件描述符可以写；
                // 修改sockfd上要处理的事件为EPOLLOUT
                // epoll_ctl(epfd,EPOLL_CTL_MOD,sockfd,&ev);
            }
            else if (event_list[i].events & EPOLLOUT) // 如果有数据发送
            {
                sockfd = event_list[i].data.fd;
                write(sockfd, line_buff, n);
                // 设置用于读操作的文件描述符
                ev.data.fd = sockfd;
                // 设置用于注测的读操作事件
                ev.events = EPOLLIN | EPOLLET;
                // 修改sockfd上要处理的事件为EPOLIN
                epoll_ctl(epfd, EPOLL_CTL_MOD, sockfd, &ev);
            }
        }
    }
    return 0;
}