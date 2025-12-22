#include "global.h"
void register_flow(void)
{
    char username[64];
    char password[64];
    char password2[64];

    printf("\n--- 注册 ---\n");
    printf("请输入用户名：");
    if (fgets(username, sizeof(username), stdin) == NULL) return;
    username[strcspn(username, "\n")] = '\0';

    printf("请输入密码：");
    if (fgets(password, sizeof(password), stdin) == NULL) return;
    password[strcspn(password, "\n")] = '\0';

    printf("请再次输入密码：");
    if (fgets(password2, sizeof(password2), stdin) == NULL) return;
    password2[strcspn(password2, "\n")] = '\0';

    if (strcmp(password, password2) != 0) {
        printf("两次输入的密码不一致，注册失败。\n");
        return;
    }

    // TODO: 在此处保存用户信息（写入文件或数据库），并处理已存在用户的情况
    printf("已收到注册请求：用户名='%s' 密码长度=%zu （演示，不写入）\n",
           username, strlen(password));
}