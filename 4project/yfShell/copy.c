#include <stdio.h>
#include <unistd.h>

int main()
{
    const char *path = "/";

    // 使用 access 函数检查文件是否存在
    if (access(path, F_OK) == 0)
    {
        // 文件存在
        printf("文件存在，获取文件名：%s\n", path);
    }
    else
    {
        // 文件不存在或无法访问
        printf("文件不存在：%s\n", path);
    }

    return 0;
}
