#include <stdio.h>
#include <string.h>
int main()
{
    char* a = "abcasdf";
    printf("%s",strstr(a, "bc"));

    return 0;
}