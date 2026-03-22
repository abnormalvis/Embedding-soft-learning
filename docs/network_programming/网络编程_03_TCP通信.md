# 3. TCP基本通信

socket 起源于 UNIX，在 Unix 一切皆文件哲学的思想下，socket 是一种"打开一读/写一关闭"模式的实现，服务器和客户端各自维护一个"文件"，在建立连接打开后，可以向自己文件写入内容供对方读取或者读取对方内容，通讯结束时关闭文件。

# 3.1 TCP 介绍

传输控制协议（TCP，Transmission Control Protocol）是一种面向连接的、可靠的、基于字节流的传输层通信协议。

UDP是User Datagram Protocol的简称，中文名是用户数据报协议，是OSI（Open System Interconnection，开放式系统互联）参考模型中一种无连接的传输层协议，提供面向事务的简单不可靠信息传送服务，提供了一种无需建立连接就可以发送封装的IP数据包的方法。

# 3.2 TCP 编程模型

![image](https://cdn-mineru.openxlab.org.cn/result/2026-03-20/972edad8-2f03-4bde-a593-d3f177302fb8/48402ca3b2073e16ee228d56d695bb4b5504cefeb6425cf4234b98e1fe827681.jpg)



编程模型简解：



整个TCP通信过程主要包括以下三个阶段：


阶段一：建立连接，主要是通过三次握手实现

a. TCP 服务器建立

初始化 Socket，然后与端口绑定 (bind)，对端口进行监听 (listen)，调用 accept 阻塞，等待客户端连接

b. TCP 客户端建立

初始化一个 Socket，然后连接服务器.connect)，如果连接成功，这时客户端与服务器端的连接就建立了。

阶段二：收发数据，主要是通过阻塞实现

建立好连接之后，服务端与客户端就可以进行正常的数据收发操作了，主要分为以下两种：

第一种：服务器调用接收函数阻塞，等待客户端发送数据过来，再进行处理

第二种：客户端调用接收函数阻塞，等待服务端发送数据过来，再进行处理

阶段三：断开连接，主要是通过四次挥手实现

断开连接，也有两种情况：

第一种：服务端正在阻塞接收，客户端关闭，会给服务端发送一个0数据

第二种：客户端正在阻塞接收，服务端关闭，会给客户端发送一个0数据

# 3.3 TCP 连接建立

# 3.3.1 套接字函数

```c
// api1: socket 用于创建一个 socket 描述符
int socket(int __domain, int __type, int __protocol);
```

Pam1: __domain, 即协议域, 又称为协议族 (family)。常用的协议族有

AF_INET (IPV4) 代表使用 ipv4 地址

AF_INET6 (IPV6) 代表使用 ipv6 地址

AF_LOCAL（或称 AF_UNIX）代表 Unix 系统本地通信

协议族决定了socket的地址类型，在通信中必须采用对应的地址，如AF_INET决定了要用ipv4地址（32位的）与端口号（16位的）的组合、AF_UNIX决定了要用一个绝对路径名作为地址。

Parm2: 指定 socket 类型。常用的 socket 类型有

SOCK_STREAM ----流式套接字，应用于传输层的TCP

SOCK_DGRAM ---- 数据报式套接字，应用于传输层的 UDP

SOCKRaw ---- 原始套接字，工作在网络层的IP

SOCK_PACKET ----内核不网络数据包操作，直接从网卡协议栈交给用户等等

Parm3: protocol: 故名思意, 就是指定协议。常用的协议有

IPPROTO_TCP：TCP传输协议

IPPTOTOUDP：UDP传输协议

IPPROTO_SCTP: STCP 传输协议

IPPROTO_TIPC：TIPC传输协议

等等

Return: 返回一个 socket_fd 描述符，代表着一个网络设备文件，后面对网络设备进行读写操作需要用到。

# 注意：

并不是上面的 type 和 protocol 可以随意组合的，如 SOCK_STREAM 不可以跟 IPPROTOUDP 组合。

