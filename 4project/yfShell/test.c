#include <stdio.h>
#include <time.h>

void printTime(struct timespec *st_mtim)
{
  //  time_t sec = st_mtim->tv_sec;
    struct tm *timeinfo = localtime(&st_mtim->tv_sec);

    // 将时间信息格式化为字符串
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);

    // 打印格式化后的时间字符串
    printf("Formatted Time: %s\n", buffer);
}

int main()
{
    // 假设有一个 struct timespec 对象 st_mtim
    struct timespec st_mtim;
    st_mtim.tv_sec = time(NULL); // 设置为当前时间

    // 调用函数打印格式化后的时间
    printTime(&st_mtim);

    return 0;
}
