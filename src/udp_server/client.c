#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define SERVER_PORT 8090
#define SERVER_IP "127.0.0.1"
#define MAX_BUF_SIZE 1024

int main()
{
	int client_fd;
	struct sockaddr_in server_addr;
	socklen_t server_len;
	char send_buf[MAX_BUF_SIZE];
	char recv_buf[MAX_BUF_SIZE];
	int ret;

	/* 创建 UDP 套接字 */
	client_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (client_fd < 0) {
		perror("socket fail");
		return -1;
	}

	printf("UDP client_fd: %d\n", client_fd);

	/* 设置服务器地址 */
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
	server_len = sizeof(server_addr);

	/* 发送数据 */
	while (1) {
		printf("Enter message (or 'quit' to exit): ");
		fgets(send_buf, sizeof(send_buf), stdin);
		send_buf[strlen(send_buf) - 1] = '\0';  // 去掉换行符

		if (strcmp(send_buf, "quit") == 0) {
			break;
		}

		ret = sendto(client_fd, send_buf, strlen(send_buf), 0,
					 (struct sockaddr *)&server_addr, server_len);
		if (ret < 0) {
			perror("sendto fail");
			continue;
		}

		/* 接收服务器回复 */
		memset(recv_buf, 0, sizeof(recv_buf));
		ret = recvfrom(client_fd, recv_buf, MAX_BUF_SIZE, 0, NULL, NULL);
		if (ret < 0) {
			perror("recvfrom fail");
			continue;
		}

		printf("Server response: %s\n", recv_buf);
	}

	close(client_fd);
	return 0;
}
