#include "cp.h"
#include <dirent.h>
void cp(cmdInfo_t *cmdInfo)
{
    if (cmdInfo->cmdParaNum != 2)
    {
        printf("参数个数不对\n");
        return;
    }
    fileInfo_t *fileInfo = (fileInfo_t *)malloc(sizeof(fileInfo_t));
    memset(fileInfo, 0, sizeof(fileInfo));
    int ret = fileType(cmdInfo->cmdParaList[0]); // 判断源文件类型
    if (ret == -1)
    {
        printf("原文件不存在\n");
        return;
    }
    fileInfo->type = ret;

    if (fileInfo->type == 1)
    {
        if (-1 == fileType(cmdInfo->cmdParaList[1]))
        {
            printf("目标路径不存在\n");
            return;
        }
        cpDir(cmdInfo->cmdParaList[0], cmdInfo->cmdParaList[1]);
    }
    else if (fileInfo->type == 2)
    {
        cpFile(cmdInfo->cmdParaList[0], cmdInfo->cmdParaList[1]);
    }
    else
    {
        printf("源文件未知类型\n");
        return;
    }
}

int fileType(char *pathname)
{
    struct stat statbuf;
    int ret = stat(pathname, &statbuf);
    if (ret != 0)
    {
        return -1; // 不存在
    }
    // 判断文件类型
    if (S_ISDIR(statbuf.st_mode))
    {
        return 1; // 目录类型
    }
    if (S_ISREG(statbuf.st_mode))
    {
        return 2; // 文件类型
    }
    else
    {
        return 3; // 未知类型
    }
    return 0;
}

void cpFile(char *source, char *target)
{
    char srcName[64] = {0};
    strcpy(srcName, getFileName(source)); // 获取到源文件名
    char targetName[64] = {0};            // 存放拼接后的完整路径文件名 因为存在多种场景:指定文件名 指定目录等
    strcpy(targetName, target);
    int type = fileType(target);
    FILE *writeFile;
    FILE *readFile;
    if (type == -1) // 场景1 复制时指定文件名 结果肯定是不存在的 所以type=-1
    {
    }
    else if (type == 1) // 场景2 不指定文件名 需拼接完整路径
    {
        strcat(targetName, "/");
        strcat(targetName, srcName);
    }

    // 开始写数据
    readFile = fopen(source, "r");
    writeFile = fopen(targetName, "w+");
    char buf[1024] = {0};
    while (1)
    {
        size_t rbyte = fread(buf, 1, sizeof(buf), readFile);
        if (rbyte == 0)
        {
            break;
        }
        size_t wbyte = fwrite(buf, 1, rbyte, writeFile); // 注意读多少写多少!
    }
    fclose(readFile);
    fclose(writeFile);
}

//    cp  /a: /a/1.c  /a/a1/1.c     /b
void cpDir(char *source, char *target)
{
    DIR *Dir = opendir(source);
    if (!Dir)
    {
        return;
    }
    char sourceName[64] = {0};               // 当前目录名
    strcpy(sourceName, getFileName(source)); // 获取源目录名  

    char subName[64] = {0}; // 当前目录相对源目录名
    strcpy(subName, source);
    strcat(subName, "/");

    char subTarget[64] = {0}; // 目标子目录
    strcpy(subTarget, target);
    strcat(subTarget, "/");
    strcat(subTarget, sourceName);
    mkdir(subTarget, 0777); // 在目标目录创建子目录

    struct dirent *dir;
    while (dir = readdir(Dir))
    {
        if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0)
        {
            continue;
        }
        char temp[64] = {0};
        strcpy(temp, subName);
        strcat(temp, dir->d_name);
        if (dir->d_type == DT_REG) // 文件
        {
            printf("复制文件:文件:%s 目标:%s\n", temp, subTarget);
            cpFile(temp, subTarget);
        }
        else if (dir->d_type == DT_DIR) // 目录
        {
            printf("复制目录:文件:%s 目标:%s\n", temp, subTarget);
            cpDir(temp, subTarget);
        }
    }
}

// 获取文件名 gpt提供
char *getFileName(char *path)
{
    char *filename = strrchr(path, '/');
    if (filename == NULL)
    {
        filename = path; // 没有斜杠，整个路径就是文件名
    }
    else
    {
        filename++; // 跳过斜杠
    }
    return filename;
}