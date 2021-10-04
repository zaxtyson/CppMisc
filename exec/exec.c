#include <unistd.h>

int main(int argc, char const *argv[])
{
    char *const m_argv[] = {"ps", "ax", NULL};
    char *const m_envp[] = {"PATH=/bin:/usr/bin", "TERM=console", NULL};

    // execl("/bin/ps", "ps", "ax", NULL);
    // execlp("ps", "ps", "ax", NULL);
    // execle("/bin/ps", "ps", "ax", NULL, m_envp);
    // execv("/bin/ps", m_argv);
    // execvp("ps", m_argv);
    execve("/bin/ps", m_argv, m_envp);
    return 0;
}
