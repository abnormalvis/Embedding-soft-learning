---
title: Qt5 框架开发指南
description: Qt5 信号与槽、界面设计、数据库操作等核心知识
tags: [Qt, Qt5, GUI, C++]
created: 2026-03-29
updated: 2026-03-29
source: 课程笔记整理
---

# Qt5 框架开发指南

> [!NOTE] 概述
> 本文档涵盖 Qt5 开发的核心理论与实践，包括工程架构、信号与槽机制、界面设计、数据库操作等。

---

## 1. Qt5 开发环境

> [!TIP] 环境配置
> 确保已安装 Qt5 库和 Qt Creator IDE。Qt5 支持 qmake 和 CMake 两种构建系统。

### 1.1 Qt5 vs Qt6 快速对比

| 特性 | Qt5 | Qt6 |
|------|-----|-----|
| 发布时间 | 2012 | 2020.12（LTS: 5.15） |
| 维护状态 | 维护中（5.15 LTS） | 活跃开发中 |
| 构建系统 | qmake / CMake | CMake 优先 |
| 图形架构 | OpenGL | Vulkan/Metal/OpenGL |
| 模块化 | Qt Labs 模块 | 正式模块化 |
| SQL 模块 | ✅ 稳定 | ✅ 基本兼容 |

---

## 2. 工程架构及文件属性

### Qt 工程架构介绍

