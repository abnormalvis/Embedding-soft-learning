---
title: QSqlite 数据库开发指南
description: Qt5 中使用 QSqlite 操作 SQLite 数据库的完整指南
tags: [Qt, SQLite, Database, QSqlite]
created: 2026-03-29
updated: 2026-03-29
source: 课程笔记整理
related: [[Qt5_framework_development]]
---

# QSqlite 数据库开发指南

> [!NOTE] 概述
> QSqlite 是 Qt 提供的 SQLite 数据库操作模块，属于 Qt SQL 模块的一部分。本文详细介绍 QSqlite 的使用方法。

---

## 1. 快速入门

### 1.1 引入方式

> [!IMPORTANT] 注意事项
> QSqlite 是 Qt 的外挂插件，**不是 Qt 的内置模块**，需要在 `.pro` 文件中显式引入。

#### 在 `.pro` 文件中添加

```qmake
QT += core gui sql
```

#### 引入头文件

```cpp
#include <QSql>
```

### 1.2 核心类介绍

| 类名 | 功能 |
|------|------|
| **QSqlDatabase** | 管理数据库连接操作 |
| **QSqlQuery** | 执行 SQL 语句 |
| **QSqlRecord** | 封装数据库记录 |

---

## 2. 数据库操作流程

```
┌────────────┐    ┌────────────┐    ┌────────────┐    ┌────────────┐
│ 创建连接   │ → │ 打开数据库  │ → │ 操作数据库  │ → │ 关闭连接   │
└────────────┘    └────────────┘    └────────────┘    └────────────┘
```

---

## 3. 创建/删除数据库连接

### 3.1 核心方法

| 方法 | 说明 |
|------|------|
| `contains(连接名称)` | 判断连接列表中是否存在指定连接 |
| `database(连接名称, 打开状态)` | 根据连接名称返回连接对象 |
| `addDatabase(驱动类型, 连接名称)` | 新建连接对象并添加到连接列表 |
| `removeDatabase(连接名称)` | 删除连接对象 |

### 3.2 使用默认连接名称

> [!INFO] 说明
> 当只使用一个数据库文件时，使用平台默认的"连接名称"即可。

```cpp
// 创建连接对象
QSqlDatabase db;

// 关联数据库文件给连接对象
if (QSqlDatabase::contains())
{
    qDebug() << "exist dbConnection[main]";
    // 如果数据库文件已存在连接列表，则返回给连接对象
    db = QSqlDatabase::database();
}
else
{
    qDebug() << "No exist dbConnection[main]";
    // 如果没有存在连接列表，则新建一个连接对象
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("CashSystem.db");
}

// ... 使用数据库 ...

// 删除连接
db.removeDatabase();
```

### 3.3 自定义连接名称

> [!TIP] 应用场景
> 当需要同时连接多个数据库文件时，必须使用不同的"连接名称"来区分。

```cpp
// 创建连接对象
QSqlDatabase db;

// 使用自定义连接名称
if (QSqlDatabase::contains("dbConnection"))
{
    qDebug() << "exist dbConnection[main]";
    db = QSqlDatabase::database("dbConnection");
}
else
{
    qDebug() << "No exist dbConnection[main]";
    db = QSqlDatabase::addDatabase("QSQLITE", "dbConnection");
    db.setDatabaseName("CashSystem.db");
}

// ... 使用数据库 ...

// 删除连接
QSqlDatabase::removeDatabase("dbConnection");
```

> [!CAUTION] 重要提示
> 使用自定义连接名称时，`QSqlQuery` 构造必须传入连接对象：
> ```cpp
> QSqlQuery query(db);  // 正确写法
> // 如果不传，运行时可能报错：
> // "QSqlQuery::prepare: database not open"
> // "Driver not loaded"
> ```

> [!WARNING] removeDatabase 注意事项
> 直接在 `db` 变量作用域内调用 `db.removeDatabase()` 会导致错误，因为 `db` 本身还在占用引用。正确做法：
> - **方案 1**：确保 `db` 变量先销毁（超出作用域）
> - **方案 2**：使用静态方法 `QSqlDatabase::removeDatabase("连接名称")`

