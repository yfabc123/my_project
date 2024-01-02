#include "Server.h"
#include "Global.h"
#include "TcpSocket.h"
#include "Transfer.h"
#include <cerrno>
#include <cstring>
#include <errno.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
void Server::runServer()
{
    Global globa;
    Transfer transfer;
    TcpSocket tcpObj;
    globa.initHash(); // 初始化哈希表
    int mainSocket = tcpObj.myCreateTcpSocket();
    tcpObj.setNonblocking(mainSocket);
    // int len; // 保存长度信息
    tcpObj.myBind(mainSocket, SERVERIP, SERVERPORT, NULL);
    tcpObj.myListen(mainSocket, HASH_COUNT);
    struct sockaddr_in peerAddr;
    int ret = 0; // 保存返回值
    int newSfd;
    int instances[SUB_THREAD_COUNT];
    thread threads[SUB_THREAD_COUNT]; // 注意必须在外层 否则出了作用域线程对象被析构了
    for (int i = 0; i < SUB_THREAD_COUNT; i++)
    {
        instances[i] = epoll_create(1); // 创建子线程的epoll实例id
        threads[i] = thread(subThread, instances[i]);
    }
    size_t epfd = epoll_create(1); // 创建主线程自己的epoll实例
    struct epoll_event event;
    event.events = EPOLLIN | EPOLLET; // 主套接字边缘模式
    event.data.fd = mainSocket;
    epoll_ctl(epfd, EPOLL_CTL_ADD, mainSocket, &event);
    struct epoll_event readSockets[1]; // 只监听自己
    int waitRet;
    int i = 0; // 记录把套接字分配给指定线程
    while (1)
    {
        cout << this_thread::get_id() << "主线程等待连接" << endl;
        waitRet = epoll_wait(epfd, readSockets, 1, -1); //
        if (waitRet == -1 && errno != EINTR)
        {
            DEBUG_INFO(strerror(errno));
            exit(EXIT_FAILURE);
        }
        else if (waitRet > 0) // 当检测到可读时 可能是主套接字也可能是子套接字 两种处理逻辑
        {
            cout << "有客户连接把任务分配给" << instances[i] << endl;
            newSfd = tcpObj.myAccept(mainSocket, &peerAddr);
            event.events = EPOLLIN; // 子套接字默认水平模式
            event.data.fd = newSfd;
            tcpObj.setNonblocking(newSfd);
            epoll_ctl(instances[i], EPOLL_CTL_ADD, newSfd, &event);
            i = (i + 1) % SUB_THREAD_COUNT; // 简陋版 负载均衡?
        }
        fprintf(stderr, "----------------------------------------------------\n");
    }
}

void subThread(int instanceId)
{
    Transfer transfer;
    struct epoll_event event;
    struct epoll_event readSockets[THREAD_CONNECTION]; //
    int waitRet;
    int ret;
    while (1)
    {
        cout << this_thread::get_id() << "子线程等待连接" << endl;
        waitRet = epoll_wait(instanceId, readSockets, THREAD_CONNECTION, -1); //
        if (waitRet < 0 && errno != EINTR)
        {
            DEBUG_INFO(strerror(errno));
            exit(EXIT_FAILURE);
        }
        else if (waitRet > 0) // 当检测到可读时 可能是主套接字也可能是子套接字 两种处理逻辑
        {
            for (int i = 0; i < waitRet; i++)
            {
                if ((readSockets[i].events & EPOLLIN))
                {
                    cout << this_thread::get_id() << "有客户请求子线程" << endl;
                    ret = transfer.serverDownload(readSockets[i].data.fd);
                    if (ret == 2) // 说明客户要求下载 服务器转为上传逻辑 修改监听事件为写
                    {
                        event.events = EPOLLOUT; // 设置为水平模式
                        event.data.fd = readSockets[i].data.fd;
                        epoll_ctl(instanceId, EPOLL_CTL_MOD, readSockets[i].data.fd, &event);
                    }
                    else if (ret == 0)
                    {
                        epoll_ctl(instanceId, EPOLL_CTL_DEL, readSockets[i].data.fd, NULL);
                        close(readSockets[i].data.fd);
                    }
                }
                else if ((readSockets[i].events & EPOLLOUT))
                {
                    cout << this_thread::get_id() << "处理客户下载请求" << endl;
                    ret = transfer.serverUpload(readSockets[i].data.fd);
                    if (ret == 0 || ret == -2)
                    {
                        epoll_ctl(instanceId, EPOLL_CTL_DEL, readSockets[i].data.fd, NULL); // 第四个参数不重要
                        close(readSockets[i].data.fd);
                    }
                }
            }
        }
    }
}