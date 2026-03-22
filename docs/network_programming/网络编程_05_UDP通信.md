# 5. UDP基本通信

- [[网络编程_01_基础.md|第一章：网络通信基础]]
- [[网络编程_02_调试工具.md|第二章：网络调试工具]]
- [[网络编程_03_TCP通信.md|第三章：TCP基本通信]]
- [[网络编程_04_并发服务器.md|第四章：TCP并发服务器（含select）]]

---

# 5.1 UDP 介绍

# 5.1.1 UDP 的工作原理

UDP将网络数据流量压缩成数据报的形式，每一个数据报用8个字节（ $8 \times 8$ 位=64位）描述报头信息，剩余字节包含具体的传输数据。UDP报头（只有8个字节）相当于TCP的报头（至少20个字节）很短，UDP报头由4个域组成，每个域各占2个字节，具体为源端口、目的端口、用户数据报长度和校验和，

具体结构见下图（下面也贴出了TCP报文的结构图，与UDP数据报做一个对比的作用）：

![image](https://cdn-mineru.openxlab.org.cn/result/2026-03-20/972edad8-2f03-4bde-a593-d3f177302fb8/da91138a61796f49a76a76762d381b61940ca096facf14b30fd96da568b38074.jpg)


![image](https://cdn-mineru.openxlab.org.cn/result/2026-03-20/972edad8-2f03-4bde-a593-d3f177302fb8/05e60ffca59dd63f9629a539ea9d73559bf1dc0101db553830e59866ef824950.jpg)


UDP协议和TCP协议都使用端口号为不同的应用保留其各自的数据传输通道这一机制，数据发送方将UDP数据报通过源端口发送出去，而数据接收方则通过目标端口接收数据

# 5.1.2 UDP的优势

前面介绍中说 UDP 相对于 TCP 是不可靠的，不能保证有序传输的传输协议，然而 UDP 协议相对于 TCP 协议的优势在哪里呢？，

UDP相对于TCP的优势主要有三个方面的：

1）UDP速度比TCP快。

由于 UDP 不需要先与对方建立连接，也不需要传输确认，因此其数据的传输速度比 TCP 快很多。对于一些着重传输性能而不是传输完整性的应用（网络音频播放、视频点播和网络会议等），使用 UDP 协议更加适合，因为它传输速度快，使通过网络播放的视频音质好、画面清晰。

# 2）UDP有消息边界。

通过 UDP 协议进行传输的发送方对应用程序交下来的报文，在添加首部后就向下直接交付给 IP 层。既不拆分也不合并，而是保留这些报文的边界，所以使用 UDP 协议不需要像 TCP 那样考虑消息边界的问题，这样就使得 UDP 编程相对于 TCP 在接收到的数据处理方面要简单的多。（对于 TCP 消息边界的问题可以查看相关的文档，在这里我就不列出来了）

# 3）UDP可以一对多传输

由于传输数据部建立连接，也就不需要维护连接状态，因此一台服务器可以同时向多个客户端发送相同的信息。利用 UDP 可以使用广播或者组播的方式同时向子网的所有客户端进程发送信息，广播和组播的介绍放到后面 TCP 编程中介绍。

上面介绍了 UDP 协议相对于 TCP 协议的优势，其中速度快是 UDP 的最重要的优势，也是像一些网络会议、即时通信软件传输层选择 UDP 协议进行传输的原因所在。

# 5.2 UDP编程模型

面向无连接的 socket 通信是不可靠传输，在通信前不需要建立可靠连接。下图所示为面向无连接的 socket 通信双方执行函数流程。所有面向无连接的 socket 通信实现基本都符合这一流程。

![image](https://cdn-mineru.openxlab.org.cn/result/2026-03-20/972edad8-2f03-4bde-a593-d3f177302fb8/fcfc21efb0b05d1e09d47b83b305bd9d3bf2a0af4518e7c0801aaf2ccedd80f2.jpg)


基本流程如下：

1）服务器端建立

① 调用 socket 函数，建立 socket 对象，并指定通信协议。

(2) 调用bind函数, 将创建的socket对象与UDP端口绑定。

(3) 调用sendto或者recvfrom函数收发数据

2）客户端建立

(1) 调用 socket 函数, 建立 socket 对象, 并指定通信协议。

② 隐性调用bind函数，将创建的socket对象与UDP端口绑定。

(3) 调用 sendto 或者 recvfrom 函数收发数据

# 5.3 UDP 编程 API

include <sys/socket.h> 

Api1.sendto函数

sendto函数用于从UDP连接的一端发送数据给另外一端。

```c
ssize_t sendto(int sockfd, const void *buf, size_t nBytes, int flags, const struct sockaddr *dest_addr, socklen_t addrlen); 
```

Parm1: Sockfd, 指定接收端套接字描述符;

Parm2: buf, 指明一个缓冲区，该缓冲区用来存放 recv 函数接收到的数据；

Parm3: nBytes, 指明buf的长度;

Parm4: flags, 一般置为0。

具体定义：flags一般可以使用如下值

0：与write()无异

MSG_DONTROUTE:告诉内核，目标主机在本地网络，不用查路由表

MSG_OOB:指明发送的是带外信息

Parm5:dest_addr, 接收主机地址

Parm6:addrlen, 接收主机地址的长度

Return：成功返回接收的字节数，失败返回-1；

Api2. recvfrom 函数

recvfrom函数用于从TCP连接的另一端接收数据

```c
ssize_t recvfrom(int sockfd, void *buf, size_t len, int flags, const struct sockaddr *src_addr, socklen_t *addrlen); 
```

Parm1: Sockfd, 指定接收端套接字描述符;

Parm2: buf, 指明一个缓冲区，该缓冲区用来存放 recv 函数接收到的数据；

Parm3: len, 指明buf的长度;

Parm4: flags, 一般置为0。

具体定义：flags一般可以使用如下值

0: 常规操作, 与 read()相同

MSG OOB:指明发送的是带外信息

MSG_PEEK: 可以查看可读的信息，在接收数据后不会将这些数据丢失

Param5:src_addr 用于保存发送主机地址

Param6:addrlen 用于保存发送主机地址的长度

Return：成功返回接收的字节数，失败返回-1；

# 课堂示例

要求：使用 AF_INET 实现 UDP 点对点通信

server设计

```c
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>

#define MAX_RDBUF_LEN 1024

int main(void)
{
	int server_fd, ret, rd_cnt;
	struct sockaddr_in server_addr, client_addr;
	socklen_t server_len = sizeof(server_addr);
	socklen_t client_len = sizeof(client_addr);
	char rd_buf[MAX_RDBUF_LEN];

	server_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (server_fd == -1) {
		perror("socket fail");
		exit(1);
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(8090);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	ret = bind(server_fd, (struct sockaddr *)&server_addr, server_len);
	if (ret == -1) {
		perror("bind fail");
		exit(2);
	}

	while (1) {
		rd_cnt = recvfrom(server_fd, rd_buf, MAX_RDBUF_LEN, 0,
						  (struct sockaddr *)&client_addr, &client_len);
		if (rd_cnt == -1) {
			perror("receive client data fail");
			exit(3);
		}

		printf("received client: %s data:%s\n", inet_ntoa(client_addr.sin_addr), rd_buf);
		// ack client
		sendto(server_fd, "yes, byebye.", 12, 0,
			   (struct sockaddr *)&client_addr, client_len);
	}

	close(server_fd);
	return 0;
}
```


client设计


```c
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>

#define MAXBUF_CNT 1024

int main(void)
{
	int client_fd, ret, wr_cnt;
	struct sockaddr_in server_addr;
	socklen_t server_len;
	char send_buf[MAXBUF_CNT];
	char recv_buf[MAXBUF_CNT];

	client_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if (client_fd == -1) {
		perror("socket fail");
		exit(1);
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(8090);
	server_addr.sin_addr.s_addr = inet_addr("192.168.0.103");
	server_len = sizeof(struct sockaddr_in);

	memset(send_buf, 0, MAXBUF_CNT);
	strcpy(send_buf, "hi, udp server.");
	wr_cnt = sendto(client_fd, send_buf, strlen(send_buf) + 1, 0,
					(struct sockaddr *)&server_addr, server_len);
	if (wr_cnt == -1) {
		perror("sendto fail");
		exit(2);
	}

	memset(recv_buf, 0, MAXBUF_CNT);
	recvfrom(client_fd, recv_buf, MAXBUF_CNT, 0, NULL, NULL);
	printf("receive server data:%s\n", recv_buf);

	close(client_fd);
	return 0;
}
```

练习：编写代码，实现 UDP 的一对多通信（UDP 广播）。

