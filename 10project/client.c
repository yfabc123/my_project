#include "stdio.h"
int main()
{
    char test[8] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
    int* test_p = (int*)test;
    int a = test_p[0];
    printf("%#x", a);
    return 0;
}