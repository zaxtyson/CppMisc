#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

int main(int argc, char const *argv[])
{
    int ret = daemon(1, 0); // 没有第二次 fork, ppid = 1, sid=pid=grpid
    if (ret != 0)
    {
        exit(1);
    }
    else
    {
        char buf[128] = {0};
        snprintf(buf, sizeof(buf), "%d/%d/%d/%d\n", getpid(), getppid(), getpgrp(), getsid(0));
        FILE *fp = fopen("my.pid", "w");
        fwrite(buf, sizeof(buf), 1, fp); // 17753/1/17753/17753
        // fwrite("bye", 4, 1, fp);
        fclose(fp);
        sleep(60);
    }
    return 0;
}
