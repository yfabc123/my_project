#include <stdio.h>
#include <setjmp.h>
#include <unistd.h>
jmp_buf env;
void fun()
{
    printf("插入一个数据\n");
    longjmp(env, 4);
}
int main()
{
    for (int i = 0; i < 5; i++)
    {
        printf("%d\n", i);
        sleep(1);
        if (i == 1)
        {
            setjmp(env);
            fun();
        }
    }
    return 0;
}
