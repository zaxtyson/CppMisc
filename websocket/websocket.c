#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/tcp.h>
#include <string.h>
#include <sys/epoll.h>
#include <errno.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

enum WS_STATUS
{
    WS_INIT,
    WS_HANDSHAKE,
    WS_DATATTRANSFROM,
    WS_DATAEND
};

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
    char recv_buffer[BUFFER_SIZE];                  // 接收缓冲区, tcp 粘包时, 半包数据可以存在这
    char send_buffer[BUFFER_SIZE];                  // 发送缓冲区
    int rlength;                                    // 接收数据的长度
    int slength;                                    // 发送数据的长度
    int status;                                     // websocket 状态
};

int accept_callback(int fd, int events, void *arg);
int recv_callback(int fd, int events, void *arg);
int send_callback(int fd, int events, void *arg);
int ws_handshake(struct sockitem *si, struct reactor *loop);

struct reactor *event_loop = NULL; // 全局的 reactor 对象

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
    {
        printf("%d has already in use\n", port);
        return -2;
    }

    if (listen(listen_fd, 5) < 0)
        return -3;

    event_loop = (struct reactor *)malloc(sizeof(struct reactor));
    // 创建一个 epoll, 内核 2.6.8 之后, 参数没有意义, 大于零即可
    event_loop->ep_fd = epoll_create(1);

    // 把 listen_fd 加入 epoll, 设置回调函数用于处理 accept
    struct sockitem *si = (struct sockitem *)malloc(sizeof(struct sockitem));
    si->sock_fd = listen_fd;
    si->status = WS_INIT;
    si->callback = accept_callback;

    struct epoll_event evt;
    evt.events = EPOLLIN;
    evt.data.fd = listen_fd;
    evt.data.ptr = si; // 携带额外的指针

    epoll_ctl(event_loop->ep_fd, EPOLL_CTL_ADD, listen_fd, &evt);

    // 进入 epoll_wait 循环, 处理就绪 fd
    while (1)
    {
        int nready = epoll_wait(event_loop->ep_fd, event_loop->events, 1024, -1);
        // printf("%d clients ready\n", nready);
        if (nready < -1)
            break;

        for (int i = 0; i < nready; i++)
        {
            // 一个 fd 看能同时满足 EPOLLIN 和 EPOLLOUT
            // 但是会导致每次 epoll_wait 都返回它, 不建议把一个 fd 同时设置为 IN & OUT
            if (event_loop->events[i].events & EPOLLIN)
            {
                struct sockitem *si = (struct sockitem *)event_loop->events[i].data.ptr;
                si->callback(si->sock_fd, event_loop->events[i].events, si);
            }

            if (event_loop->events[i].events & EPOLLOUT)
            {
                struct sockitem *si = (struct sockitem *)event_loop->events[i].data.ptr;
                si->callback(si->sock_fd, event_loop->events[i].events, si);
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
    si->status = WS_HANDSHAKE; // 连接建立, 准备握手
    si->callback = recv_callback;

    struct epoll_event evt;
    // 客户端通常主动发出数据, 所以关注读事件
    // ET 触发防止每次 epoll_wait 返回大量可写的 fd, 但是我们并不想对所以的 fd send
    evt.events = EPOLLIN | EPOLLET;
    evt.data.ptr = si;

    epoll_ctl(event_loop->ep_fd, EPOLL_CTL_ADD, client_fd, &evt);
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
    int ret = recv(fd, si->recv_buffer, BUFFER_SIZE, 0);

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
        epoll_ctl(event_loop->ep_fd, EPOLL_CTL_DEL, fd, &evt);
        close(fd);
        free(si); // 记得释放内存
    }
    else if (ret == 0) // 客户端关闭连接
    {
        printf("Client closed: %d\n", fd);
        evt.events = EPOLLIN;
        epoll_ctl(event_loop->ep_fd, EPOLL_CTL_DEL, fd, &evt);
        close(fd);
        free(si); // 记得释放内存
    }
    else // 正常读取到数据
    {
        printf("Recv (%d bytes): %s", ret, si->recv_buffer);

        if (si->status == WS_HANDSHAKE)
        {
            ws_handshake(si, event_loop);
        }
        else if (si->status == WS_DATATTRANSFROM)
        {
        }
        else if (si->status == WS_DATAEND)
        {
        }

        // 触发一次 EPOLLOUT 即可
        evt.events = EPOLLOUT | EPOLLET;
        si->sock_fd = fd;
        si->callback = send_callback;
        evt.data.ptr = si;
        epoll_ctl(event_loop->ep_fd, EPOLL_CTL_MOD, fd, &evt);
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
    struct sockitem *si = (struct sockitem *)arg;

    // 发送响应数据
    // 不需要关注什么时候调用 send, 只要把数据放到 send_buffer, 只要里面有数据就会发出去
    send(fd, si->send_buffer, si->slength, 0);

    // 再把 fd 设置为 EPOLLIN, 等待下一次读取事件
    struct epoll_event evt;
    evt.events = EPOLLIN | EPOLLET;
    si->sock_fd = fd;
    si->callback = recv_callback;
    evt.data.ptr = si;
    epoll_ctl(event_loop->ep_fd, EPOLL_CTL_MOD, fd, &evt);
    return 0;
}

/**
 * @brief 处理 websocket handshake
 * 
 * @param si 
 * @param loop 
 * @return int 
 */
int ws_handshake(struct sockitem *si, struct reactor *loop)
{
    si->recv_buffer;
    return 0;
}