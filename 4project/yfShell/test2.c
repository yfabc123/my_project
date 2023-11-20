

#include <stdio.h>
#include <sys/stat.h>

void print_file_type(mode_t mode)
{
    if (S_ISREG(mode))
    {
        putchar('-'); // 普通文件
    }
    else if (S_ISDIR(mode))
    {
        putchar('d'); // 目录
    }
    else if (__S_IFLNK & mode)
    {
        putchar('l'); // 符号链接
    }
    else if (S_ISFIFO(mode))
    {
        putchar('p'); // 命名管道（FIFO）
    }
    else if (S_ISCHR(mode))
    {
        putchar('c'); // 字符设备
    }
    else if (S_ISBLK(mode))
    {
        putchar('b'); // 块设备
    }
    else if (S_ISSOCK(mode))
    {
        putchar('s'); // 套接字
    }
    else
    {
        putchar('?'); // 未知类型
    }
}

int main()
{
    struct stat file_stat;

    if (stat("/vmlinuz", &file_stat) == -1)
    {
        perror("stat");
        return 1;
    }
    
    print_file_type(file_stat.st_mode);
    putchar('\n');

    return 0;
}
