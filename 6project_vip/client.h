#ifndef CLIENT_HEAD_H
#define CLIENT_HEAD_H
#include "global.h"
#include <netinet/in.h>
extern Packet packet;
extern int login();
extern int registration();
extern int loginRegistration();
extern void start();
extern int joinRoom();
extern void* doThread(void* arg);
#endif