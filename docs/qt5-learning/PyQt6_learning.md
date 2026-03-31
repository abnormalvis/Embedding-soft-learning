---
title: PyQt6 学习指南
description: Python 版 Qt6 GUI 开发完整指南，涵盖 PyQt6 与 Qt/C++ 版的对比与迁移
tags: [Python, PyQt, PyQt6, GUI, Qt6]
created: 2026-03-29
updated: 2026-03-29
source: 官方文档整理
related: [[Qt6_learning]], [[Qt5_framework_development]], [[QSqlite_development]]
---

# PyQt6 学习指南

> [!NOTE] 概述
> PyQt6 是 Qt6 框架的 Python 绑定，允许使用 Python 语言开发跨平台 GUI 应用程序。本文介绍 PyQt6 的安装、基本用法、与 C++ Qt6 的对比，以及从 PyQt5 迁移的最佳实践。

---

## 1. PyQt6 概览

### 1.1 什么是 PyQt6？

PyQt6 是 Riverbank Computing 公司开发的 Qt6 框架 Python 绑定：

| 项目 | 说明 |
|------|------|
| **PyQt6** | Qt6 的 Python 绑定，功能完整 |
| **PySide6** | Qt 官方 Python 绑定（Qt for Python） |
| **两者关系** | PySide6 是官方版，PyQt6 是第三方绑定，功能基本一致 |

> [!TIP] 选择建议
> - **PySide6（推荐）**：Qt 官方维护，许可证更宽松
> - **PyQt6**：功能完整，文档丰富

### 1.2 Qt 家族版本对比

| 版本 | 语言 | 说明 |
|------|------|------|
| **Qt5** (C++) | C++ | 成熟的 C++ GUI 框架 |
| **Qt6** (C++) | C++ | Qt 最新主版本，CMake 构建 |
| **PyQt5** | Python | Qt5 的 Python 绑定 |
| **PyQt6** | Python | Qt6 的 Python 绑定 |
| **PySide6** | Python | Qt 官方的 Python 绑定 |

### 1.3 PyQt6 vs C++ Qt6 对比

| 方面 | PyQt6 (Python) | Qt6 C++ |
|------|---------------|---------|
| **性能** | 稍慢（解释执行） | 最快（编译执行） |
| **开发速度** | 快（动态语言） | 较慢（静态语言） |
| **跨平台** | ✅ 完全一致 | ✅ 完全一致 |
| **API** | 几乎 1:1 映射 | 原生 |
| **数据库支持** | ✅ QSqlite 相同 | ✅ 相同 |
| **适用场景** | 快速原型、数据处理工具 | 高性能应用、游戏 |

---

## 2. 安装与环境配置

### 2.1 安装 PyQt6

```bash
# pip 安装 PyQt6
pip install PyQt6

# 或安装 PySide6（Qt 官方版）
pip install PySide6

# 检查安装
python -c "import PyQt6; print(PyQt6.__version__)"
```

### 2.2 安装 Qt Designer（辅助工具）

```bash
# Qt Designer 是 Qt 自带的可视化 UI 设计工具
# 安装 Qt6 后通常自带
# 在 PyCharm/VSCode 中可配置 external tools 使用
```

### 2.3 开发环境推荐

| IDE | 说明 |
|-----|------|
| **PyCharm** | 强大的 Python IDE，代码补全好 |
| **VS Code** | 轻量级，配合 Pylance 插件 |
| **Qt Creator** | Qt 官方 IDE，适合设计 UI |

---

## 3. PyQt6 核心概念

### 3.1 基本应用程序结构

```python
import sys
from PyQt6.QtWidgets import QApplication, QMainWindow, QPushButton, QLabel, QVBoxLayout, QWidget
from PyQt6.QtCore import Qt


class MyWindow(QMainWindow):
    """主窗口类"""

    def __init__(self):
        super().__init__()
        self.setWindowTitle("PyQt6 示例")
        self.setFixedSize(400, 300)

        # 创建中心部件
        central_widget = QWidget()
        self.setCentralWidget(central_widget)

        # 创建布局
        layout = QVBoxLayout(central_widget)

        # 创建标签
        self.label = QLabel("Hello, PyQt6!", alignment=Qt.AlignmentFlag.AlignCenter)
        layout.addWidget(self.label)

        # 创建按钮
        self.button = QPushButton("点击我")
        self.button.clicked.connect(self.on_button_clicked)
        layout.addWidget(self.button)

    def on_button_clicked(self):
        self.label.setText("按钮被点击了！")


def main():
    app = QApplication(sys.argv)

    window = MyWindow()
    window.show()

    sys.exit(app.exec())


if __name__ == "__main__":
    main()
```

