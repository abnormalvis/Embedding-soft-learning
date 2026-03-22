#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/epoll.h>

#define MAX_CLIENTS 10000
#define SERVER_PORT 8990
#define BUFFER_SIZE 1024

/*
 * setnonblocking - 设置句柄为非阻塞方式
 * 返回值: 成功返回0，失败返回-1
 */
int setnonblocking(int sockfd)
{
	int flags = fcntl(sockfd, F_GETFL, 0);
	if (flags == -1) {
		return -1;
	}
	if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1) {
		return -1;
	}
	return 0;
}

/*
 * addfd - 将文件描述符添加到 epoll 实例中
 */
void addfd(int epoll_fd, int fd, uint32_t events)
{
	struct epoll_event ev;
	ev.events = events;
	ev.data.fd = fd;

	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev) == -1) {
		perror("epoll_ctl add fd fail");
	}
}

/*
 * removefd - 从 epoll 实例中移除文件描述符
 */
void removefd(int epoll_fd, int fd)
{
	if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL) == -1) {
		perror("epoll_ctl del fd fail");
	}
}

int main()
{
	int listen_fd, client_fd;
	struct sockaddr_in server_addr, client_addr;
	socklen_t client_len;

	char buffer[BUFFER_SIZE];
	int ret;

	/* 创建 epoll 实例 */
	int epoll_fd = epoll_create(MAX_CLIENTS);
	if (epoll_fd < 0) {
		perror("epoll_create fail");
		return -1;
	}

	printf("Epoll fd: %d\n", epoll_fd);

	/* 创建监听套接字 */
	listen_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listen_fd < 0) {
		perror("socket fail");
		close(epoll_fd);
		return -1;
	}

	/* 设置端口复用 */
	int opt = 1;
	setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	/* 设置非阻塞模式 - ET模式必须使用非阻塞 */
	setnonblocking(listen_fd);

	/* 绑定地址 */
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		perror("bind fail");
		close(listen_fd);
		close(epoll_fd);
		return -1;
	}

	printf("bind success.\n");

	/* 开始监听 */
	if (listen(listen_fd, 5) < 0) {
		perror("listen fail");
		close(listen_fd);
		close(epoll_fd);
		return -1;
	}

	printf("Server listening on port %d...\n", SERVER_PORT);

	/* 将监听套接字添加到 epoll，使用边缘触发(ET)模式 */
	struct epoll_event ev;
	ev.events = EPOLLIN | EPOLLET;  // 边缘触发 + 读事件
	ev.data.fd = listen_fd;

	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &ev) < 0) {
		perror("epoll_ctl add listen_fd fail");
		close(listen_fd);
		close(epoll_fd);
		return -1;
	}

	printf("Listen socket added to epoll (ET mode)\n");

	/* 事件数组 */
	struct epoll_event events[MAX_CLIENTS];

	/* 主循环 */
	while (1) {
		/* 等待事件发生，-1表示阻塞等待 */
		int nfds = epoll_wait(epoll_fd, events, MAX_CLIENTS, -1);
		if (nfds < 0) {
			perror("epoll_wait fail");
			break;
		}

		/* 遍历所有就绪的事件 */
		for (int i = 0; i < nfds; i++) {
			int fd = events[i].data.fd;

			/* 监听套接字有事件 - 新客户端连接 */
			if (fd == listen_fd) {
				/* ET模式需要循环接受所有连接 */
				while (1) {
					client_len = sizeof(client_addr);
					client_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &client_len);

					if (client_fd < 0) {
						if (errno == EAGAIN || errno == EWOULDBLOCK) {
							/* 所有连接已接受完毕 */
							break;
						}
						perror("accept fail");
						break;
					}

					/* 打印客户端信息 */
					printf("New client: fd=%d, IP=%s, Port=%d\n",
						   client_fd,
						   inet_ntoa(client_addr.sin_addr),
						   ntohs(client_addr.sin_port));

					/* 设置客户端套接字为非阻塞 */
					setnonblocking(client_fd);

					/* 添加客户端到 epoll */
					ev.events = EPOLLIN | EPOLLET;  // 边缘触发
					ev.data.fd = client_fd;
					if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) < 0) {
						perror("epoll_ctl add client_fd fail");
						close(client_fd);
					}
				}
			}
			/* 客户端套接字有事件 - 接收数据 */
			else {
				/* ET模式需要循环读取所有数据 */
				while (1) {
					memset(buffer, 0, BUFFER_SIZE);
					ret = recv(fd, buffer, BUFFER_SIZE - 1, 0);

					if (ret < 0) {
						if (errno == EAGAIN || errno == EWOULDBLOCK) {
							/* 数据读取完毕 */
							break;
						}
						perror("recv fail");
						/* 发生错误，关闭连接 */
						close(fd);
						removefd(epoll_fd, fd);
						printf("Client fd=%d error, closed\n", fd);
						break;
					}
					else if (ret == 0) {
						/* 客户端关闭连接 */
						printf("Client fd=%d disconnected\n", fd);
						close(fd);
						removefd(epoll_fd, fd);
						break;
					}
					else {
						/* 收到数据 */
						printf("Received from fd=%d: %s (len=%d)\n", fd, buffer, ret);

						/* 回发响应 */
						char send_buf[128];
						int len = snprintf(send_buf, sizeof(send_buf),
										   "Server received: %s", buffer);
						send(fd, send_buf, len, 0);
					}
				}
			}
		}
	}

	/* 清理资源 */
	close(listen_fd);
	close(epoll_fd);

	return 0;
}
