#ifndef Transfer_HEAD_H
#define Transfer_HEAD_H
#include <iostream>
using namespace std;
class Transfer
{
public:
    int clientUpload(string &FileName, int pType);
    int clientDownload(string &FileName, int pType);
    int serverUpload(int paraTcpSocket);
    int serverDownload(int paraTcpSocket);
};
#endif