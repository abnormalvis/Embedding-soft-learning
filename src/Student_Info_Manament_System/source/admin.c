#include "admin.h"
#include "config.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

// 管理员菜单
void admin_menu(studentInfo *admin)
{
    char buf[32];
    int choice;
    
    while (1)
    {
        printf("\n");
        print_line(COLOR_CYAN);
        printf("%s          管理员控制台 - 欢迎 %s%s%s          \n", 
               COLOR_CYAN, COLOR_YELLOW, admin->stuaccout_.user, COLOR_CYAN);
        print_line(COLOR_CYAN);
        printf("%s", COLOR_WHITE);
        printf("  1) 添加学生成绩\n");
        printf("  2) 修改学生成绩\n");
        printf("  3) 查看所有学生\n");
        printf("  4) 删除学生信息\n");
        printf("  5) 退出登录\n");
        print_line(COLOR_CYAN);
        printf("%s请选择操作 (1-5): %s", COLOR_GREEN, COLOR_RESET);
        
        if (fgets(buf, sizeof(buf), stdin) == NULL)
            break;
            
        if (sscanf(buf, "%d", &choice) != 1)
        {
            printf("%s无效输入，请输入数字 1-5。%s\n", COLOR_RED, COLOR_RESET);
            continue;
        }
        
        switch (choice)
        {
            case 1:
                add_student_score();
                break;
            case 2:
                modify_student_score();
                break;
            case 3:
                view_all_students();
                break;
            case 4:
                delete_student_info();
                break;
            case 5:
                printf("%s\n退出管理员登录。\n%s", COLOR_YELLOW, COLOR_RESET);
                return;
            default:
                printf("%s无效选项，请重新选择。%s\n", COLOR_RED, COLOR_RESET);
                break;
        }
    }
}

// 根据用户名查找学生
int find_student_by_username(const char *username, studentInfo *result)
{
    FILE *fp = fopen(DATA_FILE, "r");
    if (fp == NULL)
        return FAILURE;
    
    studentInfo temp;
    while (fread(&temp, sizeof(studentInfo), 1, fp) == 1)
    {
        if (strcmp(temp.stuaccout_.user, username) == 0 && 
            temp.stuaccout_.role == ROLE_STUDENT)
        {
            memcpy(result, &temp, sizeof(studentInfo));
            fclose(fp);
            return SUCCESS;
        }
    }
    
    fclose(fp);
    return FAILURE;
}

// 根据学号查找学生
int find_student_by_id(long long id, studentInfo *result)
{
    FILE *fp = fopen(DATA_FILE, "r");
    if (fp == NULL)
        return FAILURE;
    
    studentInfo temp;
    while (fread(&temp, sizeof(studentInfo), 1, fp) == 1)
    {
        if (temp.stubase_.id == id && temp.stuaccout_.role == ROLE_STUDENT)
        {
            memcpy(result, &temp, sizeof(studentInfo));
            fclose(fp);
            return SUCCESS;
        }
    }
    
    fclose(fp);
    return FAILURE;
}

