#include "Client.h"
#include "Transfer.h"
#include <iostream>
#include <sstream>
using namespace std;
void Client::runClient()
{
    while (1)
    {
        string input;
        cout << "请输入文件名+空格+业务类型(0上传 1下载):";
        getline(std::cin, input);
        std::stringstream ss(input);
        std::string s1;
        std::string s2;
        ss >> s1;
        ss >> s2;
        if (s1 == "" || s2 == "" || !(s2 == "0" || s2 == "1")) // 不能为空 并且第二个字符串 是0 或者1之间
        {
            continue;
        }
        if (s2 == "0")
        {
            Transfer transfer;
            transfer.clientUpload(s1, 0);
        }
        else if (s2 == "1")
        {
            Transfer transfer;
            transfer.clientDownload(s1, 1);
        }
    }
}
