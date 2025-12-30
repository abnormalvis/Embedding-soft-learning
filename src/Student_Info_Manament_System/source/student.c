#include "student.h"
#include "config.h"
#include <stdio.h>
#include <string.h>

// 清理输入缓冲区
static void clear_input_buffer(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// 打印分隔线
static void print_line(const char *color)
{
    printf("%s", color);
    printf("========================================================\n");
    printf("%s", COLOR_RESET);
}

// 学生菜单
void student_menu(studentInfo *student)
{
    char buf[32];
    int choice;
    
    while (1)
    {
        printf("\n");
        print_line(COLOR_BLUE);
        printf("%s          学生中心 - 欢迎 %s%s%s          \n", 
               COLOR_BLUE, COLOR_YELLOW, student->stuaccout_.user, COLOR_BLUE);
        print_line(COLOR_BLUE);
        printf("%s", COLOR_WHITE);
        printf("  1) 设置/更新个人信息\n");
        printf("  2) 查看我的成绩\n");
        printf("  3) 退出登录\n");
        print_line(COLOR_BLUE);
        printf("%s请选择操作 (1-3): %s", COLOR_GREEN, COLOR_RESET);
        
        if (fgets(buf, sizeof(buf), stdin) == NULL)
            break;
            
        if (sscanf(buf, "%d", &choice) != 1)
        {
            printf("%s无效输入，请输入数字 1-3。%s\n", COLOR_RED, COLOR_RESET);
            continue;
        }
        
        switch (choice)
        {
            case 1:
                set_student_info(student);
                break;
            case 2:
                view_my_scores(student);
                break;
            case 3:
                printf("%s\n退出学生登录。\n%s", COLOR_YELLOW, COLOR_RESET);
                return;
            default:
                printf("%s无效选项，请重新选择。%s\n", COLOR_RED, COLOR_RESET);
                break;
        }
    }
}

// 设置学生信息
void set_student_info(studentInfo *student)
{
    printf("\n");
    print_line(COLOR_GREEN);
    printf("%s         设置/更新个人信息         %s\n", COLOR_GREEN, COLOR_RESET);
    print_line(COLOR_GREEN);
    
    if (student->stubase_.id != 0)
    {
        printf("\n当前信息:\n");
        printf("  学号: %lld\n", student->stubase_.id);
        printf("  姓名: %s\n", student->stubase_.name);
        printf("  性别: %c\n", student->stubase_.sex);
        printf("  年龄: %d\n", student->stubase_.age);
        printf("\n");
    }
    
    printf("请输入学号: ");
    scanf("%lld", &student->stubase_.id);
    clear_input_buffer();
    
    printf("请输入姓名: ");
    fgets(student->stubase_.name, MAX_NAME_LEN, stdin);
    // 移除fgets读取的换行符
    student->stubase_.name[strcspn(student->stubase_.name, "\n")] = '\0';
    
    printf("请输入性别 (M/F): ");
    scanf("%c", &student->stubase_.sex);
    clear_input_buffer();
    
    printf("请输入年龄: ");
    scanf("%d", &student->stubase_.age);
    clear_input_buffer();
    
    // 更新到文件
    if (update_student_to_file(student) == SUCCESS)
    {
        printf("%s\n个人信息设置成功！%s\n", COLOR_GREEN, COLOR_RESET);
    }
    else
    {
        printf("%s\n个人信息设置失败！%s\n", COLOR_RED, COLOR_RESET);
    }
}

// 查看个人成绩
void view_my_scores(studentInfo *student)
{
    printf("\n");
    print_line(COLOR_CYAN);
    printf("%s         我的成绩信息         %s\n", COLOR_CYAN, COLOR_RESET);
    print_line(COLOR_CYAN);
    
    if (student->stubase_.id == 0)
    {
        printf("%s您还未设置个人信息，请先设置学号和姓名。%s\n", 
               COLOR_YELLOW, COLOR_RESET);
        return;
    }
    
    printf("\n%s基本信息:%s\n", COLOR_BLUE, COLOR_RESET);
    printf("  用户名: %s\n", student->stuaccout_.user);
    printf("  学号: %lld\n", student->stubase_.id);
    printf("  姓名: %s\n", student->stubase_.name);
    printf("  性别: %c\n", student->stubase_.sex);
    printf("  年龄: %d\n", student->stubase_.age);
    
    printf("\n%s成绩信息:%s\n", COLOR_BLUE, COLOR_RESET);
    
    if (student->studscore_.Chinese == 0 && 
        student->studscore_.Maths == 0 && 
        student->studscore_.English == 0)
    {
        printf("  %s暂无成绩记录，请联系管理员添加。%s\n", 
               COLOR_YELLOW, COLOR_RESET);
    }
    else
    {
        printf("  语文: %s%d%s 分\n", COLOR_GREEN, student->studscore_.Chinese, COLOR_RESET);
        printf("  数学: %s%d%s 分\n", COLOR_GREEN, student->studscore_.Maths, COLOR_RESET);
        printf("  英语: %s%d%s 分\n", COLOR_GREEN, student->studscore_.English, COLOR_RESET);
        
        int total = student->studscore_.Chinese + 
                    student->studscore_.Maths + 
                    student->studscore_.English;
        float average = total / 3.0;
        
        printf("\n  总分: %s%d%s 分\n", COLOR_MAGENTA, total, COLOR_RESET);
        printf("  平均分: %s%.2f%s 分\n", COLOR_MAGENTA, average, COLOR_RESET);
    }
    
    print_line(COLOR_CYAN);
}

// 更新学生信息到文件
int update_student_to_file(studentInfo *student)
{
    FILE *fp = fopen(DATA_FILE, "r");
    FILE *temp_fp = fopen("data/temp.dat", "w");
    
    if (fp == NULL || temp_fp == NULL)
    {
        if (fp) fclose(fp);
        if (temp_fp) fclose(temp_fp);
        return FAILURE;
    }
    
    studentInfo temp;
    while (fread(&temp, sizeof(studentInfo), 1, fp) == 1)
    {
        if (strcmp(temp.stuaccout_.user, student->stuaccout_.user) == 0)
        {
            fwrite(student, sizeof(studentInfo), 1, temp_fp);
        }
        else
        {
            fwrite(&temp, sizeof(studentInfo), 1, temp_fp);
        }
    }
    
    fclose(fp);
    fclose(temp_fp);
    
    remove(DATA_FILE);
    rename("data/temp.dat", DATA_FILE);
    
    return SUCCESS;
}
