#include <errno.h>
#include <stdio.h>
#include <string.h>


#define DEBUG_INFO(arg) \
    fprintf(stderr, "[%s,%s,%d]:%s\n", __FILE__, __FUNCTION__, __LINE__, arg);

int main()
{

    DEBUG_INFO("有问题");

    return 0;
}
