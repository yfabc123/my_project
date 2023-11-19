#ifndef _CP_HEAD_H_
#define _CP_HEAD_H_

#include "cmd.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
// 文件信息结构体

typedef struct fileInfo
{
    int type;
    char source[64];
    char target[64];

} fileInfo_t;
// cp模块入口
extern void cp(cmdInfo_t *cmdInfo);

extern int fileType(char *pathname);

// 判断文件类型 复制文件和复制路径 两种逻辑

// 复制文件
extern void cpDir(char *source, char *target);

extern void cpFile(char *source, char *target);

// 获取到源路径的文件名
extern char *getFileName(char *path);
// 文件路径拼接
#endif