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
#include <sys/time.h>
#include <sys/resource.h>

#define MAXNUM 100
#define MAXEPOLLSIZE 10000

/*
 * setnonblocking - 设置句柄为非阻塞方式
 * */
int setnonblocking(int sockfd)
{
	if (fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0) | O_NONBLOCK) == -1)
		return -1;

	return 0;
}

int main()
{
	int listen_fd, client_fd, rdCnt;	
	struct sockaddr_in saddr = {0};
	struct sockaddr_in caddr = {0};

	socklen_t slen, clen; 

	char rd_buffer[128] = {0};
	char cmd[4] = {0};  


	struct epoll_event ev;
	struct epoll_event events[MAXEPOLLSIZE];
	
	int kdpfd, nfds, curfds;

	listen_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(listen_fd < 0)
	{
		perror("socket fail");
		return -1; 
	}

	printf("sfd: %d\n", listen_fd);

	/*设置句柄为非阻塞方式*/
//	setnonblocking(listen_fd);

	saddr.sin_family = AF_INET; 
	saddr.sin_port = htons(8990);
	saddr.sin_addr.s_addr = htons(INADDR_ANY); 
	slen = sizeof(saddr);

	if(bind(listen_fd, (struct sockaddr *)&saddr, slen)<0)
	{
		perror("bind fail\n");
		return -1; 
	}

	printf("bind success.\n");

	listen(listen_fd, MAXNUM);
	printf("begin listen....\n");

	/* 创建 epoll 句柄，把监听 socket 加入到 epoll 集合里 */
	kdpfd = epoll_create(MAXEPOLLSIZE);
	
	ev.events = EPOLLIN | EPOLLET;
	ev.data.fd = listen_fd;

	if (epoll_ctl(kdpfd, EPOLL_CTL_ADD, listen_fd, &ev) < 0)
	{

		fprintf(stderr, "epoll set insertion error: fd=%d\n", listen_fd);
		return -1;
	}
	else
	{

		printf("监听 socket 加入 epoll 成功！\n");
	}

	curfds = 1;

	clen = sizeof(struct sockaddr_in);

	while(1)
	{
		nfds = epoll_wait(kdpfd, events, curfds, -1);
		if(nfds == -1)
		{
			perror("epoll_wait\n");
			break;
		}

		for(int n = 0; n < nfds; ++n)
		{
			if(events[n].data.fd == listen_fd)
			{
				client_fd = accept(listen_fd, (struct sockaddr *)&caddr, (socklen_t *)&clen);
				if(client_fd < 0)
				{
					perror("accept fail\n");
					return -1; 
				}

				printf("client fd:%d\n", client_fd);

				printf("ip: %s\n", inet_ntoa(caddr.sin_addr));
				printf("port: %d\n", ntohs(caddr.sin_port));

				//setnonblocking(client_fd);
				ev.events = EPOLLIN | EPOLLET;
				ev.data.fd = client_fd;

				if(epoll_ctl(kdpfd, EPOLL_CTL_ADD, client_fd, &ev) < 0)
				{
					printf("client_fd add epoll failed\n");
					return -1;
				}
				curfds++;
				printf("curfds:%d\n", curfds);
			}
			else
			{
				int ret = recv(events[n].data.fd, cmd, 4, 0);
				if(ret < 0)
				{
					printf("recv failed\n");
					return -1;
				}
				else if(ret == 0)
				{
					printf("client exit...\n");
					close(events[n].data.fd);
					epoll_ctl(kdpfd, EPOLL_CTL_DEL, events[n].data.fd, &ev);
					curfds--;
					printf("curfds:%d\n", curfds);
				}
				else
					printf("cmd: %s\n", cmd);
			}
		}
	}


	close(listen_fd);


	return 0; 
}










