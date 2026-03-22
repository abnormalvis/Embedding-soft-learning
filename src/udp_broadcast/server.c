#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BROADCAST_PORT 9999
#define MAX_BUF_SIZE 1024

int main()
{
	int server_fd;
	struct sockaddr_in server_addr, client_addr;
	socklen_t client_len;
	char recv_buf[MAX_BUF_SIZE];
	int ret;
	int opt = 1;

	/* 创建 UDP 套接字 */
	server_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (server_fd < 0) {
		perror("socket fail");
		return -1;
	}

	/* 设置套接字选项 - 允许广播 */
	ret = setsockopt(server_fd, SOL_SOCKET, SO_BROADCAST, &opt, sizeof(opt));
	if (ret < 0) {
		perror("setsockopt SO_BROADCAST fail");
		close(server_fd);
		return -1;
	}

	printf("UDP Broadcast server_fd: %d\n", server_fd);

	/* 绑定地址 */
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(BROADCAST_PORT);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	ret = bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	if (ret < 0) {
		perror("bind fail");
		close(server_fd);
		return -1;
	}

	printf("UDP Broadcast Server listening on port %d...\n", BROADCAST_PORT);

	/* 主循环：接收广播消息并回复 */
	while (1) {
		memset(&client_addr, 0, sizeof(client_addr));
		client_len = sizeof(client_len);
		memset(recv_buf, 0, sizeof(recv_buf));

		ret = recvfrom(server_fd, recv_buf, MAX_BUF_SIZE, 0,
					   (struct sockaddr *)&client_addr, &client_len);
		if (ret < 0) {
			perror("recvfrom fail");
			continue;
		}

		printf("Received broadcast from %s:%d : %s\n",
			   inet_ntoa(client_addr.sin_addr),
			   ntohs(client_addr.sin_port),
			   recv_buf);

		/* 回复客户端 */
		char send_buf[MAX_BUF_SIZE];
		memset(send_buf, 0, sizeof(send_buf));
		snprintf(send_buf, sizeof(send_buf), "Broadcast received: %s", recv_buf);

		ret = sendto(server_fd, send_buf, strlen(send_buf), 0,
					 (struct sockaddr *)&client_addr, client_len);
		if (ret < 0) {
			perror("sendto fail");
		}
	}

	close(server_fd);
	return 0;
}
