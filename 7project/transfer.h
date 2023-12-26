#ifndef TRANSFER_HEAD_H
#define TRANSFER_HEAD_H
#include "global.h"
#include "tcpSocket.h"
extern int clientUpload(char *pFileName, int pType);
extern int clientDownload(char *pFileName, int pType);
extern int serverUpload(int paraTcpSocket);
extern int serverDownload(int paraTcpSocket);
#endif
