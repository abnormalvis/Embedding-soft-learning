# MySQL Demo 修复完成文档

## 问题总结

原始问题：
1. **编译错误**：C语言多行字符串语法错误
2. **运行时错误**：MySQL Access Denied

## 已完成的修复 ✅

### 1. C语法错误修复（已完成）

**问题**：第45-48行字符串拼接错误
```c
// 错误的写法（跨行未正确拼接）
const char *insert_sql = "INSERT INTO students (name, phone_number, city,age)
    VALUES('amy', '1234567890', 'Beijing', 20),
```

**修复**：使用C语言标准的字符串字面量自动拼接
```c
// 正确的写法
const char *insert_sql = 
    "INSERT INTO students (name, phone_number, city, age) "
    "VALUES('amy', '1234567890', 'Beijing', 20), "
    "      ('bobo', '9876543210', 'Shanghai', 22), "
    "      ('cici', '5555555555', 'Guangzhou', 21)";
```

### 2. 连接代码更新（已完成）

**修改**：从 socket 认证改为密码认证
```c
// 第26-27行
// 原代码：
mysql_real_connect(conn, "localhost", "root", NULL, "student_info", 0,
                   "/var/run/mysqld/mysqld.sock", 0)

// 新代码：
mysql_real_connect(conn, "localhost", "root", "123456", "student_info", 0,
                   NULL, 0)
```

### 3. 编译测试（已完成）

```bash
cd src/MySQL_server
make demo_02_schema_seed_template
# ✅ 编译成功，无错误
```

## 需要手动完成的步骤 ⚠️

由于 MySQL 配置需要 sudo 权限，无法自动完成。请按以下步骤操作：

### 方案 A：使用配置脚本（推荐）

```bash
cd ~/As-my-see/Embedding-soft-learning/src/MySQL_server
./setup_mysql.sh
```

输入 sudo 密码后，脚本将自动完成以下操作：
1. 设置 root 密码为 123456
2. 创建 student_info 数据库
3. 验证连接

### 方案 B：手动执行命令

```bash
# 1. 设置 root 密码（需要 sudo）
sudo mysql -e "ALTER USER 'root'@'localhost' IDENTIFIED WITH mysql_native_password BY '123456';"
sudo mysql -e "FLUSH PRIVILEGES;"

# 2. 创建数据库
mysql -u root -p123456 -e "CREATE DATABASE IF NOT EXISTS student_info;"

# 3. 验证连接
mysql -u root -p123456 -e "SELECT 'MySQL配置成功！' AS status;"
```

### 方案 C：创建专用学习用户（更安全）

```bash
# 1. 创建学习用户
sudo mysql -e "CREATE USER IF NOT EXISTS 'student_admin'@'localhost' IDENTIFIED BY '123456';"
sudo mysql -e "GRANT ALL PRIVILEGES ON student_info.* TO 'student_admin'@'localhost';"
sudo mysql -e "CREATE DATABASE IF NOT EXISTS student_info;"
sudo mysql -e "FLUSH PRIVILEGES;"

# 2. 修改代码使用新用户
# 将 demo_02_schema_seed_template.c 第26行的 "root" 改为 "student_admin"
```

## 完成后的测试步骤

### 1. 运行程序

```bash
cd ~/As-my-see/Embedding-soft-learning/src/MySQL_server
./demo_02_schema_seed_template
```

**预期输出**：
```
[demo_02] create table success
```

### 2. 验证数据插入

```bash
mysql -u root -p123456 student_info -e "SELECT * FROM students;"
```

**预期输出**：
```
+----+------+--------------+-----------+-----+
| id | name | phone_number | city      | age |
+----+------+--------------+-----------+-----+
|  1 | amy  | 1234567890   | Beijing   |  20 |
|  2 | bobo | 9876543210   | Shanghai  |  22 |
|  3 | cici | 5555555555   | Guangzhou |  21 |
+----+------+--------------+-----------+-----+
```

## 其他 Demo 文件的修改建议

为保持一致性，建议同步更新以下文件使用相同的认证方式：

1. `demo_01_connect_template.c`（第28行）
2. `demo_03_crud_menu_template.c`（需要检查连接代码）
3. `server.c`（需要检查连接代码）

需要我帮你修改这些文件吗？

## 故障排除

### 错误：Access denied
- **原因**：MySQL密码未设置或不正确
- **解决**：运行上述配置命令

### 错误：Unknown database 'student_info'
- **原因**：数据库未创建
- **解决**：`mysql -u root -p123456 -e "CREATE DATABASE student_info;"`

### 错误：Table 'students' already exists
- **原因**：表已存在（这是正常的，程序会跳过）
- **解决**：继续运行即可，或使用 `DROP TABLE students;` 重新开始

### 警告：Using a password on the command line
- **原因**：命令行明文密码（仅开发环境）
- **解决**：生产环境应使用配置文件 `~/.my.cnf`

## 代码改进的学习要点

1. **C语言字符串字面量拼接**：相邻的字符串字面量会自动拼接
   ```c
   "Hello " "World"  // 等同于 "Hello World"
   ```

2. **MySQL C API 连接参数**：
   - `host`: "localhost" 或 NULL
   - `user`: 用户名
   - `password`: 密码或 NULL（socket认证）
   - `db`: 数据库名
   - `port`: TCP端口，0表示默认
   - `unix_socket`: socket文件路径，NULL表示使用TCP
   - `client_flag`: 客户端标志

3. **错误处理**：始终检查返回值并使用 `mysql_error()` 获取错误信息

## 文件清单

修改的文件：
- ✅ `src/MySQL_server/demo_02_schema_seed_template.c`

新增的文件：
- ✅ `src/MySQL_server/setup_mysql.sh`（MySQL配置脚本）
- ✅ `src/MySQL_server/FIXES_COMPLETED.md`（本文档）
