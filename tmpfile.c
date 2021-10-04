#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{
    // FILE *tmp = tmpfile(); // rw, close 自动删除, 线程安全， 文件名可能冲突而失败
    char filename[] = "/tmp/hello_word_XXXXXX"; // 可指定路径(路径必须存在)， 线程安全, 不会自动删除， 冲突自动改名
    int fd = mkstemp(filename);
    if (fd < 0)
    {
        printf("%s\n", strerror(errno));
        return -1;
    }
    printf("Filename is %s\n", filename);

    FILE *fp = fdopen(fd, "w");
    fwrite(filename, sizeof(filename), 1, fp);
    // close(fd);
    unlink(filename);
    fclose(fp);
    return 0;
}
