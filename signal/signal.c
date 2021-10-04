#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void sig_handler(int signo, siginfo_t *si, void *ctx)
{
    printf("signal: %d\n", signo);
    printf("signal number: %d\n", si->si_signo);
    printf("process pid: %d\n", si->si_pid);
    printf("sigval: %d\n", si->si_value.sival_int);
}

int main(int argc, char const *argv[])
{
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = sig_handler;
    sa.sa_flags |= SA_RESTART | SA_SIGINFO;

    if (sigaction(36, &sa, NULL) != 0)
    {
        printf("set signal error\n");
        exit(1);
    }
    while (1)
    {
        sleep(2);
    }
    printf("done\n");
    return 0;
}
