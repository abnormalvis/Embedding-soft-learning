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
#define DOWNLOAD_DIR "./downloads/"

// 从服务器接收文件
int receive_file(int client_fd, const char *expected_filename)
{
    char file_buf[MAX_CNT];
    int write_cnt;
    long file_size = 0;
    
    // 先接收文件大小
    read(client_fd, &file_size, sizeof(file_size));
    printf("File size: %ld bytes\n", file_size);
    
    if (file_size <= 0)
    {
        printf("Invalid file size\n");
        return -1;
    }
    
    // 构建保存路径
    char filepath[256];
    snprintf(filepath, sizeof(filepath), "%s%s", DOWNLOAD_DIR, expected_filename);
    
    // 创建下载目录
    mkdir(DOWNLOAD_DIR, 0755);
    
    // 打开文件准备写入
    int file_fd = open(filepath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (file_fd == -1)
    {
        perror("open file fail");
        return -1;
    }
    
    printf("Saving to: %s\n", filepath);
    
    // 接收文件内容
    long total_received = 0;
    while (total_received < file_size)
    {
        int to_read = (file_size - total_received) > MAX_CNT ? MAX_CNT : (file_size - total_received);
        write_cnt = read(client_fd, file_buf, to_read);
        if (write_cnt <= 0)
        {
            perror("read file data fail");
            close(file_fd);
            return -1;
        }
        write(file_fd, file_buf, write_cnt);
        total_received += write_cnt;
        printf("\rProgress: %ld/%ld bytes (%.1f%%)", total_received, file_size, 
               (double)total_received / file_size * 100);
        fflush(stdout);
    }
    
    close(file_fd);
    printf("\nFile received successfully!\n");
    return 0;
}

// 显示菜单
void show_menu()
{
    printf("\n========== 文件下载系统 ==========\n");
    printf("请选择要下载的文件:\n");
    printf("  1 - 下载 TXT 文本文件\n");
    printf("  2 - 下载图片文件 (BMP)\n");
    printf("  3 - 下载视频文件 (MP4)\n");
    printf("  q - 退出程序\n");
    printf("===================================\n");
    printf("请输入命令：");
    fflush(stdout);
}

int main(int argc, char *argv[])
{
    int client_fd;
    struct sockaddr_in server_addr;
    char rd_buf[MAX_CNT] = {0};
    int ret;

    // 创建 socket 文件描述符
    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd == -1)
    {
        perror("socket fail");
        exit(1);
    }

    // 初始化服务器地址结构
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // 连接服务器
    ret = connect(client_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if(ret == -1)
    {
        perror("connect fail");
        exit(2);
    }
    printf("---- connect server success.\n");
    
    // 创建下载目录
    mkdir(DOWNLOAD_DIR, 0755);
    
    // 交互式命令输入
    char input_cmd[64];
    show_menu();
    
    while (1)
    {
        memset(input_cmd, 0, sizeof(input_cmd));
        
        // 读取用户输入
        if (fgets(input_cmd, sizeof(input_cmd), stdin) == NULL)
        {
            break;
        }
        
        // 去除换行符
        input_cmd[strcspn(input_cmd, "\r\n")] = 0;
        
        // 检查是否退出
        if (strcmp(input_cmd, "q") == 0 || strcmp(input_cmd, "Q") == 0)
        {
            printf("Exiting...\n");
            break;
        }
        
        // 验证命令
        if (strcmp(input_cmd, "1") != 0 && 
            strcmp(input_cmd, "2") != 0 && 
            strcmp(input_cmd, "3") != 0)
        {
            printf("无效命令，请重新输入！\n");
            show_menu();
            continue;
        }
        
        // 发送命令到服务器
        ret = write(client_fd, input_cmd, strlen(input_cmd));
        if (ret == -1)
        {
            perror("write fail");
            break;
        }
        printf("Command sent: %s\n", input_cmd);
        
        // 接收服务器响应
        memset(rd_buf, 0, MAX_CNT);
        ret = read(client_fd, rd_buf, MAX_CNT - 1);
        if (ret > 0)
        {
            rd_buf[ret] = '\0';
            printf("Server response: %s\n", rd_buf);
            
            // 如果服务器开始发送文件
            if (strstr(rd_buf, "SENDING_FILE") != NULL)
            {
                const char *filename = NULL;
                if (strcmp(input_cmd, "1") == 0)
                    filename = "downloaded_file.txt";
                else if (strcmp(input_cmd, "2") == 0)
                    filename = "downloaded_image.bmp";
                else if (strcmp(input_cmd, "3") == 0)
                    filename = "downloaded_video.mp4";
                
                receive_file(client_fd, filename);
                
                printf("\n文件下载完成！保存在：%s%s\n", DOWNLOAD_DIR, filename);
            }
            else
            {
                printf("Error: %s\n", rd_buf);
            }
        }
        else if (ret == 0)
        {
            printf("Server closed connection.\n");
            break;
        }
        else
        {
            perror("read fail");
            break;
        }
        
        show_menu();
    }
    
    close(client_fd);
    printf("---- client closed.\n");
    return 0;
}