---

## 4. 打开/关闭数据库

### 4.1 打开数据库

```cpp
if (db.open())
{
    // 连接成功，执行数据库操作
}
else
{
    qDebug() << "Database open failed";
}
```

> [!INFO] open() 工作原理
> 根据连接对象确定"数据库名称"，判断数据库文件是否存在：
> - 若存在 → 进行物理连接
> - 若不存在 → 创建并物理连接

### 4.2 关闭数据库

```cpp
db.close();
```

---

## 5. 操作数据库

### 5.1 QSqlQuery 类

> [!NOTE] 说明
> 打开数据库成功后，需要实例化 `QSqlQuery` 对象来完成各种 SQL 操作。

### 5.2 核心方法

| 方法 | 说明 |
|------|------|
| `prepare(SQL字符串)` | 加载 SQL 语句，为操作做准备 |
| `exec()` | 执行单条 SQL 语句 |
| `execBatch()` | 批量执行 SQL 语句 |
| `bindValue(占位符, 值)` | 将占位符设置为绑定值（命名绑定） |
| `addBindValue(值)` | 添加值到绑定列表（位置绑定） |

---

## 6. SQL 操作示例

### 6.1 执行无参数 SQL 语句

```cpp
// 创建表的 SQL 语句
QString create_sql_member = "create table member(id int primary key, name varchar(30), password varchar(30))";

// 打开数据库连接
if (db.open())
{
    QSqlQuery sql_query;  // 使用默认连接

    // 准备 SQL 语句
    sql_query.prepare(create_sql_member);

    // 执行 SQL 语句
    if (!sql_query.exec())
    {
        qDebug() << "Table Create failed";
        qDebug() << sql_query.lastError();
    }
    else
    {
        qDebug() << "Table Created";
    }

    // 关闭数据库连接
    db.close();
}
```

### 6.2 插入单条记录（命名占位符）

```cpp
// 插入数据（使用命名占位符）
QString insert_sql = "insert into member(id, name, password) values(:id, :name, :password)";

sql_query.prepare(insert_sql);
sql_query.bindValue(":id", ID.toInt());
sql_query.bindValue(":name", userName);
sql_query.bindValue(":password", password);

if (!sql_query.exec())
{
    qDebug() << sql_query.lastError();
}
else
{
    qDebug() << "Insert record success";
}
```

### 6.3 批量插入记录（位置占位符）

```cpp
// 插入数据（使用位置占位符 ?）
QString insert_sql = "insert into member values(?,?,?)";

QVariantList groupIds;
groupIds.append(1001);
groupIds.append(1002);

QVariantList groupNames;
groupNames.append("Alice");
groupNames.append("Bob");

QVariantList groupPasswords;
groupPasswords.append("password1");
groupPasswords.append("password2");

sql_query.addBindValue(groupIds);
sql_query.addBindValue(groupNames);
sql_query.addBindValue(groupPasswords);

if (!sql_query.execBatch())
{
    qDebug() << sql_query.lastError();
}
else
{
    qDebug() << "Batch insert success";
}
```

### 6.4 查询记录

```cpp
QString select_all_sql = "select * from member";

sql_query.prepare(select_all_sql);

if (sql_query.exec())
{
    while (sql_query.next())
    {
        int id = sql_query.value(0).toInt();
        QString name = sql_query.value(1).toString();
        QString password = sql_query.value(2).toString();

        qDebug() << QString("ID:%1  Name:%2  Password:%3")
                        .arg(id).arg(name).arg(password);
    }
}
```

### 6.5 删除记录

```cpp
QString delete_sql = "delete from member where id = ?";
sql_query.prepare(delete_sql);
sql_query.addBindValue(ID);

if (!sql_query.exec())
{
    qDebug() << sql_query.lastError();
}
else
{
    qDebug() << "Deleted!";
}
```

---

## 7. 综合实例：用户登陆权限管理系统

### 7.1 功能要求

