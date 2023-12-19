#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
void getCurtime(char *paratime)
{
    time_t t;
    int a = 1;
    time(&t);
    char timeBuf[64] = {0};
    struct tm *ptm = localtime(&t);
    sprintf(paratime, "%d-%d-%d %d:%d:%02d", ptm->tm_year + 1900,
            ptm->tm_mon + 1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
}
int main(int argc, char const *argv[])
{
    return 0;
}