如果不清楚它们间的搭配，可以取protocol为o，此时会自动选择type类型对应的默认协议。

# api2. Bind函数，把一个地址族中的特定地址赋给socket

```c
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen); 
```

Parm1: sockfd, 即 socket 描述字, 它是通过 socket()函数创建了, 唯一标识一个 socket。

Parm2: addr，一个 const struct sockaddr *指针，指向要绑定给 socketfd 的协议地址。

这个是公共结构体


struct sockaddr


```c
struct sockaddr {
	...
	char sa_data[14]; /* Address data. */
};
```


这个地址结构根据地址创建socket时的地址协议族的不同而不同，如ipv4对应的是：


```c
#include <netinet/in.h>

struct sockaddr_in {
	sa_family_t sin_family;   // IP地址所属的协议栈
	in_port_t sin_port;       // 以网络字节序存储的端口号
	struct in_addr sin_addr;  // IP地址
};

struct in_addr {
	uint32_t s_addr;          // 以网络字节序存储的IP地址
};
```

Param3: addrlen: 对应的地址长度

Return: 成功返回 0, 失败返回 -1

注意：

如果 socket 套接字要绑定的是整个网络，则可用 INADDR_ANY 宏，均定义在 netinet/in.h

api3.listen函数，开始监听绑定的端口及IP

```c
int listen(int sockfd, int backlog);
```

Parm1: Sockfd, 要监听的 socket 描述字

Parm2:Backlog，可以排队的最大客户端连接个数

Return: 成功返回 0, 失败返回 -1

# 注意：

连接客户端连接情况不同，放置在不同的队列中。

Api4.accept()函数，进入阻塞状态，等待客户端来连接

```c
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
```

Parm1: Sockfd, 监听套接字

Parm2: Addr，用于存储连接服务端的客户端的地址。如果对客户端地址不感兴趣，那么可以把这个值设置为 NULL。

Parm3: AddrLen，用来保存上述 addr 结构的大小，它指明 addr 结构所占有的字节个数。同样的，它也可以被设置为 NULL。

Return: 成功返回连接服务器的客户端的套接字；失败返回-1

注意：

accept 默认会阻塞进程，直到有一个客户连接建立后返回，它返回的是一个新可用的套接字，这个套接字是连接套接字。

问题：

至此，我们接触到了两种套接字，监听套接字及连接套接字，那请同学思考并回答以下问题

1. 在通信过程中，为什么会设计两种套接字，它们分别具有什么特点？

2. 两种套接字的生命周期分别是怎样？

api5.connect()函数，用于与服务端建立连接

```c
int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```

Parm1: Sockfd: 客户端的 socket 描述字

Parm2: Addr: 服务器的 socket 地址

Parm3:Addrlen:socket地址的长度

Return：成功返回0，失败返回-1

api6. htons等函数，用于网络字节序与主机字节序地址间转换

广州小蚁智控科技有限公司 $\bullet$ 创训工场

# 头文件

```c
#include <arpa/inet.h>
```

# 原型

```c
uint32_t htonl uint32_t hostlong);   
uint16_t htons uint16_t hostshort);   
uint32_t ntohl uint32_t netlong);   
uint16_t ntohs uint16_t netshort); 
```

# 参数

```text
hostlong/hostshort: 主机字节序地址
netlong/netshort: 网络字节序地址
```

# 返回值

主机字节序或者网络字节序

api7.INET_addr/inet_aton函数，用于带点IP地址与网络字节序地址转换

# 头文件

```c
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
```

# 原型

```c
in_addr_t inset_addr(const char *cp); 
```

```c
char *inet_ntoa(struct in_addr in); 
```

# 参数

*cp: 带点号的十进制 IP 地址

in: 网络字节序地址结构体

返回值

in_addr_t: 网络字节序地址

char *: 带点号的十进制 IP 地址

api8. getsockname/getpeername 

api9.INET_ntop 

# 3.3.2 代码设计

