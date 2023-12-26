#include "transfer.h"
#include "global.h"
#include "tcpSocket.h"
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
// 重新改造函数 使用定长方式发送
/*
发送时客户端发送文件名
返回值: -1 失败
 */
int clientUpload(char *pFileName, int pType)
{
    char tempFileName[128] = {0};
    strcpy(tempFileName, pFileName);
    Packet packet; // 每次发送的数据包
    int ret;
    char filePath[128] = {0}; // 存放绝对路径
    strcpy(filePath, CLIENT_FILE_PATH);
    strcat(filePath, tempFileName);

    FILE *file = fopen(filePath, "rb");
    if (!file)
    {
        DEBUG_INFO("文件不存在");
        return -1;
    }

    int tcpSocket = myCreateTcpSocket();
    ret = myConnect(tcpSocket, SERVERIP, SERVERPORT, NULL);
    if (ret < 0)
    {
        DEBUG_INFO(strerror(errno));
        exit(EXIT_FAILURE);
    }

    size_t rbyte;   // 本次读取的字节数
    size_t sbyte;   // 本次发送字节数
    size_t sum = 0; // 计算累计发送的字节

    // 循环读取本地文件到缓冲区然后send
    while (1)
    {
        // 重置之前数据
        memset(&packet, 0, sizeof(Packet));
        packet.type = pType;                                            // /给小包赋值业务类型
        strcpy(packet.fileName, tempFileName);                          // 给小包赋值文件名,发送给服务器是去掉路径的文件名
        rbyte = fread(packet.content, 1, sizeof(packet.content), file); // 把内容写到小包内部 同时需要把 实际读取的文件大小发送
        packet.contentLen = rbyte;
        sum += rbyte;
        if (rbyte == 0)
        {
            DEBUG_INFO(strerror(errno));
            break;
        }
        sbyte = mySend(tcpSocket, &packet, sizeof(Packet), MSG_NOSIGNAL);
        printf("客户端累计发送字节:%ld\n", sum);
        if (sbyte < sizeof(Packet))
        {
            DEBUG_INFO(strerror(errno));
            return -1;
        }
    }
    close(tcpSocket);
    fclose(file);

    return 1;
}
// 下载默认只从服务器的指定目录获取文件并且下载到指定客户端目录
int clientDownload(char *pFileName, int pType)
{
    Packet packet; // 每次发送的数据包
    memset(&packet, 0, sizeof(Packet));
    strcpy(packet.fileName, pFileName); // 文件名
    packet.type = pType;                // 业务类型
    int ret;
    FILE *file = NULL;
    int tcpSocket = myCreateTcpSocket();
    ret = myConnect(tcpSocket, SERVERIP, SERVERPORT, NULL);
    if (ret < 0)
    {
        DEBUG_INFO(strerror(errno));
        exit(EXIT_FAILURE);
    }
    size_t wbyte;       // 写入到文件的字节数
    size_t rbyte;       // 收到的字节数
    size_t sbyte;       // 发送的字节数
    size_t curByte = 0; // 定义本次收到的包大小
    sbyte = mySend(tcpSocket, &packet, sizeof(packet), MSG_NOSIGNAL);
    if (sbyte < sizeof(packet))
    {
        DEBUG_INFO(strerror(errno));
        return -1;
    }
    size_t sum = 0;
    while (1)
    {
        memset(&packet, 0, sizeof(Packet));
        ret = myUpgradeRecv(tcpSocket, &packet, sizeof(Packet), MSG_NOSIGNAL);
        if (ret == -1)
        {
            DEBUG_INFO(strerror(errno));
            break;
        }
        if (ret == 0)
        {
            DEBUG_INFO(strerror(errno));
            break;
        }

        if (!file) // 包头数据长度如果是0表示服务器没有该文件
        {
            if (packet.type == 2)
            {
                DEBUG_INFO("下载失败服务器没有该文件");
                return 1;
            }
            else
            {
                // 文件默认下载到客户端文件夹
                char path[1024] = {0};
                strcpy(path, CLIENT_FILE_PATH);
                strcat(path, packet.fileName);
                file = fopen(path, "ab+");
                if (!file)
                {
                    DEBUG_INFO(strerror(errno));
                    break;
                }
            }
        }
        wbyte = fwrite(packet.content, 1, packet.contentLen, file); // 客户端按照服务器发送长度写入
        sum += wbyte;
        printf("客户端累计接收字节:%ld\n", sum);
        if (wbyte < packet.contentLen)
        {
            DEBUG_INFO(strerror(errno));
            exit(EXIT_FAILURE);
        }
    }
    fclose(file);
    return 1;
}

/*  服务器上传逻辑一次只发送一个包
返回值: -2 表示对端关闭 -1表示本次发送失败下次继续  0 发送结束  1 表示本次发送成功
 */
