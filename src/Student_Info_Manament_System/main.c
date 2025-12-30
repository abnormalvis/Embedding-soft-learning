#include "global.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "login.h"
#include "register.h"
#include "ui_display.h"
#include "config.h"

int main(void)
{
    // UI_Display已经包含了循环逻辑
    UI_Display();
    
    printf("\n感谢使用学生信息管理系统！再见！\n");
    return 0;
}