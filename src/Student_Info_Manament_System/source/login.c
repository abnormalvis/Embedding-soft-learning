#include "global.h"

void login_flow(void)
{
    char username[64];
    char password[64];

    printf("\n--- 登录 ---\n");
    printf("用户名：");
    if (fgets(username, sizeof(username), stdin) == NULL) return;
    username[strcspn(username, "\n")] = '\0';

    printf("密码：");
    if (fgets(password, sizeof(password), stdin) == NULL) return;
    password[strcspn(password, "\n")] = '\0';

    // TODO: 在此处进行真实的验证（读取用户数据、比对密码等）
    printf("收到登录请求：用户名='%s' 密码长度=%zu （演示，不做验证）\n",
           username, strlen(password));
}