---
title: Qt6 学习指南
description: Qt6 新特性、重大变化、迁移指南与 Qt5 对比
tags: [Qt, Qt6, GUI, C++, Migration]
created: 2026-03-29
updated: 2026-03-29
source: 官方文档整理
related: [[Qt5_framework_development]], [[QSqlite_development]]
---

# Qt6 学习指南

> [!NOTE] 概述
> Qt6 是 Qt 框架的最新主版本，带来了现代化的架构改进和新的特性。本文介绍 Qt6 的新特性、与 Qt5 的主要区别，以及从 Qt5 迁移的最佳实践。

---

## 1. Qt6 概览

### 1.1 发布历史

| 版本 | 发布时间 | 说明 |
|------|----------|------|
| Qt 6.0 | 2020.12 | 首个 Qt6 主版本 |
| Qt 6.1 | 2021.05 | 引入 Qt Quick 3D |
| Qt 6.2 | 2022.09 | **LTS 版本**，功能稳定 |
| Qt 6.3 | 2022.06 | 改进性能 |
| Qt 6.4 | 2022.09 | 引入 QtHttpServer |
| Qt 6.5 | 2023.04 | 长期支持版本 |
| Qt 6.6 | 2023.09 | 持续改进 |
| Qt 6.7 | 2024.04 | 现代化改进 |
| Qt 6.8 | 2025.02 | 最新稳定版 |

> [!TIP] 版本选择建议
> - **生产环境**：推荐使用 Qt 6.5 LTS 或 6.6
> - **学习研究**：可使用最新版本体验新特性

### 1.2 Qt5 vs Qt6 核心对比

| 特性 | Qt5 | Qt6 |
|------|-----|-----|
| **最低 C++ 标准** | C++98/11 | C++17 |
| **构建系统** | qmake / CMake | **CMake 优先** |
| **图形架构** | OpenGL | **Vulkan / Metal / OpenGL** |
| **元对象系统** | 基础 | **现代化改进** |
| **QML** | QML 2 | **QML 3** |
| **SQL 模块** | ✅ 稳定 | ✅ 完全兼容 |
| **平台支持** | 广泛 | 持续扩展 |

---

## 2. 构建系统：CMake 优先

### 2.1 Qt5 qmake 项目示例

```qmake
# Qt5: *.pro 文件
QT += core gui sql

CONFIG += console
CONFIG -= app_bundle

SOURCES += \
    main.cpp \
    widget.cpp

HEADERS += \
    widget.h
```

### 2.2 Qt6 CMake 项目示例

```cmake
# Qt6: CMakeLists.txt
cmake_minimum_required(VERSION 3.16)
project(MyApp LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Sql Widgets)

qt_add_executable(myapp
    main.cpp
    widget.cpp
)

target_link_libraries(myapp PRIVATE Qt6::Widgets Qt6::Sql)
```

### 2.3 主要变化

| qmake (.pro) | CMakeLists.txt |
|--------------|----------------|
| `QT += sql` | `find_package(Qt6 REQUIRED COMPONENTS ...)` |
| `CONFIG += console` | 内置支持 |
| `SOURCES +=` | `qt_add_executable()` |

> [!TIP] 迁移建议
> 使用 Qt Creator 的 "CMake 迁移向导" 可以自动转换大部分项目。

---

## 3. C++ 标准升级

### 3.1 最低要求：C++17

Qt6 要求编译器支持 C++17 及以上：

```cpp
// Qt6 要求
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
```

### 3.2 新特性使用

```cpp
// 结构化绑定（Qt6 常用）
for (auto [key, value] : map.asKeyValueRange())
{
    qDebug() << key << value;
}

// constexpr（编译时计算）
constexpr auto version = QStringLiteral("6.8.0");
```

---

## 4. QSqlite 在 Qt6 中的变化

> [!NOTE] 好消息
> Qt6 中的 QSqlite 模块与 Qt5 **几乎完全兼容**，现有代码可以直接迁移。

### 4.1 API 对比

| 功能 | Qt5 代码 | Qt6 代码 |
|------|----------|----------|
| 添加驱动 | `QSqlDatabase::addDatabase("QSQLITE")` | ✅ 相同 |
| 设置数据库名 | `db.setDatabaseName("my.db")` | ✅ 相同 |
| 打开 | `db.open()` | ✅ 相同 |
| QSqlQuery | `QSqlQuery query(db)` | ✅ 相同 |

