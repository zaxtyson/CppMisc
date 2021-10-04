#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char const *argv[])
{
    const char *sem_name = "/test";
    sem_t *sem = sem_open(sem_name, O_RDWR | O_CREAT | O_EXCL, 0644, 0); // 可原子的指定初始值
    if (sem == SEM_FAILED)
    {
        sem = sem_open(sem_name, O_RDWR);
        if (sem == SEM_FAILED)
        {
            fprintf(stderr, "Error opening semaphore\n");
            exit(1);
        }
    }

    // sem_t *sem_dup;
    // *sem_dup = *sem; // 别复制！

    sem_wait(sem);
    printf("get sem %s\n", sem_name);

    return 0;
}
