#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_PORT 8090
#define MAX_BUF_SIZE 1024

int main()
{
	int server_fd;
	struct sockaddr_in server_addr, client_addr;
	socklen_t client_len;
	char recv_buf[MAX_BUF_SIZE];
	char send_buf[MAX_BUF_SIZE];
	int ret;

	/* 创建 UDP 套接字 */
	server_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (server_fd < 0) {
		perror("socket fail");
		return -1;
	}

	printf("UDP server_fd: %d\n", server_fd);

	/* 绑定地址 */
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);  // 监听所有网卡

	ret = bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	if (ret < 0) {
		perror("bind fail");
		close(server_fd);
		return -1;
	}

	printf("UDP Server listening on port %d...\n", SERVER_PORT);

	/* 主循环：接收数据并回复 */
	while (1) {
		memset(&client_addr, 0, sizeof(client_addr));
		client_len = sizeof(client_addr);
		memset(recv_buf, 0, sizeof(recv_buf));

		/* 接收数据 */
		ret = recvfrom(server_fd, recv_buf, MAX_BUF_SIZE, 0,
					   (struct sockaddr *)&client_addr, &client_len);
		if (ret < 0) {
			perror("recvfrom fail");
			continue;
		}

		printf("Received from %s:%d : %s\n",
			   inet_ntoa(client_addr.sin_addr),
			   ntohs(client_addr.sin_port),
			   recv_buf);

		/* 回复客户端 */
		memset(send_buf, 0, sizeof(send_buf));
		snprintf(send_buf, sizeof(send_buf), "Server received: %s", recv_buf);
		ret = sendto(server_fd, send_buf, strlen(send_buf), 0,
					 (struct sockaddr *)&client_addr, client_len);
		if (ret < 0) {
			perror("sendto fail");
		}
	}

	close(server_fd);
	return 0;
}