### 4.2 Qt6 CMake 项目中使用 QSqlite

```cmake
# CMakeLists.txt
find_package(Qt6 REQUIRED COMPONENTS Sql Widgets)

qt_add_executable(myapp
    main.cpp
    mainwindow.cpp
)

target_link_libraries(myapp PRIVATE Qt6::Sql)
```

```cpp
// main.cpp
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // 创建数据库连接
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("myapp.db");

    if (db.open())
    {
        QSqlQuery query;

        // 创建表
        query.exec("CREATE TABLE IF NOT EXISTS users ("
                   "id INTEGER PRIMARY KEY, "
                   "name TEXT NOT NULL)");

        // 插入数据
        query.exec("INSERT INTO users (name) VALUES ('Alice')");

        // 查询数据
        query.exec("SELECT * FROM users");
        while (query.next())
        {
            qDebug() << query.value(0).toInt()
                     << query.value(1).toString();
        }

        db.close();
    }
    else
    {
        qDebug() << "Database open failed!";
    }

    return 0;
}
```

---

## 5. 信号与槽的变化

### 5.1 Qt5 旧式连接

```cpp
// Qt5: 使用 SIGNAL/SLOT 宏
connect(sender, SIGNAL(valueChanged(int)), receiver, SLOT(setValue(int)));
```

### 5.2 Qt6 新式连接（推荐）

```cpp
// Qt6: 使用函数指针（编译时检查，更安全）
connect(sender, &Sender::valueChanged, receiver, &Receiver::setValue);

// Lambda 表达式（更灵活）
connect(sender, &Sender::valueChanged, [](int value) {
    qDebug() << "Value changed:" << value;
});
```

### 5.3 对比

| 方面 | 旧式 (SIGNAL/SLOT) | 新式 (函数指针) |
|------|-------------------|----------------|
| 编译时检查 | ❌ 运行时 | ✅ 编译时 |
| 类型安全 | ❌ | ✅ |
| 自动 completion | ❌ | ✅ |
| Qt5 兼容性 | ✅ | ✅（仍可用） |

> [!TIP] 推荐
> Qt6 中推荐使用**新式连接语法**，能获得更好的编译时检查和 IDE 自动补全支持。

---

## 6. QML 和 Qt Quick 更新

### 6.1 QML 3 新特性

```qml
// Qt6: QML 3 语法
import QtQuick
import QtQuick.Controls

ApplicationWindow {
    visible: true
    width: 800
    height: 600

    // 新的 ListView 语法
    ListView {
        anchors.fill: parent
        model: myModel
        delegate: ItemDelegate {
            text: model.display
            onClicked: console.log("Clicked")
        }
    }
}
```

### 6.2 Qt Quick 3D

Qt6 引入 Qt Quick 3D，支持 3D 场景：

```qml
import QtQuick
import QtQuick3D

View3D {
    anchors.fill: parent

    // 3D 场景
    Scene3D {
        camera: Camera {
            position: Qt.vector3d(0, 2, 5)
        }

        // 3D 立方体
        Cube {
            eulerRotation.y: rotation
        }

        // 旋转动画
        SequentialAnimation {
            running: true
            loops: Animation.Infinite
            PropertyAnimation {
                target: rotation
                property: "y"
                from: 0; to: 360
                duration: 3000
            }
        }
    }
}
```

---

## 7. 图形架构更新

### 7.1 Qt6 图形堆栈

```
Qt6 图形架构
├── Qt GUI (低级 API)
├── QPA (平台抽象)
└── 后端支持
    ├── Vulkan (推荐，跨平台)
    ├── Metal (macOS/iOS)
    ├── Direct3D (Windows)
    └── OpenGL (向后兼容)
```

### 7.2 迁移注意事项

> [!CAUTION] OpenGL 兼容性
> Qt6 保留 OpenGL 支持，但如果可能，建议迁移到现代图形 API（如 Vulkan）。

```cpp
// Qt5: OpenGL 直接使用
#include <QOpenGLFunctions>

// Qt6: 推荐使用 QOpenGLWidget（仍然支持）
#include <QOpenGLWidget>
```

---

## 8. 模块变化

### 8.1 Qt5 → Qt6 模块映射

