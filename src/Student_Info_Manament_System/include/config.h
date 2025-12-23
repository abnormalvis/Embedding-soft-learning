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

#define SUCCESS 1
#define FAILURE 0

#define GENDER_MALE 'M'
#define GENDER_FEMALE 'F'
#endif // !CONFIG_H