```
┌─────────────────────────────────────────────────────────────┐
│                     用户注册/审核/登陆系统                      │
├─────────────────────────────────────────────────────────────┤
│  1. 用户注册（插入待审表）                                    │
│  2. 管理员审核（从待审表转移到用户表）                          │
│  3. 用户登陆（从用户表验证）                                  │
└─────────────────────────────────────────────────────────────┘
```

### 7.2 数据库设计

| 表名 | 字段 | 说明 |
|------|------|------|
| `member` | id, name, password | 待审用户表 |
| `Bmember` | id, name, password | 已审核用户表 |

### 7.3 工程配置

```qmake
# uManage.pro
QT += core gui sql
```

### 7.4 main.cpp 中创建数据库和表

```cpp
#include <QApplication>
#include <QMessageBox>
#include <QSql>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 连接 SQLite 数据库
    QSqlDatabase database = QSqlDatabase::addDatabase("QSQLITE");
    database.setDatabaseName("CashSystem.db");

    if (database.open())
    {
        QSqlQuery sql_query;

        // 创建待审信息表
        QString create_sql_member = "create table member(id int primary key, name varchar(30), password varchar(30))";
        sql_query.prepare(create_sql_member);
        if (!sql_query.exec())
        {
            qDebug() << "Table Create failed:" << sql_query.lastError();
        }
        else
        {
            qDebug() << "Table Created";
        }

        // 创建用户信息表
        QString create_sql_Bmember = "create table Bmember(id int primary key, name varchar(30), password varchar(30))";
        sql_query.prepare(create_sql_Bmember);
        if (!sql_query.exec())
        {
            qDebug() << "User Table Create failed:" << sql_query.lastError();
        }
        else
        {
            qDebug() << "User infomations Table Created";
        }

        MainWindow w;
        w.show();
        database.close();
    }
    else
    {
        QMessageBox::information(nullptr, "Warning", "Connect sqlite fail", QMessageBox::Yes);
        return 0;
    }

    return a.exec();
}
```

### 7.5 注册功能实现

```cpp
void regist::on_submitbt_clicked()
{
    // 获取控件数据
    QString ID = ui->register_id_le->text();
    QString USER = ui->register_name_le->text();
    QString CODE = ui->register_code_le->text();

    // 输入验证
    if (ID.isEmpty() || USER.isEmpty() || CODE.isEmpty())
    {
        QMessageBox::warning(this, "Warning", "number or user or code is empty, enter again!");
        return;
    }

    // 打开数据库
    QSqlDatabase database = QSqlDatabase::addDatabase("QSQLITE");
    database.setDatabaseName("CashSystem.db");

    if (database.open())
    {
        QSqlQuery sql_query;

        // 插入数据
        QString insert_sql = "insert into member values(?,?,?)";
        sql_query.prepare(insert_sql);

        QVariantList groupIds;
        groupIds.append(ID.toInt());

        QVariantList groupNames;
        groupNames.append(USER);

        QVariantList groupPasswords;
        groupPasswords.append(CODE);

        sql_query.addBindValue(groupIds);
        sql_query.addBindValue(groupNames);
        sql_query.addBindValue(groupPasswords);

        if (!sql_query.execBatch())
        {
            qDebug() << sql_query.lastError();
        }
        else
        {
            qDebug() << "Insert record success";
        }

        // 查询待审表中所有待审记录
        QString select_all_sql = "select * from member";
        sql_query.prepare(select_all_sql);

        if (sql_query.exec())
        {
            while (sql_query.next())
            {
                int id = sql_query.value(0).toInt();
                QString name = sql_query.value(1).toString();
                QString password = sql_query.value(2).toString();

                qDebug() << QString("ID:%1  Name:%2  Password:%3")
                                .arg(id).arg(name).arg(password);
            }
        }

        // 提示提交完毕
        QMessageBox::information(this, "Info", "Update finished!");

        // 清除控件中内容
        ui->register_id_le->clear();
        ui->register_name_le->clear();
        ui->register_code_le->clear();

        database.close();
    }
}
```

### 7.6 管理员审核功能实现

#### 构造函数中显示待审表数据

