#include <stdio.h>
#include <string.h>
#include <errno.h>

// #define IO_DEBUG

int main(int argc, char const *argv[])
{
    // setbuf(stdout, NULL);
    FILE *fp = fopen("read.c", "r");
    printf("%d\n", fp->_fileno); // fileno(fp)

    if (!fp)
    {
        printf("%s\n", strerror(errno));
        return -1;
    }
    int c = 0;
    while ((c = fgetc(fp)) != EOF)
    {
        printf("%d", c);
    }
    return 0;
}
