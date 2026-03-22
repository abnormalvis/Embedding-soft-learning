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
#include <sys/select.h>

#define MAX_CLIENTS FD_SETSIZE
#define SERVER_PORT 8990

int main()
{
	int listen_fd, client_fd;
	struct sockaddr_in server_addr, client_addr;
	socklen_t server_len, client_len;

	char recv_buf[1024] = {0};
	int ret;

	/* 创建监听套接字 */
	listen_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listen_fd < 0) {
		perror("socket fail");
		return -1;
	}

	printf("listen_fd: %d\n", listen_fd);

	/* 绑定地址 */
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = htons(INADDR_ANY);
	server_len = sizeof(server_addr);

	if (bind(listen_fd, (struct sockaddr *)&server_addr, server_len) < 0) {
		perror("bind fail");
		return -1;
	}

	printf("bind success.\n");

	/* 开始监听 */
	listen(listen_fd, 5);
	printf("Server listening on port %d...\n", SERVER_PORT);

	/* select 相关变量 */
	fd_set readfds, tempfds;
	int max_fd;
	int client_fds[MAX_CLIENTS];  /* 存储已连接的客户端套接字 */
	int client_count = 0;

	/* 初始化 fd_set */
	FD_ZERO(&readfds);
	FD_SET(listen_fd, &readfds);
	max_fd = listen_fd;

	/* 初始化客户端数组 */
	for (int i = 0; i < MAX_CLIENTS; i++) {
		client_fds[i] = -1;
	}

	while (1) {
		tempfds = readfds;  /* 每次循环需要重新赋值 */

		/* 等待事件发生 */
		ret = select(max_fd + 1, &tempfds, NULL, NULL, NULL);
		if (ret < 0) {
			perror("select error");
			break;
		}

		/* 检查监听套接字 - 是否有新客户端连接 */
		if (FD_ISSET(listen_fd, &tempfds)) {
			client_len = sizeof(client_addr);
			client_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &client_len);
			if (client_fd < 0) {
				perror("accept fail");
				continue;
			}

			printf("New client connected: fd=%d, IP=%s, Port=%d\n",
			       client_fd,
			       inet_ntoa(client_addr.sin_addr),
			       ntohs(client_addr.sin_port));

			/* 将新客户端加入数组 */
			int i;
			for (i = 0; i < MAX_CLIENTS; i++) {
				if (client_fds[i] == -1) {
					client_fds[i] = client_fd;
					break;
				}
			}

			if (i >= MAX_CLIENTS) {
				printf("Too many clients, reject!\n");
				close(client_fd);
			} else {
				client_count++;
				/* 将新客户端加入 select 监听集合 */
				FD_SET(client_fd, &readfds);
				if (client_fd > max_fd) {
					max_fd = client_fd;
				}
			}
		}

		/* 检查所有客户端套接字 - 是否有数据到达 */
		for (int i = 0; i < MAX_CLIENTS; i++) {
			client_fd = client_fds[i];
			if (client_fd > 0 && FD_ISSET(client_fd, &tempfds)) {
				memset(recv_buf, 0, sizeof(recv_buf));

				ret = recv(client_fd, recv_buf, sizeof(recv_buf), 0);
				if (ret < 0) {
					perror("recv error");
				} else if (ret == 0) {
					/* 客户端断开连接 */
					printf("Client fd=%d disconnected\n", client_fd);
					close(client_fd);
					FD_CLR(client_fd, &readfds);
					client_fds[i] = -1;
					client_count--;
				} else {
					/* 收到数据 */
					printf("Received from fd=%d: %s\n", client_fd, recv_buf);

					/* 回发数据给客户端 */
					char send_buf[128];
					snprintf(send_buf, sizeof(send_buf), "Server received: %s", recv_buf);
					send(client_fd, send_buf, strlen(send_buf), 0);
				}
			}
		}
	}

	/* 关闭所有客户端连接 */
	for (int i = 0; i < MAX_CLIENTS; i++) {
		if (client_fds[i] > 0) {
			close(client_fds[i]);
		}
	}

	close(listen_fd);
	return 0;
}