```cpp
// manage.cpp 构造函数
Manage::Manage(QWidget *parent) : QMainWindow(parent), ui(new Ui::Manage)
{
    ui->setupUi(this);

    // 链接数据库
    QSqlDatabase database = QSqlDatabase::addDatabase("QSQLITE");
    database.setDatabaseName("CashSystem.db");
    database.open();

    QSqlQuery sql_query;
    QString select_all_sql = "select * from member";
    sql_query.prepare(select_all_sql);

    // 统计记录数
    int number = 0;
    if (sql_query.exec())
    {
        while (sql_query.next())
        {
            number++;
        }
    }

    // 设置表格数
    ui->register_info_tw->setRowCount(number);
    ui->register_info_tw->setColumnCount(3);

    // 设置列标题
    QStringList colTitles;
    colTitles << "Number" << "Name" << "Balance";
    ui->register_info_tw->setHorizontalHeaderLabels(colTitles);

    // 设置列宽
    ui->register_info_tw->setColumnWidth(0, 160);
    ui->register_info_tw->setColumnWidth(1, 160);

    // 设置表格样式
    ui->register_info_tw->setSelectionBehavior(QAbstractItemView::SelectItems);
    ui->register_info_tw->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->register_info_tw->setStyleSheet("selection-background-color:pink");
    ui->register_info_tw->setFocusPolicy(Qt::NoFocus);

    // 填充数据
    if (sql_query.exec())
    {
        for (int row = 0; row < number; ++row)
        {
            sql_query.next();

            QTableWidgetItem *item0 = new QTableWidgetItem;
            item0->setText(sql_query.value(0).toString());
            item0->setTextAlignment(Qt::AlignCenter);
            ui->register_info_tw->setItem(row, 0, item0);

            QTableWidgetItem *item1 = new QTableWidgetItem;
            item1->setText(sql_query.value(1).toString());
            item1->setTextAlignment(Qt::AlignCenter);
            ui->register_info_tw->setItem(row, 1, item1);

            QTableWidgetItem *item2 = new QTableWidgetItem;
            item2->setText(sql_query.value(2).toString());
            item2->setTextAlignment(Qt::AlignCenter);
            ui->register_info_tw->setItem(row, 2, item2);
        }
    }

    database.close();
}
```

#### 审核通过功能

```cpp
void manage::on_allowbtn_clicked()
{
    int ID = ui->register_num_le->text().toInt();

    QSqlDatabase database = QSqlDatabase::addDatabase("QSQLITE");
    database.setDatabaseName("CashSystem.db");

    if (database.open())
    {
        QSqlQuery sql_query;

        // 检查是否已在用户表中
        QString select_all_sql = "select * from Bmember";
        sql_query.prepare(select_all_sql);

        if (sql_query.exec())
        {
            while (sql_query.next())
            {
                int id = sql_query.value(0).toInt();
                if (id == ID)
                {
                    database.close();
                    QMessageBox::warning(this, "Warning", "The id is in Bmember, enter again!");
                    return;
                }
            }
        }

        // 从待审表获取用户信息
        select_all_sql = "select * from member";
        sql_query.prepare(select_all_sql);

        if (sql_query.exec())
        {
            while (sql_query.next())
            {
                int id = sql_query.value(0).toInt();
                if (id == ID)
                {
                    QString name = sql_query.value(1).toString();
                    QString password = sql_query.value(2).toString();

                    qDebug() << QString("ID:%1  Name:%2  Password:%3")
                                    .arg(id).arg(name).arg(password);
                }
            }
        }

        // 插入到用户表
        QString insert_sql = "insert into Bmember values(?,?,?)";
        sql_query.prepare(insert_sql);

        QVariantList groupNames;
        groupNames.append(name);

        QVariantList groupPasswords;
        groupPasswords.append(password);

        sql_query.addBindValue(ID);
        sql_query.addBindValue(groupNames);
        sql_query.addBindValue(groupPasswords);

        if (!sql_query.execBatch())
        {
            qDebug() << sql_query.lastError();
        }
        else
        {
            qDebug() << "Insert record to Bmember success";
            QMessageBox::information(this, "Info", "Register success!");

            // 从表格和数据库中删除
            for (int row = 0; row < ui->register_info_tw->rowCount(); ++row)
            {
                QTableWidgetItem *item = ui->register_info_tw->item(row, 0);
                if (item->text().toInt() == ID)
                {
                    ui->register_info_tw->removeRow(row);
                    break;
                }
            }

            // 从数据库删除
            QString delete_sql = "delete from member where id = ?";
            sql_query.prepare(delete_sql);
            sql_query.addBindValue(ID);
            sql_query.exec();

            // 清除输入框
            ui->register_num_le->clear();
        }

        database.close();
    }
}
```

