#include "register.h"
#include <stdio.h>
#include <string.h>
#include "global.h"
#include "config.h"
#include "ui_display.h"

/*
注册需求分析：
- 用户输入用户名和密码
- 确认密码输入一致
- 保存用户信息，进行文件操作
- 回到主界面
*/

// 清理输入缓冲区
static void clear_input_buffer(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void Register_Flow(studentInfo *stuinfo, studentInfo *stutemp)
{
    char username[16];
    char password[16];
    char confirm_pwd[16];
    
    Register_Display();
    
    printf("\n请输入用户名：");
    scanf("%15s", username);
    clear_input_buffer(); // 清理输入缓冲区
    
    // 检查用户名是否已存在
    FILE *fp = fopen(DATA_FILE, "r");
    if (fp != NULL)
    {
        studentInfo temp;
        while (fread(&temp, sizeof(studentInfo), 1, fp) == 1)
        {
            if (strcmp(temp.stuaccout_.user, username) == 0)
            {
                printf("\n用户名已存在，请使用其他用户名！\n");
                fclose(fp);
                return;
            }
        }
        fclose(fp);
    }
    
    printf("请输入密码：");
    scanf("%15s", password);
    clear_input_buffer(); // 清理输入缓冲区
    
    printf("请确认密码：");
    scanf("%15s", confirm_pwd);
    clear_input_buffer(); // 清理输入缓冲区
    
    // 验证两次密码是否一致
    if (strcmp(password, confirm_pwd) != 0)
    {
        printf("\n两次密码输入不一致，注册失败！\n");
        return;
    }
    
    // 保存注册信息到stutemp
    memset(stutemp, 0, sizeof(studentInfo));
    strncpy(stutemp->stuaccout_.user, username, sizeof(stutemp->stuaccout_.user) - 1);
    stutemp->stuaccout_.user[sizeof(stutemp->stuaccout_.user) - 1] = '\0';
    strncpy(stutemp->stuaccout_.password, password, sizeof(stutemp->stuaccout_.password) - 1);
    stutemp->stuaccout_.password[sizeof(stutemp->stuaccout_.password) - 1] = '\0';
    
    // 写入文件
    fp = fopen(DATA_FILE, "a");
    if (fp == NULL)
    {
        printf("\n无法打开数据文件，注册失败！\n");
        return;
    }
    
    if (fwrite(stutemp, sizeof(studentInfo), 1, fp) != 1)
    {
        printf("\n写入文件失败，注册失败！\n");
        fclose(fp);
        return;
    }
    
    fclose(fp);
    
    Register_Success_Display();
    printf("\n注册成功！用户名：%s\n", username);
    printf("请返回主菜单进行登录。\n");
}