// 添加学生成绩
void add_student_score(void)
{
    printf("\n");
    print_line(COLOR_BLUE);
    printf("%s         添加学生成绩         %s\n", COLOR_BLUE, COLOR_RESET);
    print_line(COLOR_BLUE);
    
    char username[16];
    printf("请输入学生用户名: ");
    scanf("%15s", username);
    clear_input_buffer();
    
    studentInfo student;
    if (find_student_by_username(username, &student) == FAILURE)
    {
        printf("%s错误：找不到该学生！%s\n", COLOR_RED, COLOR_RESET);
        return;
    }
    
    // 检查学生是否已有学号
    if (student.stubase_.id == 0)
    {
        printf("%s该学生还未设置学号，请先让学生登录并设置学号。%s\n", 
               COLOR_YELLOW, COLOR_RESET);
        return;
    }
    
    printf("\n当前学生信息:\n");
    printf("  用户名: %s\n", student.stuaccout_.user);
    printf("  学号: %lld\n", student.stubase_.id);
    printf("  姓名: %s\n", student.stubase_.name);
    
    printf("\n请输入成绩:\n");
    printf("  语文: ");
    scanf("%d", &student.studscore_.Chinese);
    clear_input_buffer();
    
    printf("  数学: ");
    scanf("%d", &student.studscore_.Maths);
    clear_input_buffer();
    
    printf("  英语: ");
    scanf("%d", &student.studscore_.English);
    clear_input_buffer();
    
    // 更新文件
    FILE *fp = fopen(DATA_FILE, "r");
    FILE *temp_fp = fopen("data/temp.dat", "w");
    
    if (fp == NULL || temp_fp == NULL)
    {
        printf("%s文件操作失败！%s\n", COLOR_RED, COLOR_RESET);
        if (fp) fclose(fp);
        if (temp_fp) fclose(temp_fp);
        return;
    }
    
    studentInfo temp;
    while (fread(&temp, sizeof(studentInfo), 1, fp) == 1)
    {
        if (strcmp(temp.stuaccout_.user, username) == 0)
        {
            fwrite(&student, sizeof(studentInfo), 1, temp_fp);
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
    
    printf("%s\n成绩添加成功！%s\n", COLOR_GREEN, COLOR_RESET);
}

// 修改学生成绩
void modify_student_score(void)
{
    printf("\n");
    print_line(COLOR_BLUE);
    printf("%s         修改学生成绩         %s\n", COLOR_BLUE, COLOR_RESET);
    print_line(COLOR_BLUE);
    
    long long id;
    printf("请输入学生学号: ");
    scanf("%lld", &id);
    clear_input_buffer();
    
    studentInfo student;
    if (find_student_by_id(id, &student) == FAILURE)
    {
        printf("%s错误：找不到该学号的学生！%s\n", COLOR_RED, COLOR_RESET);
        return;
    }
    
    printf("\n当前学生信息:\n");
    printf("  学号: %lld\n", student.stubase_.id);
    printf("  姓名: %s\n", student.stubase_.name);
    printf("  语文: %d\n", student.studscore_.Chinese);
    printf("  数学: %d\n", student.studscore_.Maths);
    printf("  英语: %d\n", student.studscore_.English);
    
    printf("\n请输入新的成绩:\n");
    printf("  语文: ");
    scanf("%d", &student.studscore_.Chinese);
    clear_input_buffer();
    
    printf("  数学: ");
    scanf("%d", &student.studscore_.Maths);
    clear_input_buffer();
    
    printf("  英语: ");
    scanf("%d", &student.studscore_.English);
    clear_input_buffer();
    
    // 更新文件
    FILE *fp = fopen(DATA_FILE, "r");
    FILE *temp_fp = fopen("data/temp.dat", "w");
    
    if (fp == NULL || temp_fp == NULL)
    {
        printf("%s文件操作失败！%s\n", COLOR_RED, COLOR_RESET);
        if (fp) fclose(fp);
        if (temp_fp) fclose(temp_fp);
        return;
    }
    
    studentInfo temp;
    while (fread(&temp, sizeof(studentInfo), 1, fp) == 1)
    {
        if (temp.stubase_.id == id && temp.stuaccout_.role == ROLE_STUDENT)
        {
            fwrite(&student, sizeof(studentInfo), 1, temp_fp);
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
    
    printf("%s\n成绩修改成功！%s\n", COLOR_GREEN, COLOR_RESET);
}

// 查看所有学生信息
void view_all_students(void)
{
    printf("\n");
    print_line(COLOR_MAGENTA);
    printf("%s         所有学生信息列表         %s\n", COLOR_MAGENTA, COLOR_RESET);
    print_line(COLOR_MAGENTA);
    
    FILE *fp = fopen(DATA_FILE, "r");
    if (fp == NULL)
    {
        printf("%s暂无学生数据。%s\n", COLOR_YELLOW, COLOR_RESET);
        return;
    }
    
    printf("\n%-12s %-12s %-10s %-6s %-6s %-6s\n", 
           "学号", "用户名", "姓名", "语文", "数学", "英语");
    print_line(COLOR_CYAN);
    
    studentInfo temp;
    int count = 0;
    
    while (fread(&temp, sizeof(studentInfo), 1, fp) == 1)
    {
        if (temp.stuaccout_.role == ROLE_STUDENT)
        {
            printf("%-12lld %-12s %-10s %-6d %-6d %-6d\n",
                   temp.stubase_.id,
                   temp.stuaccout_.user,
                   temp.stubase_.name[0] ? temp.stubase_.name : "(未设置)",
                   temp.studscore_.Chinese,
                   temp.studscore_.Maths,
                   temp.studscore_.English);
            count++;
        }
    }
    
    fclose(fp);
    
    if (count == 0)
    {
        printf("%s暂无学生数据。%s\n", COLOR_YELLOW, COLOR_RESET);
    }
    else
    {
        print_line(COLOR_CYAN);
        printf("%s共有 %d 名学生%s\n", COLOR_GREEN, count, COLOR_RESET);
    }
}

// 删除学生信息
void delete_student_info(void)
{
    printf("\n");
    print_line(COLOR_RED);
    printf("%s         删除学生信息         %s\n", COLOR_RED, COLOR_RESET);
    print_line(COLOR_RED);
    
    char username[16];
    printf("请输入要删除的学生用户名: ");
    scanf("%15s", username);
    clear_input_buffer();
    
    studentInfo student;
    if (find_student_by_username(username, &student) == FAILURE)
    {
        printf("%s错误：找不到该学生！%s\n", COLOR_RED, COLOR_RESET);
        return;
    }
    
    printf("\n将删除以下学生信息:\n");
    printf("  用户名: %s\n", student.stuaccout_.user);
    printf("  学号: %lld\n", student.stubase_.id);
    printf("  姓名: %s\n", student.stubase_.name);
    
    char confirm;
    printf("\n%s确认删除吗？(y/n): %s", COLOR_YELLOW, COLOR_RESET);
    scanf("%c", &confirm);
    clear_input_buffer();
    
    if (confirm != 'y' && confirm != 'Y')
    {
        printf("已取消删除。\n");
        return;
    }
    
    // 更新文件
    FILE *fp = fopen(DATA_FILE, "r");
    FILE *temp_fp = fopen("data/temp.dat", "w");
    
    if (fp == NULL || temp_fp == NULL)
    {
        printf("%s文件操作失败！%s\n", COLOR_RED, COLOR_RESET);
        if (fp) fclose(fp);
        if (temp_fp) fclose(temp_fp);
        return;
    }
    
    studentInfo temp;
    while (fread(&temp, sizeof(studentInfo), 1, fp) == 1)
    {
        if (strcmp(temp.stuaccout_.user, username) != 0)
        {
            fwrite(&temp, sizeof(studentInfo), 1, temp_fp);
        }
    }
    
    fclose(fp);
    fclose(temp_fp);
    
    remove(DATA_FILE);
    rename("data/temp.dat", DATA_FILE);
    
    printf("%s\n学生信息已删除！%s\n", COLOR_GREEN, COLOR_RESET);
}
