#ifndef _CMD_HEAD_H_
#define _CMD_HEAD_H_
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct cmdInfo
{
    char cmdName[64];        // 命令名称
    char cmdParaList[3][64]; // 参数列表
    int cmdParaNum;          // 参数个数
} cmdInfo_t;
// 总的入口 接收命令参数
extern void cmdHandle(char *cmd);

/* 命令拆分函数 比如 cp 1.txt 2.txt cp 1 2  gcc 1.c -o
命令包含三个部分
命令名称
参数列表
参数个数
*/
extern void splitCmd(char *cmd, cmdInfo_t *cmdInfo);

// 命令分发模块 根据命令名称分给不同的函数处理  cp 调用cp函数 ls 调用 ls函数
extern void cmdDistribute(cmdInfo_t *cmdInfo);

#endif