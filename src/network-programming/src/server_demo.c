//
// Created by idris-24-04 on 2026/3/16.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>

#define SERVER_PORT 9999
#define MAX_CNT 1024
#define FILES_DIR "./files/"

// 发送文件到客户端
int send_file(int client_fd, const char *filename)
{
    int file_fd;
    char file_buf[MAX_CNT];
    int read_cnt;
    struct stat file_stat;
    
    // 构建完整文件路径
    char filepath[256];
    snprintf(filepath, sizeof(filepath), "%s%s", FILES_DIR, filename);
    
    // 打开文件
    file_fd = open(filepath, O_RDONLY);
    if (file_fd == -1)
    {
        perror("open file fail");
        return -1;
    }
    
    // 获取文件大小
    if (fstat(file_fd, &file_stat) == -1)
    {
        perror("get file size fail");
        close(file_fd);
        return -1;
    }
    
    printf("Sending file: %s, size: %ld bytes\n", filepath, file_stat.st_size);
    
    // 先发送文件大小
    long file_size = file_stat.st_size;
    write(client_fd, &file_size, sizeof(file_size));
    
    // 读取并发送文件内容
    while ((read_cnt = read(file_fd, file_buf, MAX_CNT)) > 0)
    {
        write(client_fd, file_buf, read_cnt);
    }
    
    close(file_fd);
    printf("File sent successfully: %s\n", filepath);
    return 0;
}

// 处理客户端命令
int handle_command(int client_fd, char *cmd)
{
    int ret = 0;
    
    // 去除换行符
    cmd[strcspn(cmd, "\r\n")] = 0;
    
    printf("Received command: %s\n", cmd);
    
    if (strcmp(cmd, "1") == 0)
    {
        // 发送 txt 文件
        printf("Client requested TXT file\n");
        write(client_fd, "SENDING_FILE", 12);
        ret = send_file(client_fd, "test.txt");
    }
    else if (strcmp(cmd, "2") == 0)
    {
        // 发送图片文件
        printf("Client requested IMAGE file\n");
        write(client_fd, "SENDING_FILE", 12);
        ret = send_file(client_fd, "test_image.bmp");
    }
    else if (strcmp(cmd, "3") == 0)
    {
        // 发送视频文件
        printf("Client requested VIDEO file\n");
        write(client_fd, "SENDING_FILE", 12);
        ret = send_file(client_fd, "test_video.mp4");
    }
    else
    {
        // 未知命令
        char error_msg[MAX_CNT];
        snprintf(error_msg, sizeof(error_msg), "Unknown command: %s", cmd);
        write(client_fd, error_msg, strlen(error_msg));
        ret = -1;
    }
    
    return ret;
}

int main(int argc, char *argv[])
{
    int listen_fd;
    int connect_fd;
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    socklen_t client_len;
    char rd_buf[MAX_CNT] = {0};
    int rd_cnt;
    int ret;

    // 创建 socket 文件描述符
    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd == -1)
    {
        perror("socket fail");
        exit(1);
    }

    // 初始化服务器地址结构
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // 绑定 socket 和地址
    ret = bind(listen_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (ret == -1)
    {
        perror("bind fail");
        close(listen_fd);
        exit(2);
    }
    printf("---- bind success.\n");

    // 监听 socket
    ret = listen(listen_fd, 5);
    if (ret == -1)
    {
        perror("listen fail");
        close(listen_fd);
        exit(3);
    }
    printf("---- listen success. Waiting for client...\n");

    while(1)
    {
        // 接受客户端连接
        client_len = sizeof(client_addr);
        connect_fd = accept(listen_fd, (struct sockaddr *)&client_addr, &client_len);
        if(connect_fd == -1)
        {
            perror("accept fail");
            continue;
        }
        printf("connect success.\n");
        printf("client ip: %s\n", inet_ntoa(client_addr.sin_addr));
        printf("client port: %d \n", ntohs(client_addr.sin_port));
        
        while(1)
        {
            memset(rd_buf, 0, MAX_CNT);
            rd_cnt = read(connect_fd, rd_buf, MAX_CNT - 1);
            if(rd_cnt == 0)
            {
                printf("client exit\n");
                break;
            }
            else if(rd_cnt < 0)
            {
                perror("read fail");
                break;
            }
            printf("+++read data: %s\n", rd_buf);
            
            // 处理客户端命令
            handle_command(connect_fd, rd_buf);
        }
        close(connect_fd);
        printf("--- client connection closed.\n");
    }
    close(listen_fd);
    return 0;
}