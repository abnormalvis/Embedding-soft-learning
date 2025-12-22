# UI 初始化与交互验证记录

## 概要
本次工作目标：实现并验证学生信息管理系统的登录/注册交互界面初始化（命令行菜单），并确保能编译运行以便后续开发。

## 已完成的修改（文件）
- 新增：`source/main_test.c` — 测试入口，调用 UI 菜单。
- 新增：`source/stubs.c` — 提供 `login_flow` / `register_flow` 存根以便快速链接与测试。
- 新增：`include/ui_display_clean.h` — 干净的 UI 头文件（绕开仓库中存在的格式问题）。
- 新增/修改：`Makefile` — 简易构建脚本，构建测试二进制 `build/student_ui_test`。
- 修改：`source/main.c` — 规范化为合法 `main`（保留原实现的调用形式）。
- 修改：`source/ui_display.c` — 清理并修复为正确可编译的 `ui_display` 实现（菜单循环、选择分发）。
- 修改：`include/login.h`、`include/register.h` — 补充函数声明以便编译。

注意：仓库中原有的某些头文件存在格式异常（含 Markdown 代码块或错误的 guard），为避免破坏已有文件，我新增了 `ui_display_clean.h` 并让测试程序与 UI 实现使用它；其余改动尽量保持最小化。

## 构建与运行（复现）
在项目路径下执行：

```bash
cd src/Student_Info_Manament_System
make
./build/student_ui_test
```

运行后会展示菜单：

- 1) 登 录
- 2) 注 册
- 3) 退 出

选择 `1`/`2` 会调用对应的存根函数，输出 `(stub) login_flow called` / `(stub) register_flow called`。选择 `3` 退出程序。

## 验证结果
- 程序成功编译为 `build/student_ui_test`。
- 菜单显示与交互正常，能读取输入并分发到登录/注册处理逻辑（当前为存根）。

## 建议的下一步
1. 清理并修复仓库中格式异常的头文件（例如 `include/global.h`、原 `include/ui_display.h`），将 `ui_display_clean.h` 的内容合并回正式头文件并移除临时替代品。 
2. 用真实实现替换 `stubs.c`：完善 `login_flow` / `register_flow`，实现用户数据的持久化（例如 `users.csv`）和基本校验。
3. 将其他模块（学生管理相关源文件）逐步纳入 Makefile 构建，恢复完整编译链。
4. 添加单元测试（或脚本化输入）以自动验证菜单与基础行为。

如需我现在开始执行第 1 或第 2 步，请告诉我你偏好：先修头文件还是先实现登录/注册持久化？

## 设计细节
### 登陆界面
```
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
```
- 使用到了`fgets()`函数，该函数会读取一行数据，并返回读取的字节数。
- 使用到了  `strcspn()`函数，该函数会返回字符串中第一个匹配的字符的位置。