#ifndef SERVER_HEAD_H
#define SERVER_HEAD_H
#include "global.h"
#include <stdio.h>
extern void runServer();
extern void *subThread(void *arg);
#endif