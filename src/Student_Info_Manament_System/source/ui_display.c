
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ui_display.h"
#include "login.h"
#include "register.h"
#include "global.h"
#include "config.h"

// 打印装饰性分隔线
static void print_line(const char *color)
{
    printf("%s", color);
    printf("========================================================\n");
    printf("%s", COLOR_RESET);
}

void UI_Display(void)
{
    char buf[32];
    int choice = 0;
    studentInfo stuinfo;
    studentInfo stutemp;

    while (1)
    {
        printf("\n");
        print_line(COLOR_CYAN);
        printf("%s          ✦ 学生信息管理系统 ✦          %s\n", COLOR_CYAN, COLOR_RESET);
        print_line(COLOR_CYAN);
        printf("%s", COLOR_WHITE);
        printf("  1) 🔑 登 录\n");
        printf("  2) 📝 注 册\n");
        printf("  3) 🚪 退 出\n");
        print_line(COLOR_CYAN);
        printf("%s请选择 (1-3)：%s", COLOR_GREEN, COLOR_RESET);

        if (fgets(buf, sizeof(buf), stdin) == NULL)
        {
            printf("\n输入错误，退出。\n");
            break;
        }

        if (sscanf(buf, "%d", &choice) != 1)
        {
            printf("%s无效输入，请输入数字 1-3。%s\n", COLOR_RED, COLOR_RESET);
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
            printf("%s\n谢谢使用，再见！%s\n", COLOR_YELLOW, COLOR_RESET);
            return;
        default:
            printf("%s请选择有效选项 1-3。%s\n", COLOR_RED, COLOR_RESET);
            break;
        }
    }
}
void Register_Display(void)
{
    printf("\n");
    print_line(COLOR_GREEN);
    printf("%s         📝 账号注册         %s\n", COLOR_GREEN, COLOR_RESET);
    print_line(COLOR_GREEN);
}

void Login_Display(void)
{
    printf("\n");
    print_line(COLOR_BLUE);
    printf("%s         🔑 用户登录         %s\n", COLOR_BLUE, COLOR_RESET);
    print_line(COLOR_BLUE);
}

void Register_Success_Display(void)
{
    printf("\n");
    print_line(COLOR_GREEN);
    printf("%s         ✓ 注册成功         %s\n", COLOR_GREEN, COLOR_RESET);
    print_line(COLOR_GREEN);
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
    printf("\n");
    print_line(COLOR_RED);
    printf("%s         ✗ 登录失败         %s\n", COLOR_RED, COLOR_RESET);
    print_line(COLOR_RED);
}

void Root_Main_Menu_Display(void)
{
    printf("\n");
    print_line(COLOR_CYAN);
    printf("%s         根主菜单         %s\n", COLOR_CYAN, COLOR_RESET);
    print_line(COLOR_CYAN);
}

void User_Login_Success_Display(void)
{
    printf("\n");
    print_line(COLOR_GREEN);
    printf("%s         ✓ 登录成功         %s\n", COLOR_GREEN, COLOR_RESET);
    print_line(COLOR_GREEN);
}

void User_Main_Menu_Display(void)
{
    printf("\n");
    print_line(COLOR_BLUE);
    printf("%s         用户主菜单         %s\n", COLOR_BLUE, COLOR_RESET);
    print_line(COLOR_BLUE);
    printf("  1) 查看个人信息\n");
    printf("  2) 修改个人信息\n");
    printf("  3) 退出登录\n");
    print_line(COLOR_BLUE);
}