#include "client.h"
#include "transfer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
int runClient()
{
    char buf[1024];
    int ch;
    int ret;
    while (1)
    {
        memset(buf, 0, sizeof(buf));
        fprintf(stderr, "请输入文件名+空格+业务类型(0上传 1下载):");
        fgets(buf, sizeof(buf), stdin);
        buf[strlen(buf) - 1] = 0; // 回车干掉
        if (strlen(buf) == 0)
        {
            DEBUG_INFO("输入格式不对");
            continue;
        }
        char *fileName = strtok(buf, " ");
        char *type = strtok(NULL, " ");
        if (!type)
        {
            DEBUG_INFO("输入格式不对");
            continue;
        }
        if (strcmp(type, "0") == 0)
        {
            printf("%s-%s\n", fileName, type);
            clientUpload(fileName, atoi(type));
        }
        else if (strcmp(type, "1") == 0)
        {
            printf("%s-%s\n", fileName, type);
            clientDownload(fileName, atoi(type));
        }
    }
    return 1;
}
