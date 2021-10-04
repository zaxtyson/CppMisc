#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <errno.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <fcntl.h>
#include <string.h>

#define MAX_BUFFER 256
#define MAX_EPOLLSIZE (384 * 1024)
#define MAX_PORT 100

#define TIME_SUB_MS(tv1, tv2) ((tv1.tv_sec - tv2.tv_sec) * 1000 + (tv1.tv_usec - tv2.tv_usec) / 1000)

static int ntySetNonblock(int fd)
{
	int flags;

	flags = fcntl(fd, F_GETFL, 0);
	if (flags < 0)
		return flags;
	flags |= O_NONBLOCK;
	if (fcntl(fd, F_SETFL, flags) < 0)
		return -1;
	return 0;
}

static int ntySetReUseAddr(int fd)
{
	int reuse = 1;
	return setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse));
}

int main(int argc, char **argv)
{
	if (argc < 3)
	{
		printf("Usage: %s ip port\n", argv[0]);
		exit(0);
	}

	const char *ip = argv[1];
	int port = atoi(argv[2]);

	int connections = 0;
	char buffer[MAX_BUFFER] = {0};

	struct epoll_event events[MAX_EPOLLSIZE];
	int epoll_fd = epoll_create(MAX_EPOLLSIZE);
	strcpy(buffer, "Data From MulClient\n");

	struct sockaddr_in addr;
	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(ip);

	struct timeval tv_begin;
	gettimeofday(&tv_begin, NULL);

	int index = 0;
	while (1)
	{
		if (++index >= MAX_PORT)
			index = 0;

		struct epoll_event ev;
		int sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd == -1)
		{
			printf("error : %s\n", strerror(errno));
			return -1;
		}

		// ntySetReUseAddr(sockfd);
		addr.sin_port = htons(port + index);

		if (connect(sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) < 0)
		{
			printf("error : %s\n", strerror(errno));
			return -2;
		}

		ntySetNonblock(sockfd);
		ntySetReUseAddr(sockfd);

		snprintf(buffer, MAX_BUFFER, "Hello Server: client --> %d\n", connections);
		send(sockfd, buffer, strlen(buffer), 0);

		ev.data.fd = sockfd;
		ev.events = EPOLLIN | EPOLLOUT;
		epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sockfd, &ev);

		connections++;

		if (connections % 1000 == 0)
		{
			struct timeval tv_cur;
			memcpy(&tv_cur, &tv_begin, sizeof(struct timeval));
			gettimeofday(&tv_begin, NULL);

			int time_used = TIME_SUB_MS(tv_begin, tv_cur);
			printf("Connections: %d, sockfd: %d, time_used: %d\n", connections, sockfd, time_used);

			int nfds = epoll_wait(epoll_fd, events, connections, 100);
			for (int i = 0; i < nfds; i++)
			{
				int clientfd = events[i].data.fd;

				if (events[i].events & EPOLLOUT)
				{
					snprintf(buffer, MAX_BUFFER, "data from %d\n", clientfd);
					send(sockfd, buffer, strlen(buffer), 0);
				}
				else if (events[i].events & EPOLLIN)
				{
					char rev_buffer[MAX_BUFFER] = {0};
					ssize_t length = recv(sockfd, rev_buffer, MAX_BUFFER, 0);
					if (length == 0)
					{
						printf("Disconnect clientfd: %d\n", clientfd);
						connections--;
						close(clientfd);
					}
					else if (length < 0)
					{
						// nonblock 模式 read_buffer 为空
						if (errno == EINTR || errno == EAGAIN || errno == EWOULDBLOCK)
							continue;

						// if(errno == ETIMEDOUT)  // tcp 探活包超时, 对方掉线, 可以 close
						// net.ipv4.tcp_keepalive_time = 7200 (s)
						printf("Error clientfd: %d, %s\n", clientfd, strerror(errno));
						connections--;
						close(clientfd);
					}
				}
				else
				{
					printf("Events error, clientfd: %d, %s\n", clientfd, strerror(errno));
					connections--;
					close(clientfd);
				}
			}
		}

		usleep(1 * 1000);
	}

	return 0;
}
