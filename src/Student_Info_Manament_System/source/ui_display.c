
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ui_display.h"
#include "login.h"
#include "register.h"

void ui_display(void)
{
    char buf[32];
    int choice = 0;

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
            login_flow();
            break;
        case 2:
            register_flow();
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
