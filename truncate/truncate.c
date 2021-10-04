#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

int main(int argc, char const *argv[])
{
    int fd = open("1.pid", O_RDONLY | O_WRONLY);
    if (fd < 0)
    {
        printf("%s\n", strerror(errno));
        return 1;
    }

    char buf[1024] = {0};
    read(fd, buf, sizeof(buf));
    printf("%s\n", buf); // 1111111

    ftruncate(fd, 0);
    write(fd, "12345", 5); // 1234511
    close(fd);
    return 0;
}
