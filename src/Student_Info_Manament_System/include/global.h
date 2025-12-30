#ifndef GLOBAL_H
#define GLOBAL_H

#define MAX_NAME_LEN 10
typedef struct
{
    int id;
    char name[MAX_NAME_LEN];
    char sex;
    int age;
} studentBase;

typedef struct
{
    char user[16];     /* 用户名 */
    char password[16]; /* 密码 */
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
