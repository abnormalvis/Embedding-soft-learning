# MySQL 学生管理系统练习指引

这份文档配合同目录下示例一起使用：

- server.c: 主程序骨架（含伪代码 TODO）
- demo_01_connect_template.c: 连接练习
- demo_02_schema_seed_template.c: 建表与初始化练习
- demo_03_crud_menu_template.c: CRUD 菜单练习

## 1. 先跑通编译

在当前目录执行：

```bash
make
```

如果你不想全部编译：

```bash
make server
```

## 2. 建议练习顺序

1. demo_01_connect_template.c
2. demo_02_schema_seed_template.c
3. demo_03_crud_menu_template.c
4. server.c

## 3. 每个阶段要达到的结果

1. 连接成功
- 能打印 connect success
- 故意写错密码时能打印 mysql_error

2. 建表和插入
- students 表创建成功
- 插入至少 3 条样例记录

3. CRUD 菜单
- 能显示所有学生
- 能根据 name 更新 city
- 能根据 name 删除记录

4. 综合主程序
- 菜单 1~5 都可用
- 所有查询都能正确释放结果集

## 4. 推荐扩展练习

1. 把字符串拼接 SQL 改成预处理语句（mysql_stmt_*）
2. 增加登录鉴权（管理员用户名密码）
3. 增加分页查询（LIMIT/OFFSET）
4. 增加输入校验（空字符串、年龄范围、超长输入）
5. 增加日志文件（记录每次 SQL 执行结果）

## 5. 常见错误提示

1. Access denied
- 用户名/密码错误，或用户无权限

2. Unknown database
- 数据库不存在，先在 MySQL 中创建，或修改连接参数

3. Table doesn't exist
- 先执行建表逻辑，再执行查询逻辑

4. Commands out of sync
- 上一次查询结果未释放，检查 mysql_free_result