> [!INFO] 代码说明
> - `QApplication`：管理应用程序的主事件循环
> - `QMainWindow`：提供主窗口框架（菜单栏、工具栏、状态栏）
> - `sys.argv`：命令行参数（PyQt6 需要）

### 3.2 信号与槽机制（PyQt6）

PyQt6 的信号与槽与 C++ Qt6 几乎完全对应：

```python
from PyQt6.QtCore import pyqtSignal, QObject


class MyEmitter(QObject):
    """自定义信号发射器"""

    # 定义信号
    my_signal = pyqtSignal()  # 无参数信号
    my_signal_int = pyqtSignal(int)  # 带 int 参数
    my_signal_str = pyqtSignal(str)  # 带 str 参数

    def trigger_signals(self):
        """触发信号"""
        self.my_signal.emit()
        self.my_signal_int.emit(42)
        self.my_signal_str.emit("Hello from PyQt6!")


# 使用示例
class MyWindow(QMainWindow):
    def __init__(self):
        super().__init__()

        self.emitter = MyEmitter()

        # 连接信号到槽
        self.emitter.my_signal.connect(self.on_signal_received)
        self.emitter.my_signal_int.connect(self.on_int_received)
        self.emitter.my_signal_str.connect(self.on_str_received)

        # 触发信号
        self.emitter.trigger_signals()

    def on_signal_received(self):
        print("Received basic signal!")

    def on_int_received(self, value):
        print(f"Received int: {value}")

    def on_str_received(self, text):
        print(f"Received string: {text}")
```

### 3.3 使用 Lambda 连接信号

```python
from PyQt6.QtWidgets import QPushButton, QVBoxLayout, QWidget


class MyWindow(QMainWindow):
    def __init__(self):
        super().__init__()

        central_widget = QWidget()
        self.setCentralWidget(central_widget)
        layout = QVBoxLayout(central_widget)

        # 使用 Lambda 传递额外参数
        for i in range(5):
            btn = QPushButton(f"按钮 {i}")
            btn.clicked.connect(lambda checked, x=i: self.button_clicked(x))
            layout.addWidget(btn)

    def button_clicked(self, index):
        print(f"点击了按钮 {index}")
```

---

## 4. PyQt6 常用控件

### 4.1 基础控件一览

| 控件 | PyQt6 类 | 说明 |
|------|----------|------|
| 窗口 | `QMainWindow` | 主窗口 |
| 按钮 | `QPushButton` | 按钮 |
| 标签 | `QLabel` | 显示文本/图片 |
| 输入框 | `QLineEdit` | 单行文本输入 |
| 多行文本 | `QTextEdit` | 多行文本编辑 |
| 复选框 | `QCheckBox` | 多选 |
| 单选框 | `QRadioButton` | 单选 |
| 下拉框 | `QComboBox` | 下拉选择 |
| 列表 | `QListWidget` | 列表显示 |
| 表格 | `QTableWidget` | 表格显示 |
| 滑块 | `QSlider` | 滑动条 |
| 进度条 | `QProgressBar` | 进度显示 |

### 4.2 布局管理器

```python
from PyQt6.QtWidgets import (
    QVBoxLayout,   # 垂直布局
    QHBoxLayout,   # 水平布局
    QGridLayout,   # 网格布局
    QFormLayout,   # 表单布局
)


class LayoutDemo(QMainWindow):
    def __init__(self):
        super().__init__()

        central_widget = QWidget()
        self.setCentralWidget(central_widget)

        # 垂直布局
        v_layout = QVBoxLayout()

        # 水平布局
        h_layout = QHBoxLayout()
        h_layout.addWidget(QPushButton("左"))
        h_layout.addWidget(QPushButton("右"))

        # 网格布局
        grid_layout = QGridLayout()
        for row in range(3):
            for col in range(3):
                grid_layout.addWidget(QPushButton(f"{row},{col}"), row, col)

        # 组合布局
        v_layout.addLayout(h_layout)
        v_layout.addLayout(grid_layout)

        central_widget.setLayout(v_layout)
```

