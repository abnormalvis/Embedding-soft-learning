#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define SERVER_PORT 8990
#define SERVER_IP "127.0.0.1"

int main()
{
	int client_fd, ret;
	struct sockaddr_in server_addr;
	socklen_t server_len;

	char send_buf[1024] = {0};
	char recv_buf[1024] = {0};

	/* 创建套接字 */
	client_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (client_fd < 0) {
		perror("socket fail");
		return -1;
	}

	printf("client_fd: %d\n", client_fd);

	/* 设置服务器地址 */
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
	server_len = sizeof(server_addr);

	/* 连接服务器 */
	ret = connect(client_fd, (struct sockaddr *)&server_addr, server_len);
	if (ret < 0) {
		perror("connect fail");
		close(client_fd);
		return -1;
	}

	printf("Connected to server %s:%d\n", SERVER_IP, SERVER_PORT);

	/* 发送数据 */
	while (1) {
		printf("Enter message: ");
		fgets(send_buf, sizeof(send_buf), stdin);
		send_buf[strlen(send_buf) - 1] = '\0';  /* 去掉换行符 */

		if (strcmp(send_buf, "quit") == 0) {
			break;
		}

		ret = send(client_fd, send_buf, strlen(send_buf), 0);
		if (ret < 0) {
			perror("send fail");
			break;
		}

		/* 接收服务器回应 */
		memset(recv_buf, 0, sizeof(recv_buf));
		ret = recv(client_fd, recv_buf, sizeof(recv_buf), 0);
		if (ret < 0) {
			perror("recv fail");
			break;
		} else if (ret == 0) {
			printf("Server closed connection\n");
			break;
		} else {
			printf("Server response: %s\n", recv_buf);
		}
	}

	close(client_fd);
	return 0;
}
