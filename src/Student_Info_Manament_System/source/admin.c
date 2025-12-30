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
        printf("  5) 查看学生信息(按学号)\n");
        printf("  6) 退出登录\n");
        print_line(COLOR_CYAN);
        printf("%s请选择操作 (1-6): %s", COLOR_GREEN, COLOR_RESET);
        
        if (fgets(buf, sizeof(buf), stdin) == NULL)
            break;
            
        if (sscanf(buf, "%d", &choice) != 1)
        {
            printf("%s无效输入，请输入数字 1-6。%s\n", COLOR_RED, COLOR_RESET);
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
                view_student_by_id();
                break;
            case 6:
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
    
    // 检查学生是否已设置基本信息
    if (student.stubase_.name[0] == '\0')
    {
        printf("%s该学生还未设置基本信息，请先让学生登录并设置个人信息。%s\n", 
               COLOR_YELLOW, COLOR_RESET);
        return;
    }
    
    printf("\n当前学生信息:\n");
    printf("  学号: %lld\n", student.stubase_.id);
    printf("  姓名: %s\n", student.stubase_.name);
    printf("  用户名: %s\n", student.stuaccout_.user);
    
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
    printf("  当前成绩:\n");
    printf("    语文: %d\n", student.studscore_.Chinese);
    printf("    数学: %d\n", student.studscore_.Maths);
    printf("    英语: %d\n", student.studscore_.English);
    
    printf("\n%s开始修改成绩（输入y修改，n跳过）%s\n", COLOR_YELLOW, COLOR_RESET);
    
    char choice;
    int new_score;
    
    // 语文成绩
    printf("\n是否修改语文成绩？(y/n): ");
    scanf("%c", &choice);
    clear_input_buffer();
    if (choice == 'y' || choice == 'Y')
    {
        printf("  请输入新的语文成绩: ");
        scanf("%d", &new_score);
        clear_input_buffer();
        student.studscore_.Chinese = new_score;
        printf("%s  ✓ 语文成绩已更新为: %d%s\n", COLOR_GREEN, new_score, COLOR_RESET);
    }
    else
    {
        printf("  语文成绩保持不变: %d\n", student.studscore_.Chinese);
    }
    
    // 数学成绩
    printf("\n是否修改数学成绩？(y/n): ");
    scanf("%c", &choice);
    clear_input_buffer();
    if (choice == 'y' || choice == 'Y')
    {
        printf("  请输入新的数学成绩: ");
        scanf("%d", &new_score);
        clear_input_buffer();
        student.studscore_.Maths = new_score;
        printf("%s  ✓ 数学成绩已更新为: %d%s\n", COLOR_GREEN, new_score, COLOR_RESET);
    }
    else
    {
        printf("  数学成绩保持不变: %d\n", student.studscore_.Maths);
    }
    
    // 英语成绩
    printf("\n是否修改英语成绩？(y/n): ");
    scanf("%c", &choice);
    clear_input_buffer();
    if (choice == 'y' || choice == 'Y')
    {
        printf("  请输入新的英语成绩: ");
        scanf("%d", &new_score);
        clear_input_buffer();
        student.studscore_.English = new_score;
        printf("%s  ✓ 英语成绩已更新为: %d%s\n", COLOR_GREEN, new_score, COLOR_RESET);
    }
    else
    {
        printf("  英语成绩保持不变: %d\n", student.studscore_.English);
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

// 通过学号查看学生详细信息
void view_student_by_id(void)
{
    printf("\n");
    print_line(COLOR_MAGENTA);
    printf("%s         查看学生信息(按学号)         %s\n", COLOR_MAGENTA, COLOR_RESET);
    print_line(COLOR_MAGENTA);
    
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
    
    // 显示详细信息
    printf("\n");
    print_line(COLOR_CYAN);
    printf("%s         学生详细信息         %s\n", COLOR_CYAN, COLOR_RESET);
    print_line(COLOR_CYAN);
    
    printf("\n%s基本信息:%s\n", COLOR_YELLOW, COLOR_RESET);
    printf("  用户名: %s\n", student.stuaccout_.user);
    printf("  学号: %lld\n", student.stubase_.id);
    printf("  姓名: %s\n", student.stubase_.name[0] ? student.stubase_.name : "(未设置)");
    printf("  性别: %c\n", student.stubase_.sex ? student.stubase_.sex : '-');
    printf("  年龄: %d\n", student.stubase_.age > 0 ? student.stubase_.age : 0);
    
    printf("\n%s成绩信息:%s\n", COLOR_YELLOW, COLOR_RESET);
    printf("  语文: %d\n", student.studscore_.Chinese);
    printf("  数学: %d\n", student.studscore_.Maths);
    printf("  英语: %d\n", student.studscore_.English);
    
    // 计算总分和平均分
    int total = student.studscore_.Chinese + student.studscore_.Maths + student.studscore_.English;
    double average = total / 3.0;
    
    printf("\n%s统计信息:%s\n", COLOR_YELLOW, COLOR_RESET);
    printf("  总分: %d\n", total);
    printf("  平均分: %.2f\n", average);
    
    print_line(COLOR_CYAN);
}
