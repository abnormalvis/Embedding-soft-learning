#ifndef UI_DISPLAY_H
#define UI_DISPLAY_H
#include <stdio.h>
void UI_Display(void);                    /* 主UI显示 */
void Register_Display(void);              /* 账号注册显示 */
void Login_Display(void);                 /* 登录显示 */
void Register_Success_Display(void);      /* 注册成功显示 */
void Login_Input(void);                   /* 登录输入显示 */
void Root_Menu_Display(void);             /* 根菜单显示 */
void Root_Register_Request_Display(void); /* 根注册请求显示 */
void Login_Failed_Display(void);          /* 登录失败显示 */
void Root_Main_Menu_Display(void);        /* 根主菜单显示 */
void User_Login_Success_Display(void);    /* 用户登录成功显示 */
void User_Main_Menu_Display(void);        /* 用户主菜单显示 */
#endif                                    // !UI_DISPLAY_H