| Qt5 模块 | Qt6 模块 | 说明 |
|----------|----------|------|
| `QtCore` | `Qt6::Core` | 核心功能 |
| `QtGui` | `Qt6::Gui` | 图形界面 |
| `QtWidgets` | `Qt6::Widgets` | 传统 Widget |
| `QtSql` | `Qt6::Sql` | 数据库 |
| `QtQml` | `Qt6::Qml` | QML 引擎 |
| `QtQuick` | `Qt6::Quick` | Quick 框架 |
| `QtQuick3D` | `Qt6::Quick3D` | 3D 支持 |

### 8.2 新增模块

| 模块 | 说明 |
|------|------|
| `QtHttpServer` | HTTP 服务器 (Qt 6.4+) |
| `QtShaderTools` | 着色器工具 |
| `Qt6::OpenGL` | 现代 OpenGL 支持 |

---

## 9. 迁移检查清单

> [!WARNING] 从 Qt5 迁移注意事项

### 9.1 代码层面

- [ ] 升级 C++ 标准到 C++17
- [ ] 将 `.pro` 文件转换为 `CMakeLists.txt`
- [ ] 更新 `QT +=` 为 `find_package(Qt6 ...)`
- [ ] 使用新式 `connect()` 语法
- [ ] 检查第三方库兼容性
- [ ] 测试数据库操作（QSqlite 应无变化）

### 9.2 CMake 迁移示例

```cmake
# Qt6 完整 CMakeLists.txt 示例
cmake_minimum_required(VERSION 3.16)
project(MyQt6App VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 启用一些有用的警告
if(CMAKE_CXX_COMPILER_ID IN ("GNU", "Clang", "AppleClang"))
    add_compile_options(-Wall -Wextra)
endif()

# 找到 Qt6
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Widgets
    Sql
)

# 添加可执行文件
qt_add_executable(myapp
    main.cpp
    mainwindow.cpp
    mainwindow.h
)

# 链接库
target_link_libraries(myapp PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Sql
)

# 设置输出目录
set_target_properties(myapp PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
```

---

## 10. 常见问题与解决方案

### Q1: 如何保留 Qt5 qmake 项目？

```bash
# Qt Creator 仍支持 qmake 项目
# 只是新项目推荐使用 CMake
```

### Q2: QSqlite 代码需要修改吗？

> [!SUCCESS] 答案
> **不需要**。Qt6 的 QSqlite API 与 Qt5 完全兼容，直接迁移即可。

### Q3: 如何检测 Qt6 版本？

```cpp
#include <QtCore/QtGlobal>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    qDebug() << "Running on Qt6";
#else
    qDebug() << "Running on Qt5";
#endif
```

### Q4: Widget 应用程序还能用吗？

> [!SUCCESS] 答案
> **可以**。Qt6 完整保留 Qt Widgets 模块，只是推荐在新项目中使用 QML。

---

## 11. 学习路径建议

```
┌─────────────────────────────────────────────────────────────┐
│                      Qt 学习路径                            │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│   1. Qt5 基础 ──→ 2. 信号与槽 ──→ 3. Widget 界面设计         │
│                                                             │
│           ↓                                                 │
│                                                             │
│   4. QSqlite 数据库操作                                      │
│                                                             │
│           ↓                                                 │
│                                                             │
│   5. Qt6 迁移 ──→ 6. CMake 构建 ──→ 7. QML 3 入门            │
│                                                             │
│           ↓                                                 │
│                                                             │
│   8. Qt Quick 3D（可选）                                    │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

---

## 12. 相关资源

> [!NOTE] 相关文档
> - [[Qt5_framework_development]] - Qt5 框架开发指南
> - [[QSqlite_development]] - SQLite 数据库操作详解
> - [[PyQt6_learning]] - PyQt6 Python 绑定详解

> [!TIP] 官方资源
> - [Qt 官方文档](https://doc.qt.io/)
> - [Qt6 迁移指南](https://doc.qt.io/qt-6/qt6-changes.html)
> - [CMake 手册](https://cmake.org/cmake/help/latest/)

---

> [!NOTE] 更新日志
> - **2026-03-29**: 创建 Qt6 学习文档
> - **2026-03-29**: 新增 Qt5 vs Qt6 详细对比
> - **2026-03-29**: 添加 CMake 迁移指南
> - **2026-03-29**: 说明 QSqlite 兼容性