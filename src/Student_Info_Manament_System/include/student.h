/**
 * @file student.h
 * @brief 学生信息结构体和函数声明
 */

#ifndef STUDENT_H
#define STUDENT_H

#include "config.h"

/* ============ 数据结构定义 ============ */

/**
 * @brief 学生基本信息
 */
typedef struct
{
    char id[MAX_ID_LEN];     // 学号（如"2024001"）
    char name[MAX_NAME_LEN]; // 姓名
    char gender;             // 性别（'M'男/'F'女）
    int age;                 // 年龄
} StudentBase;

/**
 * @brief 学生成绩信息
 */
typedef struct
{
    float math;       // 数学成绩
    float english;    // 英语成绩
    float c_language; // C语言成绩
} StudentScore;

/**
 * @brief 完整学生信息（组合前面两个结构体）
 */
typedef struct
{
    StudentBase base;   // 基本信息
    StudentScore score; // 成绩信息
    int isActive;       // 状态标识（1=有效，0=已删除）
} Student;

/**
 * @brief 学生管理器（管理所有学生数据）
 */
typedef struct
{
    Student students[MAX_STUDENT]; // 学生数组
    int count;                     // 当前学生数量
} StudentManager;

/* ============ 函数声明 ============ */

/**
 * @brief 初始化学生管理器
 * @param mgr 管理器指针
 */
void initStudentManager(StudentManager *mgr);

/**
 * @brief 添加学生
 * @param mgr 管理器指针
 * @param student 待添加的学生信息
 * @return SUCCESS成功，FAILURE失败
 */
int addStudent(StudentManager *mgr, const Student *student);

/**
 * @brief 根据学号查找学生
 * @param mgr 管理器指针
 * @param id 学号
 * @return 找到返回学生指针，否则返回NULL
 */
Student *findStudentById(const StudentManager *mgr, const char *id);

/**
 * @brief 显示所有学生信息
 * @param mgr 管理器指针
 */
void displayAllStudents(const StudentManager *mgr);

/**
 * @brief 删除学生（根据学号）
 * @param mgr 管理器指针
 * @param id 学号
 * @return SUCCESS成功，FAILURE失败
 */
int deleteStudent(StudentManager *mgr, const char *id);

/**
 * @brief 修改学生信息
 * @param mgr 管理器指针
 * @param id 学号
 * @return SUCCESS成功，FAILURE失败
 */
int updateStudent(StudentManager *mgr, const char *id);

/**
 * @brief 保存数据到文件
 * @param mgr 管理器指针
 * @return SUCCESS成功，FAILURE失败
 */
int saveToFile(const StudentManager *mgr);

/**
 * @brief 从文件加载数据
 * @param mgr 管理器指针
 * @return SUCCESS成功，FAILURE失败
 */
int loadFromFile(StudentManager *mgr);

#endif // STUDENT_H