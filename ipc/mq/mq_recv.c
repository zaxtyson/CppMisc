#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <mqueue.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/types.h>

int main(int argc, char const *argv[])
{
    char *mq_name = "/test01";
    mqd_t mqd = mq_open(mq_name, O_RDWR | O_NONBLOCK);
    if (mqd == -1)
    {
        fprintf(stderr, "open %s faild: %s\n", mq_name, strerror(errno));
        exit(1);
    }

    printf("open %s success\n", mq_name);

    struct mq_attr attr;
    mq_getattr(mqd, &attr);
    int msg_len = attr.mq_msgsize;
    char *msg = (char *)malloc(msg_len); // 至少 mq_msgsize

    fd_set rfds;
    FD_ZERO(&rfds);

    int ret;
    for (;;)
    {

        FD_SET(mqd, &rfds);
        int nfds = select(mqd + 1, &rfds, NULL, NULL, NULL);
        if (nfds == -1 && (errno == EINTR || errno == EAGAIN))
        {
            continue;
        }
        if (FD_ISSET(mqd, &rfds))
        {
            // 最大优先级 MQ_PRIO_MAX 32768, 优先级高的先取走
            // 返回 msg_len
            ret = mq_receive(mqd, msg, msg_len, NULL);
            if (ret == -1 && errno == EAGAIN)
            {
                continue;
            }
            printf("[%d] Receive: %s", ret, msg);
            fflush(stdout);
            // ret = mq_send(mqd, msg, msg_len, 100); // 消息优先级， 非负
        }
    }

    free(msg);
    msg = NULL;
    return 0;
}