server端设计

```c
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>

#define MAX_LISTEN 16

int main(void)
{
	int listen_fd, connect_fd, ret;
	struct sockaddr_in server_addr, client_addr;
	socklen_t server_len = sizeof(server_addr);
	socklen_t client_len = sizeof(client_addr);

	listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_fd == -1) {
		perror("socket fail");
		exit(1);
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(8090);
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

	ret = bind(listen_fd, (struct sockaddr *)&server_addr, server_len);
	if (ret == -1) {
		perror("bind fail");
		exit(2);
	}

	ret = listen(listen_fd, MAX_LISTEN);
	if (ret == -1) {
		perror("listen fail");
		exit(3);
	}

	connect_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &client_len);
	if (connect_fd == -1) {
		perror("accept fail");
		exit(4);
	}

	printf("connect success.\n");
	printf("client ip:%s\n", inet_ntoa(client_addr.sin_addr));
	printf("client port:%d\n", ntohs(client_addr.sin_port));

	close(connect_fd);
	close(listen_fd);
	return 0;
}
```

以下打印客户端发送过来的数据包信息，还可以通过如下方式操作：

还有一种方式：

```c
char server_ip[20];
char client_ip[20];

socklen_t server_len = sizeof(server_addr);
socklen_t client_len = sizeof(client_addr);
getsockname(connect_fd, (struct sockaddr *)&server_addr, &server_len);
getpeername(connect_fd, (struct sockaddr *)&client_addr, &client_len);
inet_ntop(AF_INET, &server_addr.sin_addr, server_ip, sizeof(server_ip));
inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
printf("server:ip=%s,port=%d client:ip=%s,port=%d\n",
	server_ip, ntohs(server_addr.sin_port),
	client_ip, ntohs(client_addr.sin_port));
```


client端设计

```c
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <arpa/inet.h>

int main(void)
{
	int client_fd, ret;
	struct sockaddr_in server_addr;

	client_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (client_fd == -1) {
		perror("socket fail");
		exit(1);
	}

	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(8090);
	server_addr.sin_addr.s_addr = inet_addr("192.168.0.103");

	ret = connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	if (ret == -1) {
		perror("connect fail");
		exit(2);
	}

	close(client_fd);
	return 0;
}
```

# 结果测试

1. 服务器先启动，客户端再启动，可以看到服务器有连接成功信息显示

2. 服务器可以获取到客户端信号，并且打印出，但是要注意，打印的客户端的端口信息时不是我们设定的8090，而是一个动态数值。

主要是客户端发送数据包中，包含了服务器目标机的 IP 及 port 及客户端本身的 IP 及 PORT，但是只有目标机的端口才是 8090，而客户端的端口是动态分配的

# 3.3.3 三次握手

服务器与客户端“连接的建立过程”（三次握手）

TCP 协议通过三个报文段完成连接的建立，这个过程称为三次握手(three-way handshake)，过程如下图所示：