#### 删除当前选中项

```cpp
void manage::on_cur_removebtn_clicked()
{
    int row = ui->register_info_tw->currentRow();
    qDebug() << "row:" << row;

    if (row == -1)
    {
        QMessageBox::warning(this, "Warning", "Select your row!");
        return;
    }

    ui->register_info_tw->removeRow(row);
}
```

#### 删除全部项

```cpp
void manage::on_all_removebtn_clicked()
{
    QSqlDatabase database = QSqlDatabase::addDatabase("QSQLITE");
    database.setDatabaseName("CashSystem.db");

    if (database.open())
    {
        QSqlQuery sql_query;

        QString delete_all_sql = "delete from member";
        sql_query.prepare(delete_all_sql);

        if (!sql_query.exec())
        {
            qDebug() << "Delete all records failed!";
            return;
        }

        database.close();
    }
}
```

### 7.7 用户登陆功能实现

```cpp
void login::on_user_loginbtn_clicked()
{
    bool flag = false;

    QString user = ui->user_le->text();
    QString code = ui->code_le->text();

    if (user.isEmpty() || code.isEmpty())
    {
        QMessageBox::warning(this, "Warning", "user or code is empty, enter again!");
        return;
    }

    // 打开数据库
    QSqlDatabase database = QSqlDatabase::addDatabase("QSQLITE");
    database.setDatabaseName("CashSystem.db");
    database.open();

    QSqlQuery sql_query;

    // 查询所有记录
    QString select_all_sql = "select * from Bmember";
    sql_query.prepare(select_all_sql);

    if (sql_query.exec())
    {
        while (sql_query.next())
        {
            QString name = sql_query.value(1).toString();
            QString password = sql_query.value(2).toString();

            if (user == name && code == password)
            {
                flag = true;
                qDebug() << "The password is right!";

                // 打开用户菜单
                Menu *men = new Menu(this);
                men->show();
                this->hide();

                // 清除输入框
                ui->user_le->clear();
                ui->code_le->clear();

                break;
            }
        }
    }

    if (!flag)
    {
        // 清除输入框
        ui->user_le->clear();
        ui->code_le->clear();

        database.close();
        QMessageBox::warning(this, "Warning", "user or code is wrong, enter again!");
    }
    else
    {
        database.close();
    }
}
```

---

## 8. Qt5 vs Qt6 QSqlite 对比

| 方面 | Qt5 | Qt6 |
|------|-----|-----|
| 驱动名称 | `QSQLITE` | `QSQLITE` |
| API 兼容性 | 完整 | ✅ 完全兼容 |
| 模块引入 | `QT += sql` | `find_package(Qt6 REQUIRED COMPONENTS Sql)` |
| CMake 集成 | 可选 | 推荐 |

> [!TIP] 迁移建议
> Qt6 中的 QSqlite API 与 Qt5 基本一致，直接迁移即可，无需修改业务逻辑代码。

---

## 9. 相关资源

> [!NOTE] 相关文档
> - [[Qt5_framework_development]] - Qt5 框架开发指南
> - [[Qt6_learning]] - Qt6 新特性与迁移指南
> - [[PyQt6_learning]] - PyQt6 Python 绑定详解

---

> [!NOTE] 更新日志
> - **2026-03-29**: 完成格式美化与内容整理
> - **2026-03-29**: 新增 Qt5 vs Qt6 QSqlite 对比
> - **2026-03-29**: 添加 Obsidian callout 语法优化可读性