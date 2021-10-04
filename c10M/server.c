// echo server

// ulimit -n : open files, 限制进程打开的文件描述符数量, 默认 8192
//             limit -n 100000 修改仅对当前进程有效
// 修改配置文件永久生效: /etc/security/limits.conf

// socket --> fd --> 五元组 (src_ip, dest_ip, src_port, dest_port, protocol)
// 当远程主机 protocol://ip:port 确定后, 本机只能通过变化端口号来与服务器创建更多连接, 而进程可以打开多 port 是有限多
// sysctl -w net.ipv4.ip_local_port_range="32768 60999", 默认只允许本机使用 2.8w 个端口去连接远程主机
// 配置文件: /etc/sysctl.conf, /etc/sysctl.d/*.conf, ...

//                                                   iptables(应用层)
//                                                       ↑
// 网卡数据 --(DMA)--> 网卡内存缓冲区 --(网卡中断处理程序)--> netfilter --> 协议栈 --> socket recv_buffer
// sysctl -w net.netfilter.nf_conntrack_max=10000, 控制连接数量, 过多时禁止向外连接, 客户端: Connection refused

// net.ipv4.tcp_mem = 189096 252131 378192, 单位 页(4KB), tcp 连接使用的内存限制, 达到 252131 时进行优化, 达到 378192 时不再分配
// net.ipv4.tcp_wmem = 4096 16384 4194304, 单位 字节, socket 的 write buffer 的内存限制
// net.ipv4.tcp_rmem = 4096 131072 6291456, 单位 字节, socket 的 read buffer 的内存限制
// 如果服务器用于文件传输, 把 wmem/rmem 调大; 字符传输时调小, 使得同内存情况下能够创建更多 fd (fd的占用空间主要是r/w buffer)
// 程序内部可以使用 setsockopt() 修改 r/w buffer 大小

// 连接抖动: 单个线程 accept 处理不过来, 三次握手时半/全连接队列排满了, 很多连接在排队(accept 在全连接队列取)
// 建议: 多线/进程去做 accept 接入, recv/send 分离到线程池去处理

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/tcp.h>
#include <string.h>
#include <sys/epoll.h>
#include <errno.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024
#define LISTEN_PORT 100

struct reactor
{
    int ep_fd;                       // epoll 对象的 fd
    struct epoll_event events[1024]; // epoll_wait 就绪队列
};

struct sockitem
{
    int sock_fd;                                    // socket 的 fd
    int (*callback)(int fd, int events, void *arg); // 处理该 socket 事件的回调函数
    char recv_buffer[BUFFER_SIZE];                  // 接收缓冲区, tcp 粘包时, 半包数据可以存在这
    char send_buffer[BUFFER_SIZE];                  // 发送缓冲区
    int rlength;                                    // 接收数据的长度
    int slength;                                    // 发送数据的长度
};

int listen_port(uint16_t port);
int accept_callback(int fd, int events, void *arg);
int recv_callback(int fd, int events, void *arg);
int send_callback(int fd, int events, void *arg);

struct reactor *event_loop = NULL; // 全局的 reactor 对象

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: ./server port\n");
        return -1;
    }

    int port = atoi(argv[1]);

    event_loop = (struct reactor *)malloc(sizeof(struct reactor));
    // 创建一个 epoll, 内核 2.6.8 之后, 参数没有意义, 大于零即可
    event_loop->ep_fd = epoll_create(1);

    // 监听 LISTEN_PORT 个端口
    for (int i = 0; i < LISTEN_PORT; i++)
    {
        int listen_fd = listen_port((uint16_t)(port + i));

        // 把 listen_fd 加入 epoll, 设置回调函数用于处理 accept
        struct sockitem *si = (struct sockitem *)malloc(sizeof(struct sockitem));
        si->sock_fd = listen_fd;
        si->callback = accept_callback;

        struct epoll_event evt;
        evt.events = EPOLLIN;
        evt.data.fd = listen_fd;
        evt.data.ptr = si; // 携带额外的指针

        epoll_ctl(event_loop->ep_fd, EPOLL_CTL_ADD, listen_fd, &evt);
    }

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

        // 如果需要返回数据给客户端, 把待发送的数据放到 send_buffer
        si->rlength = ret; // 接收到的数据长度
        memcpy(si->send_buffer, si->recv_buffer, si->rlength);
        si->slength = si->rlength;

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
 * @brief 监听一个端口
 * 
 * @param port 端口号 
 * @return int listen_fd
 */
int listen_port(uint16_t port)
{
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

    printf("Listen port %d\n", port);
    return listen_fd;
}