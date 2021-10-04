#include <stdio.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{
    setbuf(stdout, NULL);
    printf("Hello ");
    if (fork() == 0)
    {
        printf(" child\n");
    }
    else
    {
        printf(" parent\n");
    }
    return 0;
}
