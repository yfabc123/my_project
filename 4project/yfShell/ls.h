#ifndef LS_HEAD_H
#define LS_HEAD_H
#include "cmd.h"
typedef struct lsFileInfo
{
    char fileType;
    char filePermissions[10];
    int hardLinkNUm;
    char userName[64];
    char groupName[64];
    int fileSize;
    char lastUpdatTime[64];
    char fileName[64];
} lsFileInfo_t;

extern void ls(cmdInfo_t *cmdInfo);

extern void show(lsFileInfo_t *fileInfo);
extern char lsFileType(mode_t mode);
extern void lsL(cmdInfo_t *cmdInfo);
extern void filePermissions(lsFileInfo_t *fileInfo, mode_t mode);
extern void userGroupName(lsFileInfo_t *lsFileInfo, uid_t st_uid, gid_t st_gid);
extern void lastUpdatTime(lsFileInfo_t *fileInfo, struct timespec *st_mtim);
#endif