int serverUpload(int paraTcpSocket)
{
    printf("%ld处理客户上传请求\n", pthread_self());

    int tempIndex = findHash(paraTcpSocket);
    Packet packet;
    if (tempIndex < 0)
    {
        DEBUG_INFO("没找到自己?");
        exit(EXIT_FAILURE);
    }
    int tempOffset = socketInfo[tempIndex]->offset;       // 获取索引值
    char *tempFileName = socketInfo[tempIndex]->fileName; // 获取文件名( serverDownload替咱插入到哈希表的 得感谢人家)
    char path[128] = {0};
    strcpy(path, SERVER_FILE_PATH);
    strcat(path, tempFileName);
    FILE *file = fopen(path, "rb");
    if (!file)
    {
        packet.type = 2;
        mySend(paraTcpSocket, &packet, sizeof(Packet), MSG_NOSIGNAL); // 告诉客户端没这个文件.
        DEBUG_INFO(strerror(errno));
    }
    fseek(file, tempOffset, SEEK_SET);
    memset(&packet, 0, sizeof(Packet));

    strcpy(packet.fileName, tempFileName);
    size_t rbyte = fread(packet.content, 1, sizeof(packet.content), file);
    if (rbyte == -1)
    {
        DEBUG_INFO(strerror(errno));
        exit(EXIT_FAILURE);
    }
    else if (rbyte == 0)
    {
        fclose(file);
        DEBUG_INFO(strerror(errno));
        DEBUG_INFO("服务器发送结束关闭套接字\n");
        freeHash(paraTcpSocket);
        return 0;
    }
    packet.contentLen = rbyte;

    size_t sbyte = mySend(paraTcpSocket, &packet, sizeof(packet), MSG_NOSIGNAL);

    if (-1 == sbyte && (errno == EAGAIN || errno == EWOULDBLOCK)) // 下次继续发送
    {
        fclose(file);
        DEBUG_INFO(strerror(errno));
        return 1;
    }
    else if (-1 == sbyte) // 其他失败情况 比如对端关闭连接   从队列删除并关闭套接字
    {
        freeHash(paraTcpSocket);
        fclose(file);
        DEBUG_INFO("关闭套接字");
        return -2;
    }
    else if (sbyte < sizeof(packet)) // 发送数据包不完整 直接挂掉
    {
        DEBUG_INFO("包不完整");
        DEBUG_INFO(strerror(errno));
        fclose(file);
        exit(EXIT_FAILURE);
    }
    // 注意把实际读取的数据更新哈希表同时更新 结构体中记录本次发送数据长度
    updateHash(paraTcpSocket, tempOffset + rbyte);
    printf("服务器累计发送字节:%ld\n", tempOffset + rbyte);
    fclose(file);
    return 1;
}
/* 服务器下载逻辑一次只读取一个包
返回值:-2 服务器写入异常
 -1本次读取异常下次重试
 0对端关闭连接
 1本次发送成功
 2表示转到上传逻辑处理修改监控事件
  发送失败 */
int serverDownload(int paraTcpSocket)
{
    Packet packet;
    size_t wbyte = 0;
    size_t rbyte = 0;
    int ret = 0;
    memset(&packet, 0, sizeof(Packet));
    ret = myUpgradeRecv(paraTcpSocket, &packet, sizeof(Packet), MSG_NOSIGNAL);
    if (-1 == ret) // 本次接收失败下次继续
    {
        return -1;
    }
    else if (0 == ret)
    {
        DEBUG_INFO("对端关闭连接");
        DEBUG_INFO(strerror(errno));
        freeHash(paraTcpSocket); // 释放哈希
        return 0;
    }
    else if (-2 == ret) // 其他错误情况
    {
        DEBUG_INFO(strerror(errno));
        close(paraTcpSocket);
        exit(EXIT_FAILURE);
    }
    /*
    收到请求发现是客户端下载逻辑 将文件名还有初始偏移量插入哈希表然后直接返回,
    另外如果是下载逻辑也插入数据便于输出字节数 不返回继续处理
     */
    if (findHash(paraTcpSocket) == -1) // 哈希表没数据说明第一次请求,插入
    {
        ret = insertHash(paraTcpSocket, packet.fileName); // 将文件信息插入哈希
        if (ret == -1)
        {
            DEBUG_INFO(strerror(errno));
            exit(EXIT_FAILURE);
        }
        if (packet.type == 1) // 客户端要求下载文件 转到下载逻辑,
        {
            return 2;
        }
    }
    printf("%ld处理客户下载请求\n", pthread_self());

    int tempIndex = findHash(paraTcpSocket);
    if (tempIndex < 0)
    {
        DEBUG_INFO(strerror(errno));
        DEBUG_INFO("为什么没找到哈希?");
        exit(EXIT_FAILURE);
    }
    int tempOffset = socketInfo[tempIndex]->offset;
    char path[128] = {0};
    strcpy(path, SERVER_FILE_PATH);
    //   printf("收到的文件名:%s 哈希表文件名:%s\n", packet.fileName, socketInfo[findHash(paraTcpSocket)]->fileName); // 不知道为什么客户端传输的文件名会缺失
    strcat(path, packet.fileName);
    FILE *file = fopen(path, "ab+");
    if (!file)
    {
        DEBUG_INFO(strerror(errno));
        exit(EXIT_FAILURE);
    }
    wbyte = fwrite(packet.content, 1, packet.contentLen, file);
    if (-1 == wbyte)
    {
        DEBUG_INFO(strerror(errno));
        exit(EXIT_FAILURE);
    }
    else if (packet.contentLen != wbyte)
    {
        DEBUG_INFO("数据写少了??? ");
        exit(EXIT_FAILURE);
    }
    updateHash(paraTcpSocket, tempOffset + packet.contentLen);
    printf("服务器累计接收字节:%ld\n", tempOffset + packet.contentLen);
    fclose(file);
    return 1;
}