![Qt工程架构图](https://cdn-mineru.openxlab.org.cn/result/2026-03-29/513786fa-82f6-487c-a4ab-dfcd32b3243f/a873151d6062b19b6fd7cf77889707773262cc7d7fbcfbfe87f7c79a82abb093.jpg)

#### 工程文件组成

| 文件类型 | 说明 |
|----------|------|
| `*.pro` | 工程管理文件（qmake 配置） |
| `*.h` | 头文件，定义类（包括基类及派生类） |
| `*.cpp` | 源文件，定义类的方法 |
| `main.cpp` | 程序入口，实例化对象并调用方法 |
| `*.ui` | 界面文件，Qt Designer 可视化设计 |

### 工程中各个文件及其之间的关系

Qt 工程中存在三个核心类/概念：

#### 第一个：UI 界面类（namespace Ui）

```cpp
namespace Ui { class mytest; }
```

> [!INFO] 说明
> 定义在 `*.ui` 文件中，用户通过可视化界面设计操作，编译时转换为 `ui_*.h` 文本文件，用于关联逻辑代码。

#### 第二个：逻辑类（class mytest）

```cpp
class mytest : public QMainWindow
{
    Q_OBJECT  // 启用元对象系统（信号与槽）

public:
    explicit mytest(QWidget *parent = 0);
    ~mytest();

private:
    Ui::mytest *ui;  // 指向 UI 界面类实例
};
```

> [!IMPORTANT] Q_OBJECT 作用
> 只有加入了 `Q_OBJECT`，才能使用 Qt 的信号与槽机制，以及元对象系统的其他功能（如属性、动态属性）。

> [!TIP] explicit 构造函数
> 声明为 `explicit` 的构造函数不能在隐式转换中使用，防止意外的类型转换。

#### 第三个：对象创建（main.cpp）

```cpp
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    mytest w;
    w.show();

    return a.exec();
}
```

> [!INFO] QApplication 作用
> 管理图形用户界面应用程序的控制流和主要设置。它包含主事件循环，在其中来自窗口系统和其它资源的所有事件被处理和调度。

![信号与槽关系图](https://cdn-mineru.openxlab.org.cn/result/2026-03-29/513786fa-82f6-487c-a4ab-dfcd32b3243f/acf5ee4c90dbcce6798408001aa7d93cd785c74f036524cfa3fd6afc7a4c1b0f.jpg)

---

## 3. Qt Designer 界面设计工具

Qt 提供 Designer 可视化界面设计工具，大幅提升开发效率：

![Qt Designer 界面](https://cdn-mineru.openxlab.org.cn/result/2026-03-29/513786fa-82f6-487c-a4ab-dfcd32b3243f/a05c46a5fa4988123f806e923d4c73ab0f92282444761d325fe0c749a715fcc0.jpg)

> [!TIP] 工具栏组件
> Qt Designer 提供了大量现成组件，灵活使用可大大提高项目开发效率。

### 常用控件示例

1. **Label** - 标签控件（显示文本/图片）
2. **LineEdit** - 单行输入框
3. **PushButton** - 按钮控件

### 简单示例演示

![简单控件示例](https://cdn-mineru.openxlab.org.cn/result/2026-03-29/513786fa-82f6-487c-a4ab-dfcd32b3243f/edde588a68eb0d474dfd4be8be92c0de6ffdf9df37328f213e2282e873e34cd0.jpg)

---

## 4. Qt 信号与槽

### 4.1 信号与槽介绍

#### 定义

> [!ABSTRACT] 核心定义
> 信号和槽机制是 Qt 的核心机制，要精通 Qt 编程就必须对信号和槽有所了解。
>
> 信号和槽是一种高级接口，应用于对象之间的通信，它是 Qt 的核心特性，也是 Qt 区别于其它工具包的重要地方。
>
> 信号和槽是 Qt 自行定义的一种通信机制，它独立于标准的 C/C++ 语言。

#### 关于信号与槽的理解

| 概念 | 说明 |
|------|------|
| **信号 (Signal)** | 某个物体被操作后发出的消息，例如按钮被点击时发出 `clicked()` 信号 |
| **槽 (Slot)** | 接收信号后执行的具体操作，例如标签收到 `clicked()` 信号后显示笑脸 |

> [!TIP] 广播机制
> 在 Qt 中，当一个信号被发送时，按照类似广播的方式发送，每一个槽都能看到这个信号，但只有连接过的槽才会接收并响应。

### 4.2 通过 Qt Designer 实现信号与槽

#### 操作步骤

1. **建立工程** - 创建 Qt Widgets Application

2. **添加按钮** - 从控件面板拖入 PushButton

3. **跳转到槽** - 右键按钮 → "跳转到槽" → 选择信号（如 `clicked`）

4. **Qt Creator 自动生成槽函数**：

```cpp
// mybutton.h
private slots:
    void on_pushButton_clicked();  // 自动命名：on_<对象名>_<信号名>()

// mybutton.cpp
void mybutton::on_pushButton_clicked()
{
    qDebug() << "Button clicked!";
}
```

#### 自动生成代码解析

在 `ui->setupUi()` 中自动添加：

```cpp
QMetaObject::connectSlotsByName(mybutton);
```

> [!INFO] connectSlotsByName 机制
> 此函数会自动：
> 1. 创建名为 `on_<objectName>_<signal>()` 的槽函数
> 2. 调用 `connect()` 建立信号-槽关联
> 3. 无需用户手动编写连接代码

---

### 4.3 信号描述

> [!NOTE] 信号特性
> - 当某个信号对其客户或所有者发生的内部状态发生改变，信号被一个对象发射
> - 只有定义过这个信号的类及其派生类能够发射这个信号
> - 当一个信号被发射时，与其相关联的槽将被立刻执行
> - 信号-槽机制完全独立于任何 GUI 事件循环
> - 只有当所有的槽返回以后 `emit` 才返回

> [!CAUTION] 顺序不确定性
> 如果存在多个槽与某个信号相关联，当信号被发射时，这些槽会一个接一个执行，但执行顺序是随机的、不确定的。

### 槽描述

> [!NOTE] 槽函数特性
> - 槽是普通的 C++ 成员函数，可以被正常调用
> - 唯一的特殊性是很多信号可以与其相关联
> - 槽可以有参数，但参数**不能有缺省值**
> - 槽也能够声明为虚函数

#### 槽的访问权限

| 权限类型 | 说明 |
|----------|------|
| `public slots` | 任何对象都可将信号与之相连接 |
| `protected slots` | 当前类及其子类可以将信号与之相连接 |
| `private slots` | 只有类自己可以将信号与之相连接 |

---

### 4.4 信号与槽分类

#### 第一类：类自带信号与槽

Qt 预定义在各个类中，无需用户定义即可直接使用。

**示例：查询 QLabel 类的信号与槽**

![QWidget 类层次](https://cdn-mineru.openxlab.org.cn/result/2026-03-29/513786fa-82f6-487c-a4ab-dfcd32b3243f/22484701e113e66dc7461812570419e6eb14fd3d40ffde8b58997379221fb749.jpg)

![QLabel 信号与槽](https://cdn-mineru.openxlab.org.cn/result/2026-03-29/513786fa-82f6-487c-a4ab-dfcd32b3243f/cacdc1229f81874809dae895bf14710ad886e29e55d4655aea3cd55304bf3a06.jpg)

#### 第二类：自定义信号与槽

当平台预定义的信号与槽无法满足需求时，需要自定义实现。

---

### 4.5 自定义类实现信号与槽

#### 实现流程

```
┌─────────────────────────────────────────────────────────────┐
│  1. 定义信号  →  2. 定义槽函数  →  3. 实现槽函数           │
│           ↓                                              │
│  4. 绑定信号与槽  →  5. 触发信号                           │
└─────────────────────────────────────────────────────────────┘
```

#### 步骤 1：定义信号

在头文件中使用 `signals` 关键字声明：

```cpp
class mysigSlot : public QMainWindow
{
    Q_OBJECT

public:
    explicit mysigSlot(QWidget *parent = 0);
    ~mysigSlot();

signals:
    void mySignal();              // 无参数信号
    void mySignal(int x);        // 带参数信号
    void mySignalParam(int x, int y);  // 多参数信号

private slots:
    void mySlot();
    void mySlot(int x);
    void mySlotParam(int x, int y);

private:
    Ui::mysigSlot *ui;
};
```

> [!INFO] 信号声明规则
> - 使用 `signals` 关键字
> - 返回类型必须为 `void`
> - 可以传参，也可不传参

#### 步骤 2：定义槽响应函数

在头文件中使用 `slots` 关键字声明：

```cpp
public slots:
    void mySlot();
    void mySlot(int x);
    void mySlotParam(int x, int y);
```

#### 步骤 3：实现槽响应函数

```cpp
// mysigslot.cpp

// 实现槽函数
void mysigSlot::myslot(int num)
{
    qDebug() << "Received signal with value:" << num;
}

void mysigSlot::myslot2(QString &str)
{
    qDebug() << "Received QString:" << str;
}
```

#### 步骤 4：绑定信号与槽

使用 `QObject::connect()` 函数：

```cpp
// connect() 函数原型
bool QObject::connect(
    const QObject *sender,      // 信号发送者
    const char *signal,        // 信号（使用 SIGNAL() 宏）
    const QObject *receiver,   // 信号接收者
    const char *member         // 槽函数（使用 SLOT() 宏）
);

// 示例
connect(this, SIGNAL(mysignal(int)), this, SLOT(myslot(int)));
connect(this, SIGNAL(mysignal2(QString &)), this, SLOT(myslot2(QString &)));
```

> [!TIP] 宏说明
> - `SIGNAL()` 宏：指定信号
> - `SLOT()` 宏：指定槽函数
> - 信号发送者与接收者属于同一对象时，接收者参数可省略

#### 步骤 5：触发信号

使用 `emit` 关键字发射信号：

```cpp
void mysigSlot::mysigSlot(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::mysigSlot)
{
    ui->setupUi(this);

    // 绑定信号与槽
    connect(this, SIGNAL(mysignal(int)), this, SLOT(myslot(int)));
    connect(this, SIGNAL(mysignal2(QString &)), this, SLOT(myslot2(QString)));

    // 触发信号
    emit mysignal(100);  // 发送 int 参数

    QString str("hello Qt");
    emit mysignal2(str);  // 发送 QString 参数
}
```

#### 调试验证

| 步骤 | 结果 |
|------|------|
| 完成步骤 1-5 | 可正常传递 int 型数据 |
| 完成步骤 6-10（扩展） | 可正常传递 QString 数据 |

> [!EXERCISE] 课后练习
> 设计信号与槽代码，实现传递 `QLabel*` 型参数。

---

### 4.6 预定义类信号发送原理分析

以 QPushButton 为例分析信号发送机制：

#### 1. 信号定义位置

```cpp
// qpushbutton.h → qabstractbutton.h
// 信号定义：
signals:
    void pressed();              // 按下
    void released();             // 弹开
    void clicked(bool checked = false);  // 按下弹开
    void toggled(bool checked);  // 切换状态
```

> [!INFO] 说明
> 按键已预定义了一系列信号，对按键操作时只要符合相应条件，均会发出对应信号，无需用户定义。

#### 2. 自动关联机制

```cpp
// ui_*.h 中 setupUi() 函数
void setupUi(QMainWindow *mybutton)
{
    // 执行"跳转到槽"后自动添加：
    QMetaObject::connectSlotsByName(mybutton);
}
```

> [!INFO] connectSlotsByName 作用
> 自动创建名为 `on_<controlName>_<signalType>()` 的槽函数，并调用 `connect()` 建立关联。

#### 3. 自定义连接示例

如果不想使用自动槽命名，可以手动连接：

```cpp
// 手动连接（替代自动连接）
connect(ui->pushButton, &QPushButton::clicked, this, &mybutton::myCustomSlot);
```

> [!CAUTION] 注意事项
> 若手动连接，需要在 `ui_*.h` 中注释掉 `QMetaObject::connectSlotsByName()` 调用，避免重复连接。

---

### 4.7 项目小案例：简单计算器

#### 实验要求

实现一个简单计算器界面，支持加减乘除运算。

![计算器界面](https://cdn-mineru.openxlab.org.cn/result/2026-03-29/513786fa-82f6-487c-a4ab-dfcd32b3243f/672b13df8592480538c39f92a1f8dc21fdb6040ff5270f7b39d41daa5ee56c5b.jpg)

#### 关键函数实现

##### 1. 类型转换

```cpp
// QString 转 double
double firstNum = ui->firstNum_add_le->text().toDouble();
double secondNum = ui->secondNum_add_le->text().toDouble();

// double 转 QString
ui->addVal_le->setText(QString::number(result));
```

##### 2. 加法运算

```cpp
void calculator::on_equal_addbt_clicked()
{
    // 获取两个数值
    QString firstStr = ui->firstNum_add_le->text();
    QString secondStr = ui->secondNum_add_le->text();

    // 转换为 double 并运算
    double result = firstStr.toDouble() + secondStr.toDouble();

    // 显示结果
    ui->addVal_le->setText(QString::number(result));
}
```

##### 3. 其他运算（模式相同）

| 运算 | 代码 | 槽函数 |
|------|------|--------|
| 减法 | `firstStr.toDouble() - secondStr.toDouble()` | `on_equal_subbt_clicked()` |
| 乘法 | `firstStr.toDouble() * secondStr.toDouble()` | `on_equal_mulbt_clicked()` |
| 除法 | `firstStr.toDouble() / secondStr.toDouble()` | `on_equal_divbt_clicked()` |

##### 4. 清除操作

```cpp
void calculator::on_add_clearbt_clicked()
{
    ui->firstNum_add_le->clear();
    ui->secondNum_add_le->clear();
    ui->addVal_le->clear();
}
```

---

## 5. Qt 界面设计

### 5.1 界面布局的重要性

> [!WARNING] 问题
> 没有使用布局管理器的 UI 界面，各个控件之间不是一个整体，在界面放大缩小时会导致变形。

![未使用布局的界面](https://cdn-mineru.openxlab.org.cn/result/2026-03-29/513786fa-82f6-487c-a4ab-dfcd32b3243f/8c3edeef8e2d91ae76bd1b71f738c95c9c67f7e8e7827fea2ab78bf2469c0648.jpg)

点击网格布局后，控件组合成一个整体：

![使用布局后的界面](https://cdn-mineru.openxlab.org.cn/result/2026-03-29/513786fa-82f6-487c-a4ab-dfcd32b3243f/7ded1b306d0884a59391fa90f49ed0b6217b01a5d32409a33ad140d6989f6c5b.jpg)

### 5.2 三种布局管理器

| 布局类型 | 说明 | 快捷键 |
|----------|------|--------|
| **水平布局 (HBox)** | 从左到右依次排列 | `Ctrl + H` |
| **垂直布局 (VBox)** | 从上到下依次排列 | `Ctrl + L` |
| **网格布局 (Grid)** | 以矩阵形式排列 | `Ctrl + G` |

> [!TIP] 布局技巧
> 可以通过 Qt Designer 的快捷键快速应用布局，或在对象查看器中选择多个控件后应用。

### 5.3 布局管理器使用示例

#### 布局步骤演示

**Step 1**: 选择第一排按钮 → 水平布局

![水平布局第1排](https://cdn-mineru.openxlab.org.cn/result/2026-03-29/513786fa-82f6-487c-a4ab-dfcd32b3243f/274dc06a83f7ec3c7739dc32f3c02b0dfbbefdf1ce6d2b7c5b79395adbde2467.jpg)

**Step 2**: 选择第二排按钮 → 水平布局

**Step 3**: 选择全部按钮组 → 垂直布局

![垂直布局所有行](https://cdn-mineru.openxlab.org.cn/result/2026-03-29/14473846292f14e8ecfd2d3a34756aa23f71939d637b6cb4ec0ac713cfaa344a.jpg)

**Step 4**: 选中所有组件 → 网格布局

![最终网格布局](https://cdn-mineru.openxlab.org.cn/result/2026-03-29/513786fa-82f6-487c-a4ab-dfcd32b3243f/697a48f6d9e1df69b49d43c4f18d63793457db6d2bf5e8d2980e2769f5cfeb57.jpg)

#### 填充空白空间

选中所有按钮 → 设置 `sizePolicy`：

| 属性 | 值 | 说明 |
|------|-----|------|
| horizontalPolicy | Expanding | 水平方向扩展 |
| verticalPolicy | Expanding | 垂直方向扩展 |

> [!TIP] 效果
> 按钮会自动填满分配的空间，使界面更加美观。

---

## 6. 相关资源

> [!NOTE] 相关文档
> - [[QSqlite_development]] - SQLite 数据库操作详解
> - [[Qt6_learning]] - Qt6 新特性与迁移指南
> - [[PyQt6_learning]] - PyQt6 Python 绑定详解

---

> [!NOTE] 更新日志
> - **2026-03-29**: 完成格式美化与内容整理
> - **2026-03-29**: 新增 Qt5 vs Qt6 对比表格
> - **2026-03-29**: 添加 Obsidian callout 语法优化可读性