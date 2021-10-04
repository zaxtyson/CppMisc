#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

int main(int argc, char const *argv[])
{
    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork error");
    }
    else if (pid == 0)
    {
        printf("son\n");
    }
    else
    {
        printf("father\t");
    }

    return 0;
}
