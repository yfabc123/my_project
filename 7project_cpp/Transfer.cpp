#include "Transfer.h"
#include "Global.h"
#include "TcpSocket.h"
#include <cstring>
#include <fstream>
#include <thread>
#include <unistd.h>
int Transfer::clientUpload(string &pFileName, int pType)
{
    Transfer transfer;
    TcpSocket tcpObj;
    Packet packet; // 每次发送的数据包
    int ret;
    string filePath = CLIENT_FILE_PATH + pFileName; // 用户上传的是文件名,我们内部处理+路径
    ifstream file(filePath, ios::in | ios::binary); // 二进制读方式打开文件 对象
    if (!file.is_open())
    {
        cout << "文件不存在" << endl;
        return -1;
    }
    int tcpSocket = tcpObj.myCreateTcpSocket(); // 让tcp对象创建一个套接字..
    if (tcpSocket <= 0)
    {
        DEBUG_INFO(strerror(errno));
        exit(EXIT_FAILURE);
    }
    ret = tcpObj.myConnect(tcpSocket, SERVERIP, SERVERPORT, NULL);
    if (ret < 0)
    {
        DEBUG_INFO(strerror(errno));
        exit(EXIT_FAILURE);
    }
    size_t rbyte;        // 本次读取的字节数
    size_t sbyte;        // 本次发送字节数
    size_t sum = 0;      // 计算累计发送的字节
    packet.type = pType; // /给小包赋值业务类型
    strcpy(packet.fileName, pFileName.c_str());

    // 循环读取本地文件到缓冲区然后send
    while (1)
    {
        // 重置之前数据
        memset(packet.content, 0, sizeof(packet.content));
        file.read(packet.content, 4096);   // 读取到结构体数据区中
        packet.contentLen = file.gcount(); // 获取上次读取长度 并赋值给数据包
        sum += file.gcount();              // 获取每次读取数据包
        if (file.gcount() == 0)
        {
            DEBUG_INFO("客户端文件发送完毕了");
            break;
        }
        sbyte = tcpObj.mySend(tcpSocket, &packet, sizeof(Packet), MSG_NOSIGNAL);
        cout << "客户端累计发送字节" << sum << endl;

        if (sbyte < sizeof(Packet))
        {
            DEBUG_INFO(strerror(errno));
            return -1;
        }
    }
    close(tcpSocket);
    file.close(); // 关闭文件
    return 1;
}
int Transfer::clientDownload(string &pFileName, int pType)
{
    Transfer transfer;
    TcpSocket tcpObj;
    Packet packet;
    strcpy(packet.fileName, pFileName.c_str());
    packet.type = pType; // 业务类型
    int ret;
    string filePath = CLIENT_FILE_PATH + pFileName;  // 拼接路径+文件名
    ofstream file(filePath, ios::out | ios::binary); // 二进制读方式打开文件 对象
    if (!file.is_open())
    {
        cout << "打开失败" << endl;
        return -1;
    }
    int tcpSocket = tcpObj.myCreateTcpSocket();
    ret = tcpObj.myConnect(tcpSocket, SERVERIP, SERVERPORT, NULL);
    if (ret < 0)
    {
        DEBUG_INFO(strerror(errno));
        exit(EXIT_FAILURE);
    }
    size_t wbyte;                                                            // 写入到文件的字节数
    size_t rbyte;                                                            // 收到的字节数
    size_t sbyte;                                                            // 发送的字节数
    size_t curByte = 0;                                                      // 定义本次收到的包大小
    size_t sum = 0;                                                          // 累计接收字节
    sbyte = tcpObj.mySend(tcpSocket, &packet, sizeof(packet), MSG_NOSIGNAL); // 第一次先发送下载请求
    if (sbyte < sizeof(packet))
    {
        DEBUG_INFO(strerror(errno));
        return -1;
    }
    while (1)
    {
        memset(&packet, 0, sizeof(Packet));
        ret = tcpObj.myUpgradeRecv(tcpSocket, &packet, sizeof(Packet), MSG_NOSIGNAL); // 非粘包接收数据
        if (ret == -2 || ret == 0)                                                    // 阻塞模式底层返回-1 出问题了
        {
            DEBUG_INFO(strerror(errno));
            break;
        }
        if (packet.type == 2)
        {
            DEBUG_INFO("下载失败服务器没有该文件");
            break;
        }
        streampos beforePosition = file.tellp();                  // 之前偏移量
        file.write(packet.content, packet.contentLen);            // 客户端按照服务器发送长度写入
        streampos currentPosition = file.tellp();                 // 本次偏移量
        if (currentPosition - beforePosition < packet.contentLen) // 本次写入的长度如果比应该写入的字节少 则挂掉
        {
            DEBUG_INFO(strerror(errno));
            exit(EXIT_FAILURE);
        }
        sum = currentPosition;
        cout << "客户端累计接收字节" << sum << endl;
    }
    close(tcpSocket); // 关闭套接字
    file.close();     // 关闭文件
    return 1;
}
int Transfer::serverUpload(int paraTcpSocket)
{
    Transfer transfer;
    TcpSocket tcpObj;
    Global global;
    cout << this_thread::get_id() << "处理客户上传请求" << endl;
    int tempIndex = global.findHash(paraTcpSocket);
    Packet packet;
    if (tempIndex < 0)
    {
        DEBUG_INFO("没找到自己的 怎么费斯");
        exit(EXIT_FAILURE);
    }
    int tempOffset = Global::socketInfo[tempIndex]->offset;        // 获取偏移量
    string tempFileName = Global::socketInfo[tempIndex]->fileName; // 获取文件名( serverDownload替咱插入到哈希表的 得感谢人家)
    string filePath = SERVER_FILE_PATH + tempFileName;             // 拼接完整路径名
    ifstream file(filePath, ios::in | ios::binary);                // 二进制读方式打开文件 对象
    if (!file.is_open())                                           // 如果没有这个文件
    {
        cout << "文件不存在" << endl;
        packet.type = 2;
        tcpObj.mySend(paraTcpSocket, &packet, sizeof(Packet), MSG_NOSIGNAL); // 告诉客户端没这个文件.
        return -1;
    }
    file.seekg(tempOffset, std::ios::beg); // 设置偏移量,上次读取的位置
    memset(packet.content, 0, sizeof(packet.content));

    strcpy(packet.fileName, tempFileName.c_str()); // 文件名传给数据包

    file.read(packet.content, 4096); // 读取到结构体数据区中
    size_t rbyte = file.gcount();    // 本次实际读取长度
    packet.contentLen = rbyte;       // 实际读取长度
    if (rbyte == 0)
    {
        file.close();
        DEBUG_INFO("服务器发送结束关闭套接字\n");
        global.freeHash(paraTcpSocket);
        return 0;
    }
    size_t sbyte = tcpObj.mySend(paraTcpSocket, &packet, sizeof(packet), MSG_NOSIGNAL);
    if (-1 == sbyte && (errno == EAGAIN || errno == EWOULDBLOCK)) // 下次继续发送
    {
        file.close();
        DEBUG_INFO(strerror(errno));
        return 1;
    }
    else if (-1 == sbyte) // 其他失败情况 比如对端关闭连接   从队列删除并关闭套接字
    {
        global.freeHash(paraTcpSocket);
        file.close();
        DEBUG_INFO("关闭套接字");
        return -2;
    }
    else if (sbyte < sizeof(packet)) // 发送数据包不完整 直接挂掉
    {
        DEBUG_INFO(strerror(errno));
        file.close();
        exit(EXIT_FAILURE);
    }
    // 注意把实际读取的数据更新哈希表同时更新 结构体中记录本次发送数据长度
    global.updateHash(paraTcpSocket, tempOffset + rbyte);
    cout << "服务器累计发送字节" << tempOffset + rbyte << endl;
    file.close();
    return 1;
}
int Transfer::serverDownload(int paraTcpSocket)
{
    Packet packet;
    Transfer transfer;
    TcpSocket tcpObj;
    Global global;
    size_t wbyte = 0;
    size_t rbyte = 0;
    int ret = 0;
    //  memset(&packet, 0, sizeof(Packet));
    ret = tcpObj.myUpgradeRecv(paraTcpSocket, &packet, sizeof(Packet), MSG_NOSIGNAL);
    cout << packet.fileName << endl;
    if (-1 == ret) // 本次接收失败下次继续
    {
        return -1;
    }
    else if (0 == ret)
    {
        DEBUG_INFO("对端关闭连接");
        global.freeHash(paraTcpSocket); // 释放哈希
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
    if (global.findHash(paraTcpSocket) == -1) // 哈希表没数据说明第一次请求,插入
    {
        ret = global.insertHash(paraTcpSocket, packet.fileName); // 将文件信息插入哈希
        if (ret == -1)                                           // 插入哈希失败了 满了?
        {
            DEBUG_INFO(strerror(errno));
            exit(EXIT_FAILURE);
        }
        if (packet.type == 1) // 客户端请求类型是下载 转到服务器上传逻辑,
        {
            return 2;
        }
    }
    cout << this_thread::get_id() << "处理客户下载请求" << endl;
    int tempIndex = global.findHash(paraTcpSocket);
    if (tempIndex == -1)
    {
        DEBUG_INFO("为什么没找到哈希?");
        exit(EXIT_FAILURE);
    }
    // 打开文件
    string filePath = SERVER_FILE_PATH + string(packet.fileName); // 拼接路径+文件名
    ofstream file(filePath, ios::out | ios::binary | ios::app);   // 二进制追加写入方式打开文件 对象
    if (!file.is_open())
    {
        cout << "打开失败" << endl;
        return -1;
    }
    if (!file)
    {
        DEBUG_INFO(strerror(errno));
        exit(EXIT_FAILURE);
    }
    streampos beforePosition = global.socketInfo[tempIndex]->offset; // 获取之前偏移量 没啥意义主要是为了输出字节
    file.write(packet.content, packet.contentLen);                   // 客户端按照服务器发送长度写入
    streampos currentPosition = file.tellp();                        // 本次偏移量
    wbyte = currentPosition - beforePosition;                        // 本次写入数据
    if (wbyte < packet.contentLen)                                   // 本次写入的长度如果比应该写入的字节少 则挂掉
    {
        DEBUG_INFO(strerror(errno));
        exit(EXIT_FAILURE);
    }
    if (packet.contentLen != wbyte)
    {
        DEBUG_INFO("数据写少了");
        exit(EXIT_FAILURE);
    }
    global.updateHash(paraTcpSocket, global.socketInfo[tempIndex]->offset + packet.contentLen);
    cout << "服务器累计接收字节" << global.socketInfo[tempIndex]->offset << endl;
    file.close();
    return 1;
}