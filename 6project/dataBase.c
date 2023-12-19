#include "dataBase.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "mySocket.h"
MYSQL *mysql = NULL;
MYSQL *connectDB()
{
    mysql = mysql_init(NULL);
    if (mysql == NULL)
    {
        perror("本次初始化异常 MYSQL *mysql = mysql_init(NULL);");
        return NULL;
    }
    mysql_set_character_set(mysql, "utf8");
    mysql = mysql_real_connect(mysql, HOST, USER, PASSWORD, DBNAME, 0, NULL, 0); // 获取具体数据库句柄
    if (mysql == NULL)
    {
        mysql_close(mysql);
        perror("连接数据库异常 mysql_real_connect");
        return NULL;
    }
    return mysql;
}
// 查找用户是否存在 在判断密码是否正确 -1账号错误,-2密码错误,1正确
int checkUser(MYSQL *pmysql, char *puserName, char *ppassWord)
{
    // select *from user where user_name="yefan"
    char buf[64] = {0};          // 账密缓冲区
    MYSQL_RES *sqlResult = NULL; // 存放结果集
    sprintf(buf, "select *from user where user_name=\"%s\"", puserName);
    if (mysql_query(pmysql, buf) != 0)
    {
        printf("本次查询异常\n");
        perror("mysql_query");
        exit(EXIT_FAILURE);
    }
    sqlResult = mysql_store_result(pmysql);
    if (sqlResult->row_count == 0)
    {
        printf("账号不对\n");
        return -1;
    }
    mysql_free_result(sqlResult); // 每次查询完都要释放结果指针
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "select *from user where user_name=\"%s\" and user_password=\"%s\"", puserName, ppassWord);
    if (mysql_query(pmysql, buf) != 0)
    {
        printf("本次查询异常\n");
        perror("mysql_query");
        exit(EXIT_FAILURE);
    }
    sqlResult = mysql_store_result(pmysql);
    if (sqlResult->row_count == 0)
    {
        printf("密码不对\n");
        return -2;
    }
    mysql_free_result(sqlResult); // 每次查询完都要释放结果指针
    printf("账密正确\n");
    return 1;
}
/*
插入用户信息: 先判断户名是否存在 给个提示 账号已存在返回-1 插入成功返回1
 */
int insertUser(MYSQL *paraMysql, char *paraUserName, char *paraPassWord)
{
    char buf[64] = {0};          // 账密缓冲区
    int ret = 1;                 // 结果统一返回
    MYSQL_RES *sqlResult = NULL; // 存放结果集
    sprintf(buf, "select *from user where user_name=\"%s\";", paraUserName);
    if (mysql_query(paraMysql, buf) != 0)
    {
        printf("本次查询异常\n");
        perror("mysql_query");
        exit(EXIT_FAILURE);
    }
    sqlResult = mysql_store_result(paraMysql);
    if (sqlResult->row_count != 0)
    {
        printf("账号已存在\n");
        mysql_free_result(sqlResult); // 每次查询完都要释放结果指针
        return -1;
    }
    mysql_free_result(sqlResult); // 每次查询完都要释放结果指针
    // 插入函数:INSERT INTO `yefan`.`user` (`user_id`, `user_name`, `user_password`) VALUES (1, '叶凡', '叶凡');
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "insert into user (user_name,user_password)\
 VALUES (\"%s\",\"%s\");",
            paraUserName, paraPassWord);
    if (mysql_query(paraMysql, buf) != 0)
    {
        printf("本次执行结果异常\n");
        perror("mysql_query");
        exit(EXIT_FAILURE);
    }
    if (mysql_affected_rows(mysql) < 1)
    {
        printf("插入user失败\n");
        perror("mysql_query(paraMysql, buf)");
        return -1;
    }
    printf("插入用户表成功\n");
    return 1;
}

/*
把聊天信息插入到数据库
 */
int insertMessage(MYSQL *paraMysql, char *paraUserName, char *paraContent, char *ptime)
{
    char buf[1024] = {0}; // 账密缓冲区
    int ret = 1;          // 结果统一返回

    // 插入函数:INSERT INTO `yefan`.`user` (`user_id`, `user_name`, `user_password`) VALUES (1, '叶凡', '叶凡');
    memset(buf, 0, sizeof(buf));
    sprintf(buf, "insert into message (user_name,content,time)\
 VALUES (\"%s\",\"%s\",\"%s\");",
            paraUserName, paraContent, ptime);    
    if (mysql_query(paraMysql, buf) != 0)
    {
        printf("本次执行结果异常\n");
        perror("mysql_query");
        exit(EXIT_FAILURE);
    }
    if (mysql_affected_rows(mysql) < 1)
    {
        printf("插入message失败\n");
        perror("mysql_query(paraMysql, buf)");
        return -1;
    }
    printf("插入信息表成功\n");
    return 1;
}

int getHistoryMessage(MYSQL *pmysql, int threadSocket, Packet *packet)
{
    // select *from user where user_name="yefan"
    char buf[64] = {0};          // 账密缓冲区
    MYSQL_RES *sqlResult = NULL; // 存放结果集
    strcpy(buf, "select user_name,content,time from message;");
    if (mysql_query(pmysql, buf) != 0)
    {
        printf("查询信息表异常\n");
        perror("mysql_query");
        exit(EXIT_FAILURE);
    }
    sqlResult = mysql_store_result(pmysql);
    if (sqlResult->row_count == 0)
    {
        printf("没有数据\n");
        return -1;
    }
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(sqlResult)))
    {
        strcpy(packet->user_name, row[0]);
        strcpy(packet->message, row[1]);
        strcpy(packet->time, row[2]);
        mySendTo(threadSocket, packet, NULL, 0, sizeof(Packet), &packet->addr);
    }
    mysql_free_result(sqlResult); // 每次查询完都要释放结果指针
    return 1;
}