---

## 5. 菜单栏与工具栏

### 5.1 创建菜单栏

```python
from PyQt6.QtWidgets import QMainWindow, QMenuBar, QMenu
from PyQt6.QtGui import QAction


class MenuDemo(QMainWindow):
    def __init__(self):
        super().__init__()

        # 创建菜单栏
        menubar = self.menuBar()

        # 创建"文件"菜单
        file_menu = menubar.addMenu("文件(&F)")

        # 创建动作
        new_action = QAction("新建", self)
        new_action.setShortcut("Ctrl+N")
        new_action.triggered.connect(self.on_new)

        open_action = QAction("打开", self)
        open_action.setShortcut("Ctrl+O")
        open_action.triggered.connect(self.on_open)

        save_action = QAction("保存", self)
        save_action.setShortcut("Ctrl+S")
        save_action.triggered.connect(self.on_save)

        # 分隔符
        file_menu.addAction(new_action)
        file_menu.addAction(open_action)
        file_menu.addAction(save_action)
        file_menu.addSeparator()
        file_menu.addAction("退出", self.close)

        # 创建"编辑"菜单
        edit_menu = menubar.addMenu("编辑(&E)")
        edit_menu.addAction("撤销")
        edit_menu.addAction("重做")

    def on_new(self):
        print("新建文件")

    def on_open(self):
        print("打开文件")

    def on_save(self):
        print("保存文件")
```

### 5.2 创建工具栏

```python
from PyQt6.QtWidgets import QToolBar


class ToolbarDemo(QMainWindow):
    def __init__(self):
        super().__init__()

        # 创建工具栏
        toolbar = QToolBar("主工具栏")
        self.addToolBar(toolbar)

        # 添加动作到工具栏
        toolbar.addAction("新建")
        toolbar.addAction("打开")
        toolbar.addAction("保存")
```

---

## 6. PyQt6 中的 QSqlite 操作

> [!NOTE] 好消息
> PyQt6 的 QSqlite API 与 C++ Qt6 完全对应，数据库操作代码风格类似。

### 6.1 基本数据库操作

```python
import sqlite3  # Python 内置 SQLite
from PyQt6.QtWidgets import QMainWindow, QTableWidget, QTableWidgetItem
from PyQt6.QtCore import Qt


class DatabaseDemo(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("PyQt6 数据库示例")

        # 创建表格
        self.table = QTableWidget()
        self.setCentralWidget(self.table)

        # 初始化数据库
        self.init_database()

    def init_database(self):
        """初始化数据库"""
        conn = sqlite3.connect("demo.db")
        cursor = conn.cursor()

        # 创建表
        cursor.execute("""
            CREATE TABLE IF NOT EXISTS users (
                id INTEGER PRIMARY KEY,
                name TEXT NOT NULL,
                email TEXT
            )
        """)

        # 插入数据
        cursor.execute("INSERT INTO users (name, email) VALUES (?, ?)",
                       ("Alice", "alice@example.com"))
        cursor.execute("INSERT INTO users (name, email) VALUES (?, ?)",
                       ("Bob", "bob@example.com"))

        conn.commit()
        conn.close()

    def load_data(self):
        """加载数据到表格"""
        conn = sqlite3.connect("demo.db")
        cursor = conn.cursor()

        cursor.execute("SELECT * FROM users")
        rows = cursor.fetchall()

        self.table.setRowCount(len(rows))
        self.table.setColumnCount(3)
        self.table.setHorizontalHeaderLabels(["ID", "姓名", "邮箱"])

        for row_idx, row_data in enumerate(rows):
            for col_idx, value in enumerate(row_data):
                item = QTableWidgetItem(str(value))
                item.setTextAlignment(Qt.AlignmentFlag.AlignCenter)
                self.table.setItem(row_idx, col_idx, item)

        conn.close()
```

### 6.2 使用 PyQt6 QSql（可选）

