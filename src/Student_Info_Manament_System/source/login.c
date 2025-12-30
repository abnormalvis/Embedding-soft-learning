#include "login.h"
#include "stdio.h"
#include <string.h>
#include "config.h"
#include "ui_display.h"
#include "admin.h"
#include "student.h"

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
    
    printf("\n%s请输入用户名：%s", COLOR_GREEN, COLOR_RESET);
    scanf("%15s", username);
    clear_input_buffer();
    
    printf("%s请输入密码：%s", COLOR_GREEN, COLOR_RESET);
    scanf("%15s", password);
    clear_input_buffer();
    
    // 检查是否为管理员登录
    if (strcmp(username, ADMIN_USERNAME) == 0 && 
        strcmp(password, ADMIN_PASSWORD) == 0)
    {
        // 管理员登录
        memset(stutemp, 0, sizeof(studentInfo));
        strncpy(stutemp->stuaccout_.user, username, sizeof(stutemp->stuaccout_.user) - 1);
        strncpy(stutemp->stuaccout_.password, password, sizeof(stutemp->stuaccout_.password) - 1);
        stutemp->stuaccout_.role = ROLE_ADMIN;
        
        User_Login_Success_Display();
        printf("\n%s欢迎管理员：%s%s\n", COLOR_CYAN, username, COLOR_RESET);
        
        memcpy(stuinfo, stutemp, sizeof(studentInfo));
        admin_menu(stuinfo);
        return;
    }
    
    // 普通用户登录
    strncpy(stutemp->stuaccout_.user, username, sizeof(stutemp->stuaccout_.user) - 1);
    stutemp->stuaccout_.user[sizeof(stutemp->stuaccout_.user) - 1] = '\0';
    strncpy(stutemp->stuaccout_.password, password, sizeof(stutemp->stuaccout_.password) - 1);
    stutemp->stuaccout_.password[sizeof(stutemp->stuaccout_.password) - 1] = '\0';
    
    int result = login_judge(stutemp);
    
    if (result == SUCCESS)
    {
        User_Login_Success_Display();
        memcpy(stuinfo, stutemp, sizeof(studentInfo));
        
        printf("\n%s欢迎学生：%s%s\n", COLOR_BLUE, username, COLOR_RESET);
        
        // 进入学生菜单
        student_menu(stuinfo);
    }
    else
    {
        Login_Failed_Display();
        printf("\n%s账号或密码错误！如果还没有账号，请先注册。%s\n", COLOR_RED, COLOR_RESET);
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