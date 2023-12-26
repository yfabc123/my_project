#include <setjmp.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
/* 协程切换时用于保存主协程和子协程的上下文的数据结构 */
typedef struct _context
{
    jmp_buf mainBuf;
    jmp_buf coBuf;
} Context;
/* 全局对象 */
Context gContext;
/* 子协程恢复函数 */
#define resume()                           \
    do                                     \
    {                                      \
        if (0 == setjmp(gContext.mainBuf)) \
        {                                  \
            longjmp(gContext.coBuf, 1);    \
        }                                  \
    } while (0)
/* 子协程挂起函数 */
#define yield()                           \
    do                                    \
    {                                     \
        if (0 == setjmp(gContext.coBuf))  \
        {                                 \
            longjmp(gContext.mainBuf, 1); \
        }                                 \
    } while (0)
/* 函数指针 */
typedef void (*pf)(void *);
/* 子协程启动函数 */
void startCoroutine(pf func, void *arg)
{
    if (0 == setjmp(gContext.mainBuf)) // 第一次保存
    {
        /* 首次调用setjmp时，返回值为0，开始执行子协程对应函数 */
        func(arg);
    }
    /* longjmp跳转到setjmp时，返回非0，所以直接结束协程启动函数 */
}
/* 子协程执行函数 */
void coroutine_func(void *arg)
{
    while (true)
    {
        /* 循环打印@符号，为了一次显示一个@，将fprintf输出到不带缓冲的stderr */
        printf("\n *****coroutine is working******\n");
        printf("coroutine\n");
        printf("\n *****coroutine is suspending******\n");
    }
        yield();
}
int main()
{
    /* 启动子协程 */
    startCoroutine(coroutine_func, NULL);
    /* 主协程循环 */
    while (true)
    {
        printf("\n *****main is working******\n");
        printf("main\n");
        printf("\n *****main is suspending******\n");
        resume();
    }
    return 0;
}