![image](https://cdn-mineru.openxlab.org.cn/result/2026-03-20/972edad8-2f03-4bde-a593-d3f177302fb8/a94d89ca386bc88bed0ce230836fca4f3bdbbe86502d3231611b224d9af03f7d.jpg)


# 详细过程描述如下：

第一次握手：建立连接时，客户端发送 syn 包(syn=j)到服务器，并进入 SYN_SEND 状态，等待服务器确认；SYN：同步序列编号(Synchronize Sequence Numbers)。

第二次握手：服务器收到 syn 包，必须确认客户的 SYN（ack=j+1），同时自己也发送一个 SYN 包（syn=k），即 SYN+ACK 包，此时服务器进入 SYN_RECV 状态；

第三次握手：客户端收到服务器的 SYN+ACK 包，向服务器发送确认包 ACK(ack=k+1)，此包发送完毕，客户端和服务器进入 ESTABLISHED 状态，完成三次握手。

客户端和服务器进入ESTABLISHED状态后，可以进行通信数据交互

总结：

一个完整的三次握手也就是：请求---应答---再次确认。

# 演示：利用抓包工具来跟踪三次握手的过程

1）打开wireshark

2）设置过滤器

$$
i p. a d d r = = 1 9 2. 1 6 8. 1. 5 \& \& t c p. p o r t = = 8 9 8 9
$$

说明开始从 $\mathrm{IP} = 192.168.1.5, \mathrm{port} = 8989$ 中抓包

无法抓取到数据包，为什么？

原来是服务端与客户端同属于一台主机，IP及端口号一样，不好测试，此时我们可以引入window下的telnet工具，它是一个用于远程访问服务器的客户端软件。

# 3）打开windows下的telnet工具

先将server端启动

打开终端，输入 telnet 192.168.1.5 8989，回车

可以看到 wireshark 抓取到三个包，如下图示：

<table><tr><td colspan="3">Filter: ip.addr == 192.168.1.5 &amp;&amp; tcp &amp;&amp; tcp.port == 8989</td><td colspan="2">Expression... Clear Apply Save</td></tr><tr><td>No.</td><td>Time</td><td>Source</td><td>Destination</td><td>Protocol Length Info</td></tr><tr><td>136</td><td>120.090772</td><td>192.168.1.4</td><td>192.168.1.5</td><td>TCP 66 59891 - 8989 [SYN] Seq=0 Win=8192 Len=0 MSS=1460 WS=256 SACK_PERM=1</td></tr><tr><td>137</td><td>120.090971</td><td>192.168.1.5</td><td>192.168.1.4</td><td>TCP 66 8989 - 59891 [SYN, ACK] Seq=0 Ack=1 Win=29200 Len=0 MSS=1460 SACK_PERM=1 WS=128</td></tr><tr><td>138</td><td>120.091171</td><td>192.168.1.4</td><td>192.168.1.5</td><td>TCP 54 59891 - 8989 [ACK] Seq=1 Ack=1 win=525568 Len=0</td></tr></table>

# 3.4 TCP 数据收发

# 3.4.1 套接字函数

sokcet网络套接字的I/O操作函数主要有以下几组：

read()/write() 

recv()/send() 

recvfrom()/sendto() 

$\mathrm{readv() / writev()}$ 

recvmsg()/sendmsg() 

以上几组 IO 操作，功能大同小异，都是用来收发数据，不过推荐大家使用 recvmsg()/sendmsg()函数，这两个函数是最通用的 I/O 函数，实际上可以把上面的其它函数都替换成这两个函数。

下面分别介绍每一组IO操作用法：

api1. Read/write 函数

read函数是负责从sockfd中读取内容到缓冲区buf

write函数将buf中的nbytes字节内容写入sockfd

include <unistd.h> 

Ssize_t read(int sockfd, void *buf, size_t count); 

Ssize_t write(int sockfd, const void buf, size_t count); 

Parm1: Sockfd: 客户端的 socket 描述字

Parm2: buf: 指向要读出或者写入的缓冲区

Parm3: count: 读出或者写入的大小

# Return :

当读成功时，read返回实际所读的字节数

如果返回的值是o表示已经读到文件的结束了

小于o表示出现了错误

如果错误为EINTR说明读是由中断引起的

如果是 ECONNREST 表示网络连接出了问题

当写成功时，返回写的字节数。

write的返回值大于o，表示写了部分或者是全部的数据

返回的值小于o，此时出现了错误

如果错误为EINTR表示在写的时候出现了中断错误。

如果为 EPIPE 表示网络连接出现了问题(对方已经关闭了连接)。

Api2.send/recv函数

recv函数用于从TCP连接的另一端接收数据

send函数用于从TCP连接的一端发送数据给另外一端。

```c
ssize_t send(int sockfd, const void *buf, size_t len, int flags);
ssize_t recv(int sockfd, void *buf, size_t len, int flags); 
```

Parm1: Sockfd, 指定接收端套接字描述符;

Parm2:buf， 指明一个缓冲区，该缓冲区用来存放recv函数接收到的数据；

Parm3: len, 指明buf的长度;

Parm4: flags, 一般置为 0。

对于 send:

0: 与 write()无异

MSG_DONTROUTE:告诉内核，目标主机在本地网络，不用查路由表

MSG DONTWAIT:将单个I/O操作设置为非阻塞模式

MSG_OOB:指明发送的是带外信息

对于recv:

0: 常规操作, 与 read()相同

MSG_DONTWAIT:将单个I/O操作设置为非阻塞模式

MSG_OOB:指明发送的是带外信息

MSG_PEEK: 可以查看可读的信息，在接收数据后不会将这些数据丢失

MSG_WAITALL:通知内核直到读到请求的数据字节数时，才返回。

Return：成功返回接收的字节数，失败返回-1；

其他的IO操作函数，留待后面再做介绍。

# 3.4.2 代码设计


server端设计

```c
int main()
{
	while (1) {
		connect_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &client_len);
		if (connect_fd == -1) {
			perror("accept fail");
			exit(4);
		}

		printf("connect success.\n");
		printf("client ip:%s\n", inet_ntoa(client_addr.sin_addr));
		printf("client port:%d\n", ntohs(client_addr.sin_port));

		while (1) {
			rd_cnt = read(connect_fd, rd_buf, 1024);
			if (rd_cnt == 0) {
				printf("\\client exit\n");
				break;
			}
			printf("+++read data: %s\n", rd_buf);
			write(connect_fd, "receive data ok, byebye.\n", 25);
		}

		close(connect_fd);
	}

	close(listen_fd);
}
```

client端设计

```c
int main()
{
	ret = connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	if (ret == -1) {
		perror("connect fail");
		exit(2);
	}

	// 延时发送数据给服务器
	sleep(4);
	write(client_fd, "helloworld", 10);
	printf("---- client write end.\n");

	// 阻塞接收服务器回发数据
	read(client_fd, rd_buf, MAX_CNT);
	close(client_fd);
}
```

# 3.5 TCP 连接断开

# 3.5.1 套接字函数

close()函数

在服务器与客户端建立连接之后，会进行一些读写操作，完成了读写操作就要关闭相应的 socket 描述字，好比操作完打开的文件要调用 fclose 关闭打开的文件。

```c
#include <unistd.h>
int close(int fd);
```

使用完socket套接字后一定要记得关闭，否则分配的这个套接字会一直得到内核的维护，而新的连接会分配新的套接字符，从而造成资源的浪费，甚至最终无套接字可用。

服务器与客户端，如果有一方调用了close关闭连接，则对方可以接收到一个0个字节，从而也可以根据接收的值是否为0个字节来判断对方是否断开了连接。

# 演示：关闭套接字

不关闭，每次开客户端，都会得到一个新的 socket

关闭之后，每次开客户端，得到的都是同一个字符。

# 3.5.2 代码设计

# 3.4.3 四次挥手

服务器与客户端“连接的终止过程”（四次挥手）

建立一个连接需要三次握手，而终止一个连接要经过四次握手，这是由TCP的半关闭(half-close)造成的。

由于TCP连接是全双工的，因此每个方向都必须单独进行关闭。这个原则是当一方完成它的数据发送任务后就能发送一个FIN来终止这个方向的连接。收到一个FIN只意味着这一方向上没有数据流动，一个TCP连接在收到一个FIN后仍能发送数据。首先进行关闭的一方将执行主动关闭，而另一方执行被动关闭。

过程如下图所示。

![image](https://cdn-mineru.openxlab.org.cn/result/2026-03-20/972edad8-2f03-4bde-a593-d3f177302fb8/ccf77ec555a58b25057f3c054b97bec3e30fc1c9e61a73afbe1e212841e966e4.jpg)


# 详细过程描述如下：

1. 某个应用进程首先调用 close 主动关闭连接，这时 TCP 发送一个 FIN M，同时关闭本端的数据传送。

2. 另一端接收到FINM之后，执行被动关闭，对这个FIN进行确认并回应答信号ACKM+1，同时将文件描述符传递自己所属的应用进程

3.一段时间之后，接收到文件结束符的应用进程调用close关闭它的socket。这导致它的TCP也发送一个FINN;

4. 接收到这个 FIN 的源发送端发送 ACK N+1 对它进行确认，整个连接彻底完成断开。

总结：

一个完整的四次挥手也就是：告知---确认----回复知告---再次确认。

打个简单的例子：

开了一个早餐店，找蒙牛的销售员A订了定期进货的早餐奶。

后来，早餐店要关闭了

a.你打电话给A，说要取消订奶了

B.A 说“知道了”，临时取消了你的订单，同时跟蒙牛销售部作了报告

C.很快，蒙牛销售部确认了取消单，A打电话给你，告知此事

D. 你在接到电话后，说“知道了”

至此，你们的订单彻底取消。

演示：利用抓包工具来跟踪四次挥手的过程

<table><tr><td>16 13.811160</td><td>192.168.1.5</td><td>192.168.1.4</td><td>TCP</td><td>54 8989 + 60149 [FIN, ACK] Seq=1025 Ack=2 Win=229 Len=0</td></tr><tr><td>17 13.811365</td><td>192.168.1.4</td><td>192.168.1.5</td><td>TCP</td><td>54 60149 + 8989 [ACK] Seq=2 Ack=1026 win=2049 Len=0</td></tr><tr><td>18 13.811656</td><td>192.168.1.4</td><td>192.168.1.5</td><td>TCP</td><td>54 60149 + 8989 [FIN, ACK] Seq=2 Ack=1026 win=2049 Len=0</td></tr><tr><td>19 13.812236</td><td>192.168.1.5</td><td>192.168.1.4</td><td>TCP</td><td>54 8989 + 60149 [ACK] Seq=1026 Ack=3 Win=229 Len=0</td></tr></table>

# 3.6 阶段拓展

拓展：实现txt文本的收发

拓展：实现图片数据的收发

拓展：实现视频文件的收发

综合练习

要求：

1. 服务进程启动,客户端连接成功后

2. 客户端连接后,通过按键输入

1+回车：向服务器发送一个命令，获取一个txt文本

2+回车：向服务器发送第二个命令，获取一张图片文

3+回车：向服务器发送第三个命令，获取一个视频

3. 查看并测试下客户端数据是否正确

# 3.7 粘包及自定义协议方案

# 3.7.1 粘包现象出现及解决方案

首先，我们回顾一下TCP和UDP的头部信息：


图1.TCP头部


![image](https://cdn-mineru.openxlab.org.cn/result/2026-03-20/972edad8-2f03-4bde-a593-d3f177302fb8/9b9eaadf4d2cf3d1e2bff5a17133080d8692186be333172943f2f57fe5094653.jpg)



图2.UDP头部


![image](https://cdn-mineru.openxlab.org.cn/result/2026-03-20/972edad8-2f03-4bde-a593-d3f177302fb8/8cd22e69df040be5c29dda921cc2463280863a51d752cd680d744fe31c103f32.jpg)



Checksum of entire UDP segment and pseudo header (parts of IP header)



Please refer to RFC 768 for the complete User Datagram Protocol (UDP) Specification.


我们知道，TCP 和 UDP 是 TCP/IP 协议族传输层中的两个具有代表性的协议。其中，TCP 是面向连接的复杂的、可靠的字节流传输协议，而 UDP 是面向无连接的简单的、不可靠的数据报传输协议。

“流”的概念就是指不间断的数据结构，可以把它想象成你们家里的自来水管道中的水流。什么意思呢？举个例子：TCP 发送端应用程序发送了 10 次 100 字节的消息，

那么，在接收端的应用程序接收到的可能是一个 1000 字节的连续不间断的数据。但如果是 UDP 端口发送了一个 100 字节的消息，那么 UDP 接收端就会以 100 字节的长度来接收数据。正因为这样，我们可以看到 TCP 头部中并没有长度信息，而 UDP 头部包含长度信息。好了，现在你应该明白我们在创建套接字的时候，为什么 type 的类型是 SOCK_STREAM 和 SOCK_DGRAM 了吧！

正如文章标题所述，显然，你已经知道 UDP 数据包存在明确边界，是不存在粘包现象的，只有 TCP 才会出现粘包现象。那么，接下来我们逐步分析 TCP 的流传输特性所带来如粘包这样的一些问题及其解决方案。

按每次通信后是否闭关连接，可以分为两类情况：长连接和短连接。长连接——指的是客户端和服务端先建立通信连接，连接建立后不断开，然后再进行报文发送和接收。短连接——指的是客户端和服务端每进行一次报文收发交易时才进行通信连接，交易完毕后立即断开连接，比如 http 协议。

所以我们可以分析以下几种情况：

（1）如果利用TCP每次发送数据，就与对方建立连接，然后双方发送完一段数据后，就关闭连接，这样就不会出现粘包问题（因为只有一种数据结构）。

（2）如果发送数据无结构，如文件传输，这样发送端只管发送，接收端只管接收存储就行，也不用考虑粘包。

（3）如果双方建立连接，需要在连接后一段时间内发送多个不同结构的数据，这时候接收端收到就可能是一堆粘在一起的数据，这样接收端应用程序就傻了，到底是要干嘛？不知道，因为协议并没有规定这么诡异的数据。

那么，可以认为TCP粘包问题并不是对所有应用都造成困扰的，只是对那些长连接并且需要传输多种数据结构的应用造成影响。仔细分析会发现，除了粘包问题，其实还可能会出现多包、少包、半包、断包等情况。

针对这些问题，一般会有如下解决方法：

(1) 调用发送函数之后都强制数据立即传送 (PUSH 指令)。

（2）对于接收端引起的粘包，则可通过优化程序设计、精简接收进程工作量、提高接收进程优先级等措施，使其及时接收数据，从而尽量避免出现粘包现象。

（3）添加一个固定的消息头，该消息头包含数据长度信息，每次数据时先接收固定大小的消息头，再根据其携带的长度信息接收消息实体。也就是说，通过人为控制多次接收来避免粘包。

(4) 设置 TCP_NODELAY 选项, 禁止 Nagle 算法。

（5）设置SO_RCVBUF和SO_SNDBUF选项，根据应用需求修改一个合适的接收、发送缓冲区大小。

(6) 添加报文分隔标识，比如发送报文是在末尾添加 '\n'，同时接收端使用 recv() 函数接收报文，并且设置参数 flags 的值为 MSG_PEEK。注意：当 flags 参数的值设置为 MSG_PEEK 时，recv() 可以从 socket 缓存中读取数据，但是不会将缓存中该部分数据清除，但如果使用 read() 函数直接读取 socket 缓存区中的内容，会清空缓存区中的内容。假设两段报文粘包，read() 会清空缓存区中所有内容，从而导致后一段报文中的粘包的部分数据丢失。

实际上，上述的几种解决方法都有不足之处，并且不一定能够完全避免 TCP 粘包问题。所以还是需要根据实际应用来进行应用场景和性能方面的衡量。

# 2、粘包的解决方案

本质上是要在应用层维护消息与消息的边界

方式一：定长包。有可能增加了网络的负担，例如：如果定义的发送缓冲区是 100 个字节，而要发送的数据是 10 个字节，则每次都得发送 100 个字节。

方式二：包尾加\r\n（ftp）。如果数据包中含有\r、\n，则需要进行转义等操作。

方式三：非定长的数据包。即包头加上包体长度，这样可以先接收包头（含有数据包长度），再接收包体，有效的降低了网络的负担

方式四：更复杂的应用层协议。

