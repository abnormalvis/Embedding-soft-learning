#include <stdio.h>
#include <string.h>

#define MAX_NAME_LEN 10

// v1.0 旧数据结构（没有 role 字段）
typedef struct {
    int id;
    char name[MAX_NAME_LEN];
    char sex;
    int age;
} studentBase_v1;

typedef struct {
    char user[16];
    char password[16];
    // 注意：v1.0 没有 role 字段
} studentAccout_v1;

typedef struct {
    int Chinese;
    int Maths;
    int English;
} StudentScore_v1;

typedef struct {
    studentBase_v1 stubase_;
    StudentScore_v1 studscore_;
    studentAccout_v1 stuaccout_;
} studentInfo_v1;

// v2.0 新数据结构（有 role 字段）
typedef enum {
    ROLE_STUDENT = 0,
    ROLE_ADMIN = 1
} UserRole;

typedef struct {
    int id;
    char name[MAX_NAME_LEN];
    char sex;
    int age;
} studentBase_v2;

typedef struct {
    char user[16];
    char password[16];
    UserRole role;  // 新增字段
} studentAccout_v2;

typedef struct {
    int Chinese;
    int Maths;
    int English;
} StudentScore_v2;

typedef struct {
    studentBase_v2 stubase_;
    StudentScore_v2 studscore_;
    studentAccout_v2 stuaccout_;
} studentInfo_v2;

int main(void)
{
    printf("学生信息管理系统 - 数据迁移工具 v1.0 -> v2.0\n");
    printf("========================================\n\n");
    
    // 打开旧数据文件
    FILE *old_fp = fopen("data/students.dat.v1", "r");
    if (old_fp == NULL) {
        printf("错误：无法打开旧数据文件 data/students.dat.v1\n");
        printf("请先将旧的 students.dat 重命名为 students.dat.v1\n");
        return 1;
    }
    
    // 创建新数据文件
    FILE *new_fp = fopen("data/students.dat", "w");
    if (new_fp == NULL) {
        printf("错误：无法创建新数据文件 data/students.dat\n");
        fclose(old_fp);
        return 1;
    }
    
    studentInfo_v1 old_student;
    studentInfo_v2 new_student;
    int count = 0;
    
    // 读取旧数据并转换
    while (fread(&old_student, sizeof(studentInfo_v1), 1, old_fp) == 1) {
        // 复制基本信息
        new_student.stubase_.id = old_student.stubase_.id;
        strncpy(new_student.stubase_.name, old_student.stubase_.name, MAX_NAME_LEN);
        new_student.stubase_.sex = old_student.stubase_.sex;
        new_student.stubase_.age = old_student.stubase_.age;
        
        // 复制成绩信息
        new_student.studscore_.Chinese = old_student.studscore_.Chinese;
        new_student.studscore_.Maths = old_student.studscore_.Maths;
        new_student.studscore_.English = old_student.studscore_.English;
        
        // 复制账号信息
        strncpy(new_student.stuaccout_.user, old_student.stuaccout_.user, 16);
        strncpy(new_student.stuaccout_.password, old_student.stuaccout_.password, 16);
        
        // 设置角色为学生（默认）
        new_student.stuaccout_.role = ROLE_STUDENT;
        
        // 写入新文件
        fwrite(&new_student, sizeof(studentInfo_v2), 1, new_fp);
        
        printf("已迁移用户：%s\n", new_student.stuaccout_.user);
        count++;
    }
    
    fclose(old_fp);
    fclose(new_fp);
    
    printf("\n========================================\n");
    printf("迁移完成！共迁移 %d 个用户账号。\n", count);
    printf("旧数据文件：data/students.dat.v1（已保留）\n");
    printf("新数据文件：data/students.dat\n");
    printf("========================================\n");
    
    return 0;
}
