#include "cmd.h"
#include "cp.h"
int main()
{
    char cmdBuf[1024];

    while (1)
    {
        printf("yfShell:");
        fgets(cmdBuf, sizeof(cmdBuf), stdin);
        cmdBuf[strlen(cmdBuf) - 1] = '\0';
        cmdHandle(cmdBuf);
    }
    return 0;
}