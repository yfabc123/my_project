#ifndef CLIENT_HEAD_H
#define CLIENT_HEAD_H
#include <netinet/in.h>
#define SERVERIP "192.168.2.4"
#define SERVERPORT 11
#include "global.h"



extern int login();
extern int registration();
extern int loginRegistration();
extern void start();
extern int joinRoom();
extern void *doThread(void *arg);
#endif