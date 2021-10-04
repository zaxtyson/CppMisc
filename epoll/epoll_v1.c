#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/tcp.h>
#include <string.h>
#include <sys/epoll.h>
#include <errno.h>
#include <arpa/inet.h>

int ep_fd = 0; // 全局的 epoll 对象

// mainloop, eventloop
struct reactor
{
    int ep_fd;                       // epoll 对象的 fd
    struct epoll_event events[1024]; // epoll_wait 就绪队列
};

// 服务器端一个 socket 对应一个 socketitem
struct sockitem
{
    int sock_fd;                                    // socket 的 fd
    int (*callback)(int fd, int events, void *arg); // 处理该 socket 事件的回调函数
    char recv_buffer[1024];                         // 接收缓冲区, tcp 粘包时, 半包数据可以存在这
    char send_buffer[1024];
};

int accept_callback(int fd, int events, void *arg);
int recv_callback(int fd, int events, void *arg);
int send_callback(int fd, int events, void *arg);

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: ./epoll port\n");
        return -1;
    }

    int port = atoi(argv[1]);
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0)
        return -1;

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(listen_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
        return -2;

    if (listen(listen_fd, 5) < 0)
        return -3;

    // 创建一个 epoll, 内核 2.6.8 之后, 参数没有意义, 大于零即可
    ep_fd = epoll_create(1);

    // 把 listen_fd 加入 epoll, 设置回调函数用于处理 accept
    struct sockitem *si = (struct sockitem *)malloc(sizeof(struct sockitem));
    si->sock_fd = listen_fd;
    si->callback = accept_callback;

    // ET: 边沿触发, socket 的 recv buffer 从空到有数据才触发, 只触发一次
    // LT: 水平触发, 只要 recv buffer 还有数据就自动触发, 默认 EPOLLLT
    // 未知数据大小时, 不用 ET 循环读, 它可能导致一个 IO 耗费大量时间
    // 数据少 ET, 数据多 LT, listen_fd 适合 LT
    struct epoll_event evt;
    evt.events = EPOLLIN;
    evt.data.fd = listen_fd;
    evt.data.ptr = si; // 携带额外的指针

    epoll_ctl(ep_fd, EPOLL_CTL_ADD, listen_fd, &evt);

    // 进入 epoll_wait 循环, 处理就绪 fd
    struct epoll_event events[1024] = {0};
    while (1)
    {
        int nready = epoll_wait(ep_fd, events, 1024, -1);
        // printf("%d clients ready\n", nready);
        if (nready < -1)
            break;

        for (int i = 0; i < nready; i++)
        {
            if (events[i].events & EPOLLIN)
            {
                struct sockitem *si = (struct sockitem *)events[i].data.ptr;
                si->callback(si->sock_fd, events[i].events, si);
            }

            if (events[i].events & EPOLLOUT)
            {
                struct sockitem *si = (struct sockitem *)events[i].data.ptr;
                si->callback(si->sock_fd, events[i].events, si);
            }
        }
    }
}

/**
 * @brief 用于处理 accept 事件的回调函数
 * 
 * @param listen_fd 负责 accept 的 fd
 * @param events fd 的 events 类型
 * @param arg 其它附加数据, sockitem
 * @return int accept 返回的客户端 fd 
 */
int accept_callback(int listen_fd, int events, void *arg)
{
    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    memset(&client_addr, 0, client_addr_len);

    int client_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &client_addr_len);
    if (client_fd < 0)
        return -1;

    char str[INET_ADDRSTRLEN] = {0};
    printf("Accept %d: %s\n", client_fd, inet_ntop(AF_INET, (struct sockaddr_in *)&client_addr.sin_addr, str, sizeof(str)));

    // 将客户端 fd 加入 epoll
    struct sockitem *si = (struct sockitem *)malloc(sizeof(struct sockitem));
    si->sock_fd = client_fd;
    si->callback = recv_callback;

    struct epoll_event evt;
    // 客户端通常主动发出数据, 所以关注读事件
    // ET 触发防止每次 epoll_wait 返回大量可写的 fd, 但是我们并不想对所以的 fd send
    evt.events = EPOLLIN | EPOLLET;
    evt.data.ptr = si;

    epoll_ctl(ep_fd, EPOLL_CTL_ADD, client_fd, &evt);
    return client_fd;
}

/**
 * @brief 处理可读 EPOLLIN 事件的回调函数 
 * 
 * @param fd 读就绪的 fd 
 * @param events 事件类型 EPOLLIN
 * @param arg 附加数据, sockitem
 * @return int 
 */
int recv_callback(int fd, int events, void *arg)
{
    struct sockitem *si = (struct sockitem *)arg;
    struct epoll_event evt;
    char buffer[1024] = {0};
    int ret = recv(fd, buffer, 1024, 0);

    // 出现异常
    if (ret < 0)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
            return -1;
        }
        else
        {
        }

        // 关闭 fd
        evt.events = EPOLLIN;
        epoll_ctl(ep_fd, EPOLL_CTL_DEL, fd, &evt);
        close(fd);
        free(si); // 记得释放内存
    }
    else if (ret == 0) // 客户端关闭连接
    {
        printf("Client closed: %d\n", fd);
        evt.events = EPOLLIN;
        epoll_ctl(ep_fd, EPOLL_CTL_DEL, fd, &evt);
        close(fd);
        free(si); // 记得释放内存
    }
    else // 正常读取到数据
    {
        printf("Recv (%d bytes): %s", ret, buffer);

        // 如果需要返回数据给客户端, 最好是触发一次 EPOLLOUT
        evt.events = EPOLLOUT | EPOLLET;
        si->sock_fd = fd;
        si->callback = send_callback;
        evt.data.ptr = si;
        epoll_ctl(ep_fd, EPOLL_CTL_MOD, fd, &evt);
    }
    return 0;
}

/**
 * @brief 写事件就绪回调函数
 * 
 * @param fd 可写的 fd
 * @param events 事件类型, 
 * @param arg 
 * @return int 
 */
int send_callback(int fd, int events, void *arg)
{
    send(fd, "ok!!", 5, 0); // 发送响应

    // 再把 fd 设置为 EPOLLIN, 等待下一次读取事件
    struct sockitem *si = (struct sockitem *)arg;
    struct epoll_event evt;
    evt.events = EPOLLIN | EPOLLET;
    si->sock_fd = fd;
    si->callback = recv_callback;
    evt.data.ptr = si;
    epoll_ctl(ep_fd, EPOLL_CTL_MOD, fd, &evt);
}