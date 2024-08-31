#ifndef DATABASE_HEAD_H
#define DATABASE_HEAD_H
#include "global.h"
#include <mysql/mysql.h>


extern pthread_mutex_t mysql_mutex;
extern MYSQL* mysql;
extern MYSQL* connectDB();
extern int checkUser(MYSQL* pmysql, char* puserName, char* ppassWord);
extern int insertUser(MYSQL* paraMysql, char* paraUserName, char* paraPassWord);
extern int insertMessage(MYSQL* paraMysql, char* paraUserName, char* paraContent, char* ptime);
extern int getHistoryMessage(MYSQL* pmysql, int threadSocket, Packet* packet);
#endif
