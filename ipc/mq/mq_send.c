#include <stdio.h>
#include <unistd.h>
#include <mqueue.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

int main(int argc, char const *argv[])
{
    char *mq_name = "/test01";
    mqd_t mqd = mq_open(mq_name, O_RDWR | O_NONBLOCK);
    if (mqd == -1)
    {
        fprintf(stderr, "open %s failed: %s\n", mq_name, strerror(errno));
        exit(1);
    }

    struct mq_attr attr;
    mq_getattr(mqd, &attr);
    int msg_len = attr.mq_msgsize;

    char *msg = (char *)malloc(msg_len);
    int ret;

    for (;;)
    {
        printf("send: ");
        fflush(stdout);
        fgets(msg, msg_len, stdin);
        ret = mq_send(mqd, msg, msg_len, 100);
        // printf("ret： %d\n", ret);
    }

    free(msg);
    msg = NULL;
    return 0;
}
