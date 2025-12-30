/*
* @file config.h
* @brief 项目配置常量定义
*/

#ifndef CONFIG_H
#define CONFIG_H

/* 容量定义 */
#define MAX_STUDENT 100     // 最大学生数量
#define MAX_NAME_LEN 10    // 最大姓名长度
#define MAX_ID_LEN 10      // 最大学号长度

/* 文件路径 */
#define DATA_FILE "data/students.dat"   // 数据文件路径

/* 管理员配置 */
#define ADMIN_USERNAME "admin"       // 默认管理员用户名
#define ADMIN_PASSWORD "admin123"    // 默认管理员密码

#define SUCCESS 1
#define FAILURE 0

#define GENDER_MALE 'M'
#define GENDER_FEMALE 'F'

/* ANSI颜色代码 - 用于UI美化 */
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[1;31m"
#define COLOR_GREEN   "\033[1;32m"
#define COLOR_YELLOW  "\033[1;33m"
#define COLOR_BLUE    "\033[1;34m"
#define COLOR_MAGENTA "\033[1;35m"
#define COLOR_CYAN    "\033[1;36m"
#define COLOR_WHITE   "\033[1;37m"

// 用户操作枚举体
enum
{
    user_login = 1,
    user_register,
    exit_system
};
#endif // !CONFIG_H