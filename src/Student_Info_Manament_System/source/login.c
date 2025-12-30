#include "login.h"
#include "stdio.h"
#include <string.h>
#include "config.h"
#include "ui_display.h"

/*
登陆需求分析：
- 用户输入用户名和密码
- 读取文件，验证用户名和密码是否匹配
- 成功则进入系统，失败则提示错误并返回主界面
*/

// 清理输入缓冲区
static void clear_input_buffer(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void login_flow(studentInfo *stuinfo, studentInfo *stutemp)
{
    char username[16];
    char password[16];
    
    Login_Display();
    
    printf("\n请输入用户名：");
    scanf("%15s", username);
    clear_input_buffer(); // 清理输入缓冲区
    
    printf("请输入密码：");
    scanf("%15s", password);
    clear_input_buffer(); // 清理输入缓冲区
    
    // 保存输入的用户名和密码到stutemp
    strncpy(stutemp->stuaccout_.user, username, sizeof(stutemp->stuaccout_.user) - 1);
    stutemp->stuaccout_.user[sizeof(stutemp->stuaccout_.user) - 1] = '\0';
    strncpy(stutemp->stuaccout_.password, password, sizeof(stutemp->stuaccout_.password) - 1);
    stutemp->stuaccout_.password[sizeof(stutemp->stuaccout_.password) - 1] = '\0';
    
    // 验证登录
    int result = login_judge(stutemp);
    
    if (result == SUCCESS)
    {
        User_Login_Success_Display();
        // 将登录信息复制到stuinfo
        memcpy(stuinfo, stutemp, sizeof(studentInfo));
        
        // 显示用户主菜单并等待用户操作
        User_Main_Menu_Display();
        
        // 简单的用户菜单交互
        char buf[32];
        int choice;
        printf("\n请选择操作 (1-3)：");
        if (fgets(buf, sizeof(buf), stdin) != NULL)
        {
            if (sscanf(buf, "%d", &choice) == 1)
            {
                switch (choice)
                {
                case 1:
                    printf("\n=== 个人信息 ===\n");
                    printf("用户名：%s\n", stuinfo->stuaccout_.user);
                    printf("(更多功能开发中...)\n");
                    break;
                case 2:
                    printf("\n修改个人信息功能开发中...\n");
                    break;
                case 3:
                    printf("\n退出登录，返回主菜单。\n");
                    break;
                default:
                    printf("\n无效选项。\n");
                    break;
                }
            }
        }
    }
    else
    {
        Login_Failed_Display();
        printf("\n账号或密码错误！如果还没有账号，请先注册。\n");
    }
}

int login_judge(studentInfo *stuinfo)
{
    FILE *fp = fopen(DATA_FILE, "r");
    if (fp == NULL)
    {
        printf("无法打开数据文件，可能还没有注册用户。\n");
        return FAILURE;
    }
    
    studentInfo temp;
    int found = 0;
    
    // 读取文件中的所有用户信息并进行比对
    while (fread(&temp, sizeof(studentInfo), 1, fp) == 1)
    {
        if (strcmp(temp.stuaccout_.user, stuinfo->stuaccout_.user) == 0 &&
            strcmp(temp.stuaccout_.password, stuinfo->stuaccout_.password) == 0)
        {
            found = 1;
            // 将完整的用户信息复制回stuinfo
            memcpy(stuinfo, &temp, sizeof(studentInfo));
            break;
        }
    }
    
    fclose(fp);
    
    return found ? SUCCESS : FAILURE;
}