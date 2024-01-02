#include <cstdio>
#include <iostream>
#include <sstream>
#include <string.h>
#include <string>
#include <unistd.h>
using namespace std;
struct Packet
{
    size_t contentLen;  // 实际数据大小这个很关键 因为可能发送的是二进制 不是字符串数据
    int type;           // 业务类型 区分上传还是下载 还是失败
    char content[4096]; //
    string fileName;
};
int main()
{

    char a[32] = {'a', 'b'};
    string filePath = "asef" + string(a); // 拼接路径+文件名

    cout << filePath;

    return 0;
}
