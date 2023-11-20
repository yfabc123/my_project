#include "ls.h"
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <stdio.h>
#include <unistd.h>
// ls 可能有多种形式 ls -a  ls -l 需增加判断
void ls(cmdInfo_t *cmdInfo)
{
    if (0 == strcmp(cmdInfo->cmdParaList[0], "-l"))
    {
        lsL(cmdInfo);
    }
}
// 处理模块  两种 带路径或者不带  ls -l   ls -l ../
void lsL(cmdInfo_t *cmdInfo)
{
    lsFileInfo_t *fileInfo = (lsFileInfo_t *)malloc(sizeof(lsFileInfo_t));
    memset(fileInfo, 0, sizeof(fileInfo));
    // 不带路径
    // 遍历目录
    DIR *Dir;
    char parDir[64] = {0}; // 父目录
    char subDir[64] = {0};

    if (1 == cmdInfo->cmdParaNum)
    {
        Dir = opendir(".");
        strcpy(parDir, ".");
    }
    else
    {
        strcpy(parDir, cmdInfo->cmdParaList[1]);
        Dir = opendir(cmdInfo->cmdParaList[1]);
    }

    struct dirent *dir;
    struct stat statBuf;

    if (NULL == Dir)
    {
        perror("opendir");
        return;
    }
    while (1) // 路径每次要重置一下
    {
        strcpy(subDir, parDir);
        strcat(subDir, "/");
        dir = readdir(Dir);
        int ret;
        if (NULL == dir)
        {
           // perror("readdir");
            return;
        }
        if (0 == strcmp(dir->d_name, ".") || 0 == strcmp(dir->d_name, ".."))
        {
            continue;
        }
        strcat(subDir, dir->d_name); //  /parDir/subDir

        if (dir->d_type == DT_LNK) // 链接文件需要单独处理下
        {

            // 拼接名称按照 soft_link_mydisk -> /mnt/mydisk
            strcpy(fileInfo->fileName, dir->d_name);
            strcat(fileInfo->fileName, " -> ");
            char tempBuf[64] = {0};
            size_t len = readlink(subDir, tempBuf, sizeof(fileInfo->fileName));
            if (-1 == len)
            {
                //perror("readlink");
                return;
            }
            tempBuf[len] = '\0';
            strcat(fileInfo->fileName, tempBuf);
            ret = lstat(subDir, &statBuf);
        }
        else
        {
            strcpy(fileInfo->fileName, dir->d_name);
            ret = stat(subDir, &statBuf);
        }
        if (0 != ret)
        {
            perror("stat");
            return;
        }
        fileInfo->fileType = lsFileType(statBuf.st_mode);
        filePermissions(fileInfo, statBuf.st_mode);
        fileInfo->hardLinkNUm = statBuf.st_nlink;
        userGroupName(fileInfo, statBuf.st_uid, statBuf.st_gid);
        fileInfo->fileSize = statBuf.st_size;
        lastUpdatTime(fileInfo, &statBuf.st_mtim);
        show(fileInfo);
    }
    closedir(Dir);
}

// 输出数组模块
void show(lsFileInfo_t *fileInfo)
{
    printf("%c", fileInfo->fileType);
    printf("%s", fileInfo->filePermissions);
    printf("%4d", fileInfo->hardLinkNUm);
    printf("%6s", fileInfo->userName);
    printf("%s", fileInfo->groupName);
    printf("%10d ", fileInfo->fileSize);
    printf("%s ", fileInfo->lastUpdatTime);
    printf("%s\n", fileInfo->fileName);
}

// 判断文件类型
char lsFileType(mode_t mode)
{

    if (S_ISREG(mode))
    {
        return '-'; // 普通文件
    }
    else if (S_ISDIR(mode))
    {
        return 'd'; // 目录
    }
    else if (S_ISLNK(mode))
    {
        return 'l'; // 符号链接
    }
    else if (S_ISFIFO(mode))
    {
        return 'p'; // 命名管道（FIFO）
    }
    else if (S_ISCHR(mode))
    {
        return 'c'; // 字符设备
    }
    else if (S_ISBLK(mode))
    {
        return 'b'; // 块设备
    }
    else if (S_ISSOCK(mode))
    {
        return 's'; // 套接字
    }
    else
    {
        putchar('?'); // 未知类型
    }
}

void filePermissions(lsFileInfo_t *fileInfo, mode_t mode)
{

    int rwx = 0777; // 定义一个8进制数据 最高执行权限
    char list[3] = {'r', 'w', 'x'};
    int result = rwx & mode;
    for (int i = 8; i >= 0; i--)
    {
        int num = (result >> i) & 1;
        if (1 == num)
        {
            fileInfo->filePermissions[8 - i] = list[(8 - i) % 3];
        }
        else
        {
            fileInfo->filePermissions[8 - i] = '-';
        }
    }
    fileInfo->filePermissions[9] = '\0';
}

void userGroupName(lsFileInfo_t *fileInfo, uid_t st_uid, gid_t st_gid)
{
    struct passwd *pw = getpwuid(st_uid);
    strcpy(fileInfo->userName, pw->pw_name);

    struct group *grp = getgrgid(st_gid);
    strcpy(fileInfo->groupName, grp->gr_name);
}

void lastUpdatTime(lsFileInfo_t *fileInfo, struct timespec *st_mtim)
{
    struct tm *t = localtime(&st_mtim->tv_sec);

    strftime(fileInfo->lastUpdatTime, sizeof(fileInfo->lastUpdatTime), "%b %d %M:%S", t);
}