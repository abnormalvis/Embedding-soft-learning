#ifndef GLOBAL_H
#define GLOBAL_H

#define MAX_NAME_LEN 10
#define STUDENT_ID_TYPE long long
// 用户角色枚举
typedef enum {
    ROLE_STUDENT = 0,  // 学生角色
    ROLE_ADMIN = 1     // 管理员角色
} UserRole;

typedef struct
{
    long long id;      // 学号（支持大数字，如3124000893）
    char name[MAX_NAME_LEN];
    char sex;
    int age;
} studentBase;

typedef struct
{
    char user[16];     /* 用户名 */
    char password[16]; /* 密码 */
    UserRole role;     /* 用户角色 */
} studentAccout;

typedef struct
{
    int Chinese;
    int Maths;
    int English;
} StudentScore;

typedef struct
{
    studentBase stubase_;
    StudentScore studscore_;
    studentAccout stuaccout_;
} studentInfo;

#endif // !GLOBAL_H
