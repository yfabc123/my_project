#include "cmd.h"
#include "cp.h"
#include "ls.h"

void cmdHandle(char *cmd)
{
    cmdInfo_t *cmdInfo = (cmdInfo_t *)malloc(sizeof(cmdInfo_t));
    memset(cmdInfo, 0, sizeof(cmdInfo_t));

    splitCmd(cmd, cmdInfo);

    cmdDistribute(cmdInfo);
}

void splitCmd(char *cmd, cmdInfo_t *cmdInfo)
{
    char *temp = strtok(cmd, " ");
    if (!temp)
    {
        return;
    }
    strcpy(cmdInfo->cmdName, temp);
    int i = 0;
    while (temp != NULL)
    {
        if (i >= 3)
        {
            //  printf("参数满了\n");
            return;
        }
        temp = strtok(NULL, " ");
        if (temp)
        {
            strcpy(cmdInfo->cmdParaList[i], temp);
            cmdInfo->cmdParaNum++;
        }
        i++;
    }
}

void cmdDistribute(cmdInfo_t *cmdInfo)
{
    if (strcmp(cmdInfo->cmdName, "ls") == 0)
    {
        ls(cmdInfo);
    }
    else if (strcmp(cmdInfo->cmdName, "cp") == 0)
    {
        cp(cmdInfo);
    }
}