```python
# PyQt6 也支持使用 QSql 模块（与 C++ API 一致）
from PyQt6.QtSql import QSqlDatabase, QSqlQuery


class QSqlDemo(QMainWindow):
    def __init__(self):
        super().__init__()

        # 创建数据库连接
        db = QSqlDatabase.addDatabase("QSQLITE")
        db.setDatabaseName("demo.db")

        if db.open():
            query = QSqlQuery()

            # 创建表
            query.exec("""
                CREATE TABLE IF NOT EXISTS products (
                    id INTEGER PRIMARY KEY,
                    name TEXT NOT NULL,
                    price REAL
                )
            """)

            # 插入数据
            query.exec("INSERT INTO products (name, price) VALUES ('Apple', 3.5)")

            # 查询数据
            query.exec("SELECT * FROM products")
            while query.next():
                print(query.value(0), query.value(1), query.value(2))

            db.close()
```

---

## 7. PyQt5 vs PyQt6 主要变化

### 7.1 枚举值变化

> [!WARNING] 重要变化
> PyQt6 将枚举值改为枚举类，需要使用 `EnumClass.MEMBER` 语法。

| PyQt5 | PyQt6 |
|-------|-------|
| `Qt.AlignCenter` | `Qt.AlignmentFlag.AlignCenter` |
| `Qt.Vertical` | `Qt.Orientation.Vertical` |
| `Qt.Window` | `Qt.WindowFlag.Window` |
| `Qt.Qt.LeftButton` | `Qt.MouseButton.LeftButton` |

### 7.2 代码迁移示例

**PyQt5 代码：**
```python
# PyQt5
self.label.setAlignment(Qt.AlignCenter)
button.setText("Click me")
```

**PyQt6 代码：**
```python
# PyQt6
from PyQt6.QtCore import Qt

self.label.setAlignment(Qt.AlignmentFlag.AlignCenter)
button.setText("Click me")
```

### 7.3 其他变化

| 项目 | PyQt5 | PyQt6 |
|------|-------|-------|
| `QApplication.arguments()` | `sys.argv` 需要手动处理 | 相同 |
| 枚举方式 | `Qt.SomeEnum` | `Qt.EnumClass.SomeMember` |
| 信号定义 | `pyqtSignal()` | 相同 |
| QAction 文本 | `setText()` 或构造函数 | 相同 |

---

## 8. Qt Designer 与 UI 文件

### 8.1 使用 .ui 文件

Qt Designer 创建的 `.ui` 文件可以通过 `uic` 加载：

```python
from PyQt6.QtWidgets import QMainWindow
from PyQt6.QtUiTools import QUiLoader


class MyWindow(QMainWindow):
    def __init__(self):
        super().__init__()

        # 加载 .ui 文件
        loader = QUiLoader()
        self.ui = loader.load("mywindow.ui")
        self.setCentralWidget(self.ui.central_widget)

        # 连接信号
        self.ui.pushButton.clicked.connect(self.on_button_clicked)
```

### 8.2 将 .ui 转换为 Python 代码

```bash
# 使用 pyside6-uic 或 pyuic6
pyside6-uic mainwindow.ui -o ui_mainwindow.py

# 或
pyuic6 mainwindow.ui -o ui_mainwindow.py
```

```python
# 使用生成的 UI 代码
from ui_mainwindow import Ui_MainWindow


class MyWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)

        # 连接信号
        self.ui.pushButton.clicked.connect(self.on_button_clicked)
```

---

## 9. PyQt6 vs PySide6 选择

| 方面 | PyQt6 | PySide6 |
|------|-------|---------|
| **维护者** | Riverbank Computing | Qt 官方 (The Qt Company) |
| **许可证** | GPL / 商业 | LGPL / 商业 |
| **API 一致性** | 与 Qt6 完全一致 | 与 Qt6 完全一致 |
| **文档** | PyQt 特定文档 | Qt 官方文档 |
| **推荐场景** | 已有 PyQt5 项目 | 新项目 |

> [!TIP] 推荐
> 新项目推荐使用 **PySide6**（Qt 官方维护），因为：
> - 许可证更宽松（LGPL）
> - 与 Qt 官方文档一致
> - 未来维护更有保障

---

## 10. 常见问题与解决方案

