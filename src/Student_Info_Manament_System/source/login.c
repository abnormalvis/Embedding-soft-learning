#include "global.h"
/*
登陆需求分析：
- 用户输入用户名和密码
- 读取文件，验证用户名和密码是否匹配
- 成功则进入系统，失败则提示错误并返回主界面
*/
/*
FILE *fopen(const char *pathname, const char *mode);
FILE *fdopen(int fd, const char *mode);
FILE *freopen(const char *pathname, const char *mode, FILE *stream);
*/

/*
FILE *fp = NULL;
fp = fopen("/tmp/test.txt", "w+");
fprintf(fp, "This is testing for fprintf...\n");
fputs("This is testing for fputs...\n", fp);
fclose(fp);
*/
void login_flow(void)
{
    FILE *fp = NULL;    // 初始化文件操作指针
    fp = fopen("Embedding-soft-learning/src/Student_Info_Manament_System/USER_FILE.txt", "r+");
    // fgets(buff, 255, (FILE*)fp);
    // printf("3: %s\n", buff );
    // fclose(fp);
    char username[64];  // 用户名
    char password[64];  // 密码

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