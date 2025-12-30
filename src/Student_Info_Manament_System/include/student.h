#ifndef STUDENT_H
#define STUDENT_H

#include "global.h"

/* 学生功能函数声明 */

// 学生菜单
void student_menu(studentInfo *student);

// 设置学生信息（学号、姓名等）
void set_student_info(studentInfo *student);

// 查看个人成绩
void view_my_scores(studentInfo *student);

// 更新学生信息到文件
int update_student_to_file(studentInfo *student);

#endif // STUDENT_H