### Q1: 如何检测 PyQt6 版本？

```python
import PyQt6
print(PyQt6.QtCore.QT_VERSION_STR)  # Qt 版本
print(PyQt6.__version__)  # PyQt6 版本
```

### Q2: PyQt6 能做什么类型的应用？

| 应用类型 | 示例 |
|----------|------|
| 桌面应用 | 文件管理器、播放器 |
| 数据可视化 | 图表、仪表盘 |
| 图像处理 | 简单编辑器、滤镜 |
| 工具软件 | 数据库管理、配置工具 |
| 游戏（轻量） | 2D 小游戏 |

### Q3: PyQt6 vs Tkinter 选哪个？

| 方面 | PyQt6 | Tkinter |
|------|-------|---------|
| 功能 | 丰富 | 基础 |
| 外观 | 原生风格 | 较旧 |
| 学习曲线 | 中等 | 简单 |
| 适合项目 | 中大型应用 | 小型工具 |

### Q4: 如何打包 PyQt6 应用？

```bash
# 使用 PyInstaller
pip install pyinstaller

# 打包
pyinstaller --onefile --windowed myapp.py

# 参数说明
# --onefile: 单文件输出
# --windowed: 无控制台窗口（GUI 应用）
```

---

## 11. 学习路径建议

```
┌─────────────────────────────────────────────────────────────┐
│                    Python GUI 开发学习路径                   │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│   1. Python 基础 ──→ 2. PyQt6/PySide6 安装                  │
│                                                             │
│           ↓                                                 │
│                                                             │
│   3. 基本控件 ──→ 4. 布局管理 ──→ 5. 信号与槽                │
│                                                             │
│           ↓                                                 │
│                                                             │
│   6. 高级控件 ──→ 7. 数据库 ──→ 8. 样式与主题               │
│                                                             │
│           ↓                                                 │
│                                                             │
│   9. Qt Designer ──→ 10. 项目实战 ──→ 11. 打包发布           │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

---

## 12. Qt/PyQt 全家桶对照表

```
┌──────────────────────────────────────────────────────────────────┐
│                          Qt 家族                                │
├──────────────────────────────────────────────────────────────────┤
│                                                                  │
│   ┌─────────────────┐         ┌─────────────────┐              │
│   │    Qt5 (C++)    │ ←─────→ │    Qt6 (C++)    │              │
│   │   2012-2020     │         │   2020-Present  │              │
│   └────────┬────────┘         └────────┬────────┘              │
│            │                           │                       │
│     ┌──────┴──────┐             ┌──────┴──────┐              │
│     ↓             ↓             ↓             ↓              │
│ ┌───────┐   ┌───────┐     ┌───────┐   ┌───────┐              │
│ │ PyQt5 │   │ PySide │     │ PyQt6 │   │PySide6│              │
│ │(第三方)│   │  (官方)│     │(第三方)│   │ (官方)│              │
│ └───────┘   └───────┘     └───────┘   └───────┘              │
│                                                                  │
│ Python 绑定 ←────────────────→ Python 绑定                      │
│                                                                  │
└──────────────────────────────────────────────────────────────────┘
```

---

## 13. 相关资源

> [!NOTE] 相关文档
> - [[Qt6_learning]] - Qt6 C++ 学习指南
> - [[Qt5_framework_development]] - Qt5 C++ 开发指南
> - [[QSqlite_development]] - 数据库操作指南
> - [[Qt_ROS2_visualization]] - Qt + ROS2 话题/服务/参数可视化

> [!TIP] 官方资源
> - [PyQt6 官方文档](https://www.riverbankcomputing.com/static/Docs/PyQt6/)
> - [PySide6 官方文档](https://doc.qt.io/qtforpython/)
> - [Qt 官方文档](https://doc.qt.io/)

---

> [!NOTE] 更新日志
> - **2026-03-29**: 创建 PyQt6 学习文档
> - **2026-03-29**: 新增 PyQt6 vs C++ Qt6 对比
> - **2026-03-29**: 添加 QSqlite 操作示例
> - **2026-03-29**: 说明 PyQt5 到 PyQt6 迁移变化
> - **2026-03-29**: 添加 PyQt6 vs PySide6 选择指南