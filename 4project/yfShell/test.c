

#include <stdio.h>
#include <unistd.h>

int main()
{
  const char *filename = "/vmlinuz"; // 替换成你的软连接文件名
  char targetPath[256];

  ssize_t len = readlink(filename, targetPath, sizeof(targetPath) - 1);
  if (len != -1)
  {
    targetPath[len] = '\0';
    printf("%s -> %s\n", filename, targetPath);
  }
  else
  {
    perror("Error");
  }

  return 0;
}
