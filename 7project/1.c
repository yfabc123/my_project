#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
int main()
{
    fd_set readSet;
    struct timeval timeout;
    while (1)
    {
        // 清空文件描述符集合
        FD_ZERO(&readSet);
        // 添加标准输入文件描述符到集合
        FD_SET(STDIN_FILENO, &readSet);
        // 设置超时时间为 1 秒
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        // 等待文件描述符就绪
        int result = select(STDIN_FILENO + 1, &readSet, NULL, NULL, &timeout);
        if (result == -1)
        {
            perror("select");
            exit(EXIT_FAILURE);
        }
        else if (result > 0)
        {
            // 标准输入就绪，可以进行读取操作
            if (FD_ISSET(STDIN_FILENO, &readSet))
            {
                char buffer[256];
                ssize_t bytesRead = read(STDIN_FILENO, buffer, sizeof(buffer));
                if (bytesRead > 0)
                {
                    buffer[bytesRead] = '\0';
                    printf("Read from stdin: %s", buffer);
                }
                else
                {
                    perror("read");
                    exit(EXIT_FAILURE);
                }
            }
        }
        else
        {
            // 超时，可以执行其他任务
            printf("Timeout, do something else...\n");
        }
    }
    return 0;
}
