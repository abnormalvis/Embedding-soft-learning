
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ui_display.h"
#include "login.h"
#include "register.h"
#include "global.h"

void UI_Display(void)
{
    char buf[32];
    int choice = 0;
    studentInfo stuinfo;
    studentInfo stutemp;

    while (1)
    {
        printf("\n================ 学生信息管理 系统 ================\n");
        printf("1) 登 录\n");
        printf("2) 注 册\n");
        printf("3) 退 出\n");
        printf("请选择 (1-3)：");

        if (fgets(buf, sizeof(buf), stdin) == NULL)
        {
            printf("\n输入错误，退出。\n");
            break;
        }

        if (sscanf(buf, "%d", &choice) != 1)
        {
            printf("无效输入，请输入数字 1-3。\n");
            continue;
        }

        switch (choice)
        {
        case 1:
            login_flow(&stuinfo, &stutemp);
            break;
        case 2:
            Register_Flow(&stuinfo, &stutemp);
            break;
        case 3:
            printf("退出程序。\n");
            return;
        default:
            printf("请选择有效选项 1-3。\n");
            break;
        }
    }
}
void Register_Display(void)
{
    printf("\n=================== 账号注册 ===================\n");
}

void Login_Display(void)
{
    printf("\n=================== 用户登录 ===================\n");
}

void Register_Success_Display(void)
{
    printf("\n=================== 注册成功 ===================\n");
}

void Login_Input(void)
{
    printf("\n=================== 登录输入 ===================\n");
}

void Root_Menu_Display(void)
{
    printf("\n=================== 根菜单 ===================\n");
}

void Root_Register_Request_Display(void)
{
    printf("\n=================== 根注册请求 ===================\n");
}

void Login_Failed_Display(void)
{
    printf("\n=================== 登录失败 ===================\n");
}

void Root_Main_Menu_Display(void)
{
    printf("\n=================== 根主菜单 ===================\n");
}

void User_Login_Success_Display(void)
{
    printf("\n=================== 登录成功 ===================\n");
}

void User_Main_Menu_Display(void)
{
    printf("\n=================== 用户主菜单 ===================\n");
    printf("1) 查看个人信息\n");
    printf("2) 修改个人信息\n");
    printf("3) 退出登录\n");
    printf("================================================\n");
}