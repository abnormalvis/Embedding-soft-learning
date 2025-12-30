#ifndef LOGIN_H
#define LOGIN_H
#include "global.h"

void login_flow(studentInfo *stuinfo, studentInfo *stutemp);
int login_judge(studentInfo *stuinfo);

#endif // LOGIN_H