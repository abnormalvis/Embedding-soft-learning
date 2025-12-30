#ifndef ADMIN_H
#define ADMIN_H

#include "global.h"

/* 管理员功能函数声明 */

// 管理员菜单
void admin_menu(studentInfo *admin);

// 添加学生成绩
void add_student_score(void);

// 修改学生成绩
void modify_student_score(void);

// 查看所有学生信息
void view_all_students(void);

// 删除学生信息
void delete_student_info(void);

// 根据用户名查找学生
int find_student_by_username(const char *username, studentInfo *result);

// 根据学号查找学生
int find_student_by_id(int id, studentInfo *result);

#endif // ADMIN_H
