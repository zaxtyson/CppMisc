#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

// gcc -o mqcreate mqcreate.c -lrt

// https://elixir.bootlin.com/linux/v5.3.18/source/ipc/mqueue.c#L799
// mq_open() -> do_mq_open（） -> audit_mq_open()
// do_mq_open() 会 get_unused_fd_flags(O_CLOEXEC) 分配 fd

int main(int argc, char *argv[])
{
    char *mq_name = "/test01"; // 必须是 / 开头， 放在 /dev/mqueue/xxx

    // 超过 sysctl -a | grep mqueue 限制就会 EINVA
    // 普通用户默认 10 条， 一条不超过 8192 字节（实际上不能满载81920字节，因为mqueue_get_inode要占用一些空间)
    // 硬上限取决于内核版本， 3.5+ 65535 条， 16MB每条
    // 除此以外， RLIMIT_MSGQUEUE （ulimit -q查看) 限制了用户能创建的**所有mq**大小之和
    // 下面两个属性只能在创建的时候指定， 虽然提供了 mq_setattr()
    struct mq_attr attr;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = 8192;

    // O_CREAT 用于创建， 可以跟 O_EXCL 排他性创建， 如果存在就 -1, EEXIST
    // 0644 是 mq 的访问权限
    // 第 4 个参数使用指定的属性， NULL 为默认
    mqd_t mqd = mq_open(mq_name, O_RDWR | O_CREAT | O_EXCL, 0644, &attr);
    if (mqd == -1 && errno == EEXIST)
    {
        // 已经存在， 尝试打开
        mqd = mq_open(mq_name, O_RDWR);
        if (mqd == -1)
        {
            fprintf(stderr, "open mq failed: %s\n", strerror(errno));
            exit(1);
        }
    }

    printf("open %s success!\n", mq_name);

    // 查看 mq 属性
    mq_getattr(mqd, &attr);
    printf("maxmsg: %ld, maxbytes: %ld, curmsg: %ld\n", attr.mq_maxmsg, attr.mq_msgsize, attr.mq_curmsgs);

    // attr.mq_flags |= O_NONBLOCK;
    // mq_setattr(mqd, &attr);  // 只能以来改 flag

    // fork() 后子进程获得 fd， 引用相同的打开 mq
    // exec() 默认 O_CLOEXEC， mq 自动 close
    // 进程结束自动 close mq
    if (mq_close(mqd) != -1) // 引用计数 -1， 即便计数=0, 只要不unlink， mq 文件还在， 消息也还在
    {
        printf("close %s success!\n", mq_name);
    }

    // sleep(1);
    // 程序退出或者 exec, ipc 对象自动删除
    // if (mq_unlink(mq_name) != -1)
    // {
    //     printf("unlink %s success!\n", mq_name);
    // }
    return 0;
}
