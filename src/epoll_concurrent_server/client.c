#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#define MAXNUM 	(100)
#define BUFFER  (1024*1024)

int main()
{
	int ret, fd, rdCnt; 
	int connect_fd;
	char file[16] = {0};  
	struct sockaddr_in saddr = {0};

	socklen_t slen; 

	char rd_Buffer[BUFFER]; 



	connect_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(connect_fd < 0)
	{
		perror("socket fail");
		return -1; 
	}

	printf("connect fd: %d\n", connect_fd);

	saddr.sin_family = AF_INET; 
	saddr.sin_port = htons(8990);
	saddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
	slen = sizeof(saddr);

	if(connect(connect_fd, (struct sockaddr *)&saddr, slen) < 0)
	{
		perror("connect server fail");
		return -1;
	}

	printf("connect server success.\n");
	
	write(connect_fd, "vid", 3);

	sleep(10);

	close(connect_fd);


	return 0; 
}










