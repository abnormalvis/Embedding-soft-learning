---
title: Qt + ROS2 话题/服务/参数可视化
description: 使用 Qt 框架开发 ROS2 机器人数据可视化界面，包括话题、服务、参数监控
tags: [Qt, ROS2, Robot, Visualization, Topic, Service, Parameter]
created: 2026-03-30
updated: 2026-03-30
source: 机器人开发实践
related: [[Qt6_learning]], [[Qt5_framework_development]], [[PyQt6_learning]]
---

# Qt + ROS2 话题/服务/参数可视化

> [!NOTE] 概述
> ROS2（Robot Operating System 2）是机器人开发的标准中间件框架，而 Qt 是强大的跨平台 GUI 开发工具。本文介绍如何结合 Qt 与 ROS2，实现机器人数据的实时可视化，包括话题（Topic）、服务（Service）、参数（Parameter）的监控与操作界面。

---

## 1. ROS2 核心概念回顾

### 1.1 ROS2 通信机制

| 通信类型 | 说明 | 特点 | Qt 对应 |
|----------|------|------|---------|
| **Topic（话题）** | 发布/订阅模型，异步通信 | 多对多，单向 | `QLabel` / `QCustomPlot` 显示数据 |
| **Service（服务）** | 请求/响应模型，同步通信 | 一对一，双向 | `QPushButton` 触发 + 结果显示 |
| **Action（动作）** | 长期任务，带反馈 | 可取消，有目标/结果 | 进度条 + 状态显示 |
| **Parameter（参数）** | 节点配置数据 | 运行时可读写 | `QSpinBox` / `QDoubleSpinBox` 编辑 |

### 1.2 话题/服务/参数关系图

```
┌─────────────────────────────────────────────────────────────────┐
│                        ROS2 通信架构                            │
├─────────────────────────────────────────────────────────────────┤
│                                                                  │
│   [Publisher] ──Topic──→ [Subscriber]                           │
│       │                      │                                 │
│       ↓                      ↓                                 │
│   发布数据               接收数据                              │
│                                                                  │
│   [Client] ──Service──→ [Server]                                │
│       │                      │                                 │
│       ↓                      ↓                                 │
│   发起请求              处理响应                                │
│                                                                  │
│   [Node] ←──Parameter──→ [参数服务器]                           │
│       │                      │                                 │
│       ↓                      ↓                                 │
│   读写配置              存储配置                                │
│                                                                  │
└─────────────────────────────────────────────────────────────────┘
```

---

## 2. Qt + ROS2 技术方案

### 2.1 方案对比

| 方案 | 语言 | 库/包 | 适用场景 |
|------|------|-------|----------|
| **C++ Qt + rclcpp** | C++ | `rclcpp`, `rclcpp_components` | 高性能，实时要求 |
| **PyQt6 + rclpy** | Python | `rclpy`, `PyQt6` | 快速开发，原型验证 |
| **PySide6 + rclpy** | Python | `rclpy`, `PySide6` | 官方推荐 Python 方案 |
| **QML + QtQuick** | QML | `rclcpp`, `rclpy` | 现代化 UI，触屏设备 |

> [!TIP] 选择建议
> - **桌面监控应用**：PyQt6/PySide6 + rclpy
> - **嵌入式/实时**：C++ Qt + rclcpp
> - **现代化 UI**：QML + ROS2

### 2.2 架构设计

```
┌─────────────────────────────────────────────────────────────────┐
│                      Qt + ROS2 应用架构                         │
├─────────────────────────────────────────────────────────────────┤
│                                                                  │
│   ┌─────────────────────────────────────────────────────────┐  │
│   │                      Qt GUI 层                          │  │
│   │  ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────┐    │  │
│   │  │ Topic   │  │ Service │  │Param    │  │ 3D View │    │  │
│   │  │ Monitor │  │ Caller  │  │Editor   │  │ (rviz)  │    │  │
│   │  └────┬────┘  └────┬────┘  └────┬────┘  └────┬────┘    │  │
│   └───────┼────────────┼────────────┼────────────┼──────────┘  │
│           │            │            │            │             │
│   ┌───────▼────────────▼────────────▼────────────▼──────────┐ │
│   │                    ROS2 接口层                           │  │
│   │  ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────┐     │  │
│   │  │rosidl   │  │rosidl   │  │rclcpp/  │  │tf2      │     │  │
│   │  │subscriber│  │client  │  │rclpy    │  │listener │     │  │
│   │  └─────────┘  └─────────┘  └─────────┘  └─────────┘     │  │
│   └─────────────────────────────────────────────────────────┘  │
│                              │                                   │
│   ┌──────────────────────────▼──────────────────────────────┐  │
│   │                    ROS2 核心层                           │  │
│   │     DDS (Data Distribution Service)                      │  │
│   └─────────────────────────────────────────────────────────┘  │
│                                                                  │
└─────────────────────────────────────────────────────────────────┘
```

---

## 3. PySide6/PyQt6 + rclpy 实现

### 3.1 项目结构

```
ros2_qt_app/
├── package.xml
├── setup.py
├── setup.cfg
├── resource/
│   └── ros2_qt_app
├── ros2_qt_app/
│   ├── __init__.py
│   ├── main.py              # 入口
│   ├── main_window.py      # 主窗口
│   ├── topic_monitor.py    # 话题监控
│   ├── service_caller.py    # 服务调用
│   ├── param_editor.py     # 参数编辑
│   └── widgets/
│       ├── chart_widget.py  # 图表组件
│       └── image_widget.py # 图像显示
└── CMakeLists.txt
```

### 3.2 main.py - 应用入口

```python
#!/usr/bin/env python3
import sys
from PySide6.QtWidgets import QApplication
from ros2_qt_app.main_window import MainWindow


def main(args=None):
    """ROS2 初始化"""
    import rclpy
    from rclpy.executors import SingleThreadedExecutor

    rclpy.init(args=args)

    # 创建 Qt 应用
    app = QApplication(sys.argv)
    app.setApplicationName("ROS2 Monitor")

    # 创建主窗口
    window = MainWindow()

    # ROS2 单线程执行器（处理回调）
    executor = SingleThreadedExecutor()
    executor.add_node(window.node)

    # 定时处理 ROS2 回调（避免阻塞 Qt 主线程）
    timer = Timer(0.1, lambda: executor.spin_once(0))
    timer.start()

    window.show()
    exit_code = app.exec()

    # 清理
    timer.stop()
    window.destroy_node()
    rclpy.shutdown()

    return exit_code


class Timer:
    """简单的 QTimer 替代"""
    def __init__(self, interval, callback):
        import threading
        self.callback = callback
        self.timer = threading.Timer(interval, self._run)
        self.running = False

    def start(self):
        self.running = True
        self._run()

    def _run(self):
        if self.running:
            self.callback()
            self.timer = threading.Timer(0.1, self._run)
            self.timer.start()

    def stop(self):
        self.running = False
        if self.timer.is_alive():
            self.timer.cancel()


if __name__ == "__main__":
    main()
```

### 3.3 MainWindow - 主窗口框架

```python
from PySide6.QtWidgets import QMainWindow, QTabWidget, QWidget, QVBoxLayout
from PySide6.QtCore import QTimer
from PySide6.QtGui import QAction
import rclpy
from rclpy.node import Node

from ros2_qt_app.topic_monitor import TopicMonitor
from ros2_qt_app.service_caller import ServiceCaller
from ros2_qt_app.param_editor import ParameterEditor


class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("ROS2 Qt Monitor")
        self.setMinimumSize(800, 600)

        # 初始化 ROS2 节点
        self.node = rclpy.node.Node('qt_ros2_monitor')

        # 创建菜单栏
        self._create_menu()

        # 创建中心标签页
        self._create_tabs()

        # 定时更新 UI
        self.timer = QTimer()
        self.timer.timeout.connect(self._spin_ros)
        self.timer.start(100)  # 10Hz

    def _create_menu(self):
        menubar = self.menuBar()

        # 文件菜单
        file_menu = menubar.addMenu("文件")
        refresh_action = QAction("刷新话题列表", self)
        refresh_action.setShortcut("F5")
        refresh_action.triggered.connect(self._refresh_topics)
        file_menu.addAction(refresh_action)
        file_menu.addSeparator()
        file_menu.addAction("退出", self.close)

        # 视图菜单
        view_menu = menubar.addMenu("视图")
        view_menu.addAction("话题监控")
        view_menu.addAction("服务调用")
        view_menu.addAction("参数编辑")

    def _create_tabs(self):
        central_widget = QWidget()
        self.setCentralWidget(central_widget)
        layout = QVBoxLayout(central_widget)

        self.tabs = QTabWidget()

        # 添加功能标签页
        self.tabs.addTab(TopicMonitor(self.node), "话题监控")
        self.tabs.addTab(ServiceCaller(self.node), "服务调用")
        self.tabs.addTab(ParameterEditor(self.node), "参数编辑")

        layout.addWidget(self.tabs)

    def _refresh_topics(self):
        # 触发话题刷新
        pass

    def _spin_ros(self):
        # 在主线程中处理 ROS2 回调
        rclpy.spin_once(self.node, timeout_sec=0)

    def closeEvent(self, event):
        self.timer.stop()
        super().closeEvent(event)
```

---

## 4. 话题监控实现

### 4.1 TopicMonitor 类

```python
from PySide6.QtWidgets import (
    QWidget, QVBoxLayout, QHBoxLayout, QTableWidget,
    QTableWidgetItem, QComboBox, QLabel, QPushButton, QGroupBox
)
from PySide6.QtCore import QTimer
import rclpy
from rclpy.node import Node


class TopicMonitor(QWidget):
    """话题监控器 - 显示话题数据并可视化"""

    def __init__(self, node: Node):
        super().__init__()
        self.node = node

        # 话题订阅者字典
        self.subscribers = {}
        self.topic_data = {}

        self._init_ui()

        # 创建话题列表更新定时器
        self.update_timer = QTimer()
        self.update_timer.timeout.connect(self._update_topics_list)
        self.update_timer.start(2000)  # 每 2 秒更新

        # 数据更新定时器
        self.data_timer = QTimer()
        self.data_timer.timeout.connect(self._update_display)
        self.data_timer.start(100)  # 10Hz

    def _init_ui(self):
        layout = QVBoxLayout(self)

        # 话题选择区域
        topic_select = QGroupBox("话题选择")
        topic_layout = QHBoxLayout(topic_select)

        self.topic_combo = QComboBox()
        self.topic_combo.currentTextChanged.connect(self._on_topic_selected)
        topic_layout.addWidget(QLabel("选择话题:"))
        topic_layout.addWidget(self.topic_combo)

        self.subscribe_btn = QPushButton("订阅")
        self.subscribe_btn.clicked.connect(self._subscribe_topic)
        topic_layout.addWidget(self.subscribe_btn)

        self.unsubscribe_btn = QPushButton("取消订阅")
        self.unsubscribe_btn.clicked.connect(self._unsubscribe_topic)
        topic_layout.addWidget(self.unsubscribe_btn)

        layout.addWidget(topic_select)

        # 话题列表
        list_group = QGroupBox("已订阅话题")
        list_layout = QVBoxLayout(list_group)

        self.topic_table = QTableWidget()
        self.topic_table.setColumnCount(4)
        self.topic_table.setHorizontalHeaderLabels(["话题名", "类型", "频率(Hz)", "最新数据"])
        self.topic_table.setMinimumHeight(150)
        list_layout.addWidget(self.topic_table)

        layout.addWidget(list_group)

        # 数据详情区域
        detail_group = QGroupBox("数据详情")
        detail_layout = QVBoxLayout(detail_group)

        self.data_text = QLabel("选择话题查看数据")
        self.data_text.setWordWrap(True)
        detail_layout.addWidget(self.data_text)

        layout.addWidget(detail_group)

    def _update_topics_list(self):
        """获取当前可用话题列表"""
        topic_names = self.node.get_topic_names_and_types()
        current = self.topic_combo.currentText()

        self.topic_combo.clear()
        for name, types in topic_names:
            # 过滤系统话题
            if not name.startswith('/'):
                continue
            display = f"{name} [{types[0]}]"
            self.topic_combo.addItem(display)

    def _on_topic_selected(self, text):
        if text:
            self.topic_name = text.split(' [')[0]

    def _subscribe_topic(self):
        """订阅选中的话题"""
        if not hasattr(self, 'topic_name'):
            return

        if self.topic_name in self.subscribers:
            return

        # 根据话题类型创建订阅者
        try:
            if '/image' in self.topic_name:
                self._create_image_subscriber(self.topic_name)
            elif '/cmd_vel' in self.topic_name:
                self._create_twist_subscriber(self.topic_name)
            else:
                self._create_generic_subscriber(self.topic_name)
        except Exception as e:
            print(f"订阅失败: {e}")

    def _create_generic_subscriber(self, topic_name: str):
        """创建通用话题订阅者"""

        from std_msgs.msg import String

        def callback(msg):
            import threading
            self.topic_data[topic_name] = {
                'timestamp': self.node.get_clock().now().to_msg(),
                'data': str(msg.data)
            }

        self.subscribers[topic_name] = self.node.create_subscription(
            String,
            topic_name,
            callback,
            10
        )
        self._update_topic_table()

    def _create_twist_subscriber(self, topic_name: str):
        """创建 Twist 类型订阅者"""
        from geometry_msgs.msg import Twist

        def callback(msg: Twist):
            self.topic_data[topic_name] = {
                'linear': f"x={msg.linear.x:.2f} y={msg.linear.y:.2f} z={msg.linear.z:.2f}",
                'angular': f"x={msg.angular.x:.2f} y={msg.angular.y:.2f} z={msg.angular.z:.2f}"
            }

        self.subscribers[topic_name] = self.node.create_subscription(
            Twist,
            topic_name,
            callback,
            10
        )
        self._update_topic_table()

    def _unsubscribe_topic(self):
        """取消订阅"""
        if hasattr(self, 'topic_name') and self.topic_name in self.subscribers:
            self.node.destroy_subscription(self.subscribers[self.topic_name])
            del self.subscribers[self.topic_name]
            if self.topic_name in self.topic_data:
                del self.topic_data[self.topic_name]
            self._update_topic_table()

    def _update_topic_table(self):
        """更新话题列表表格"""
        self.topic_table.setRowCount(len(self.subscribers))
        for row, (topic, sub) in enumerate(self.subscribers.items()):
            self.topic_table.setItem(row, 0, QTableWidgetItem(topic))
            self.topic_table.setItem(row, 1, QTableWidgetItem("std_msg"))
            self.topic_table.setItem(row, 2, QTableWidgetItem("--"))
            self.topic_table.setItem(row, 3, QTableWidgetItem(
                str(self.topic_data.get(topic, {}).get('data', ''))[:50]))

    def _update_display(self):
        """更新数据显示"""
        if hasattr(self, 'topic_name') and self.topic_name in self.topic_data:
            data = self.topic_data[self.topic_name]
            self.data_text.setText(f"话题: {self.topic_name}\n数据: {data}")
```

---

## 5. 服务调用实现

### 5.1 ServiceCaller 类

```python
from PySide6.QtWidgets import (
    QWidget, QVBoxLayout, QHBoxLayout, QTableWidget,
    QTableWidgetItem, QComboBox, QLabel, QPushButton, QGroupBox,
    QTextEdit, QLineEdit, QSpinBox, QDoubleSpinBox, QFormLayout
)
from PySide6.QtCore import QTimer
import rclpy
from rclpy.node import Node


class ServiceCaller(QWidget):
    """服务调用器 - 调用 ROS2 服务"""

    def __init__(self, node: Node):
        super().__init__()
        self.node = node
        self.service_clients = {}
        self.pending_requests = []

        self._init_ui()

        # 更新服务列表定时器
        self.timer = QTimer()
        self.timer.timeout.connect(self._update_service_list)
        self.timer.start(2000)

    def _init_ui(self):
        layout = QVBoxLayout(self)

        # 服务选择区域
        service_select = QGroupBox("服务调用")
        service_layout = QVBoxLayout(service_select)

        # 服务下拉框
        service_row = QHBoxLayout()
        self.service_combo = QComboBox()
        self.service_combo.currentTextChanged.connect(self._on_service_selected)
        service_row.addWidget(QLabel("选择服务:"))
        service_row.addWidget(self.service_combo)
        service_layout.addLayout(service_row)

        # 请求参数区域
        self.request_group = QGroupBox("请求参数")
        self.request_layout = QFormLayout()
        self.request_group.setLayout(self.request_layout)
        service_layout.addWidget(self.request_group)

        # 调用按钮
        btn_row = QHBoxLayout()
        self.call_btn = QPushButton("调用服务")
        self.call_btn.clicked.connect(self._call_service)
        btn_row.addWidget(self.call_btn)

        self.clear_btn = QPushButton("清空")
        self.clear_btn.clicked.connect(self._clear_request)
        btn_row.addWidget(self.clear_btn)

        service_layout.addLayout(btn_row)

        layout.addWidget(service_select)

        # 响应显示区域
        response_group = QGroupBox("服务响应")
        response_layout = QVBoxLayout(response_group)

        self.response_text = QTextEdit()
        self.response_text.setReadOnly(True)
        self.response_text.setMaximumHeight(200)
        response_layout.addWidget(self.response_text)

        layout.addWidget(response_group)

        # 调用历史
        history_group = QGroupBox("调用历史")
        history_layout = QVBoxLayout(history_group)

        self.history_table = QTableWidget()
        self.history_table.setColumnCount(4)
        self.history_table.setHorizontalHeaderLabels(["时间", "服务", "请求", "响应"])
        self.history_table.setMinimumHeight(150)
        history_layout.addWidget(self.history_table)

        layout.addWidget(history_group)

    def _update_service_list(self):
        """更新服务列表"""
        service_names = self.node.get_service_names_and_types()
        current = self.service_combo.currentText()

        self.service_combo.clear()
        for name, types in service_names:
            if name.startswith('/'):
                display = f"{name} [{types[0]}]"
                self.service_combo.addItem(display)

    def _on_service_selected(self, text):
        """服务选择变化"""
        if not text:
            return

        service_name = text.split(' [')[0]

        # 清空参数布局
        while self.request_layout.count():
            child = self.request_layout.takeAt(0)
            if child.widget():
                child.widget().deleteLater()

        # 根据服务类型添加参数输入框
        if '/add_two_ints' in service_name:
            self._add_param('a', QSpinBox(), '第一个整数')
            self._add_param('b', QSpinBox(), '第二个整数')
        elif '/spawn' in service_name:
            self._add_param('name', QLineEdit(), '实体名称')
            self._add_param('x', QDoubleSpinBox(), 'X 坐标')
            self._add_param('y', QDoubleSpinBox(), 'Y 坐标')
        else:
            # 默认添加通用参数输入
            self._add_param('data', QLineEdit(), '请求数据')

    def _add_param(self, name, widget, label):
        """添加参数输入控件"""
        if isinstance(widget, QSpinBox):
            widget.setRange(-10000, 10000)
        elif isinstance(widget, QDoubleSpinBox):
            widget.setRange(-1000, 1000)
            widget.setDecimals(2)

        self.request_layout.addRow(label, widget)
        widget.setProperty('param_name', name)

    def _call_service(self):
        """调用服务"""
        if not self.service_combo.currentText():
            return

        service_name = self.service_combo.currentText().split(' [')[0]

        # 构建请求
        request = self._build_request(service_name)

        # 调用服务
        future = self.service_clients[service_name].call_async(request)
        self.pending_requests.append({
            'service': service_name,
            'future': future,
            'request': request
        })

    def _build_request(self, service_name: str):
        """根据服务类型构建请求"""
        if '/add_two_ints' in service_name:
            from example_interfaces.srv import AddTwoInts
            a = self._get_param_value('a')
            b = self._get_param_value('b')
            request = AddTwoInts.Request()
            request.a = int(a) if a else 0
            request.b = int(b) if b else 0
            return request

        # 默认返回空请求
        return None

    def _get_param_value(self, name: str):
        """获取参数值"""
        for i in range(self.request_layout.count()):
            widget = self.request_layout.itemAt(i).widget()
            if widget and getattr(widget, 'property', lambda x: None)('param_name') == name:
                if isinstance(widget, QSpinBox):
                    return widget.value()
                elif isinstance(widget, QDoubleSpinBox):
                    return widget.value()
                elif isinstance(widget, QLineEdit):
                    return widget.text()
        return None

    def _clear_request(self):
        """清空请求参数"""
        for i in range(self.request_layout.count()):
            widget = self.request_layout.itemAt(i).widget()
            if widget:
                if isinstance(widget, QSpinBox):
                    widget.setValue(0)
                elif isinstance(widget, QLineEdit):
                    widget.clear()

    def _update_response(self):
        """更新服务响应状态"""
        completed = []
        for item in self.pending_requests:
            if item['future'].done():
                completed.append(item)

        for item in completed:
            self.pending_requests.remove(item)

            if item['future'].result():
                response = item['future'].result()
                self.response_text.append(f"成功: {response}")
            else:
                self.response_text.append(f"失败: {item['future'].exception()}")

        QTimer.singleShot(100, self._update_response)
```

---

## 6. 参数编辑实现

### 6.1 ParameterEditor 类

```python
from PySide6.QtWidgets import (
    QWidget, QVBoxLayout, QHBoxLayout, QTableWidget,
    QTableWidgetItem, QComboBox, QLabel, QPushButton, QGroupBox,
    QLineEdit, QSpinBox, QDoubleSpinBox, QFormLayout, QCheckBox
)
from PySide6.QtCore import QTimer
import rclpy
from rclpy.node import Node


class ParameterEditor(QWidget):
    """参数编辑器 - 读取和修改 ROS2 参数"""

    def __init__(self, node: Node):
        super().__init__()
        self.node = node
        self.param_watchers = {}

        self._init_ui()

        # 参数更新定时器
        self.timer = QTimer()
        self.timer.timeout.connect(self._refresh_parameters)
        self.timer.start(1000)

    def _init_ui(self):
        layout = QVBoxLayout(self)

        # 参数同步控制
        sync_group = QGroupBox("参数同步")
        sync_layout = QHBoxLayout(sync_group)

        self.node_combo = QComboBox()
        self.node_combo.currentTextChanged.connect(self._on_node_selected)
        sync_layout.addWidget(QLabel("选择节点:"))
        sync_layout.addWidget(self.node_combo)

        refresh_btn = QPushButton("刷新参数")
        refresh_btn.clicked.connect(self._refresh_parameters)
        sync_layout.addWidget(refresh_btn)

        layout.addWidget(sync_group)

        # 参数列表表格
        param_group = QGroupBox("参数列表")
        param_layout = QVBoxLayout(param_group)

        self.param_table = QTableWidget()
        self.param_table.setColumnCount(5)
        self.param_table.setHorizontalHeaderLabels([
            "参数名", "类型", "当前值", "新值", "操作"
        ])
        self.param_table.setMinimumHeight(300)
        param_layout.addWidget(self.param_table)

        layout.addWidget(param_group)

        # 批量操作
        batch_layout = QHBoxLayout()
        set_all_btn = QPushButton("应用所有更改")
        set_all_btn.clicked.connect(self._apply_all_changes)
        batch_layout.addWidget(set_all_btn)

        reset_btn = QPushButton("重置")
        reset_btn.clicked.connect(self._reset_all)
        batch_layout.addWidget(reset_btn)

        layout.addLayout(batch_layout)

    def _on_node_selected(self, node_name):
        """节点选择变化"""
        self._load_parameters(node_name)

    def _refresh_parameters(self):
        """刷新节点和参数列表"""
        # 获取所有节点
        node_names = self.node.get_node_names()

        current = self.node_combo.currentText()
        self.node_combo.clear()

        for name in node_names:
            self.node_combo.addItem(name)

        # 保持选择
        if current and current in node_names:
            self.node_combo.setCurrentText(current)

    def _load_parameters(self, node_name: str):
        """加载指定节点的参数"""
        # 创建参数客户端
        from rclpy.parameter import Parameter

        try:
            param_node = rclpy.create_node(f'param_client_{node_name}')
            self.param_client = param_node._create_parameter_client(node_name)

            # 获取参数
            self.param_client.wait_for_service(timeout_sec=2.0)

            # 获取参数列表
            future = self.param_client.get_parameters_async(['*'])
            # 处理 future...

        except Exception as e:
            print(f"加载参数失败: {e}")

    def _create_param_row(self, name: str, param_type: str, value):
        """创建参数行"""
        row = self.param_table.rowCount()
        self.param_table.insertRow(row)

        # 参数名
        self.param_table.setItem(row, 0, QTableWidgetItem(name))

        # 类型
        type_item = QTableWidgetItem(param_type)
        self.param_table.setItem(row, 1, type_item)

        # 当前值
        value_item = QTableWidgetItem(str(value))
        self.param_table.setItem(row, 2, value_item)

        # 新值输入
        new_value_widget = self._create_value_widget(param_type, value)
        self.param_table.setCellWidget(row, 3, new_value_widget)

        # 操作按钮
        apply_btn = QPushButton("应用")
        apply_btn.clicked.connect(lambda: self._apply_change(row))
        self.param_table.setCellWidget(row, 4, apply_btn)

    def _create_value_widget(self, param_type: str, value):
        """根据参数类型创建编辑控件"""
        if param_type == 'integer':
            widget = QSpinBox()
            widget.setRange(-1000000, 1000000)
            widget.setValue(int(value))
        elif param_type == 'double':
            widget = QDoubleSpinBox()
            widget.setRange(-1000000.0, 1000000.0)
            widget.setValue(float(value))
            widget.setDecimals(4)
        elif param_type == 'bool':
            widget = QCheckBox()
            widget.setChecked(bool(value))
        else:  # string or other
            widget = QLineEdit()
            widget.setText(str(value))

        widget.setProperty('param_name', name)
        return widget

    def _apply_change(self, row: int):
        """应用单个参数更改"""
        param_name = self.param_table.item(row, 0).text()
        new_value_widget = self.param_table.cellWidget(row, 3)
        new_value = new_value_widget.value() if hasattr(new_value_widget, 'value') else new_value_widget.text()

        # 调用参数设置 API
        self._set_parameter(param_name, new_value)

        # 更新显示
        self.param_table.item(row, 2).setText(str(new_value))

    def _set_parameter(self, name: str, value):
        """设置参数"""
        try:
            self.param_client.set_parameters([
                rclpy.parameter.Parameter(name, rclpy.parameter.Parameter.Type.PARAMETER_NOT_SET, value).to_parameter_msg()
            ])
        except Exception as e:
            print(f"设置参数失败: {e}")

    def _apply_all_changes(self):
        """应用所有更改"""
        for row in range(self.param_table.rowCount()):
            self._apply_change(row)

    def _reset_all(self):
        """重置所有更改"""
        self._load_parameters(self.node_combo.currentText())
```

---

## 7. Qt Creator + ROS2 项目配置

### 7.1 CMakeLists.txt 配置

```cmake
cmake_minimum_required(VERSION 3.16)
project(ros2_qt_app)

# C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ROS2 依赖
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)

# Qt 依赖
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

# 源文件
set(SOURCES
    src/main.cpp
    src/main_window.cpp
    src/topic_monitor.cpp
    src/service_caller.cpp
    src/param_editor.cpp
)

# 头文件
set(HEADERS
    include/ros2_qt_app/main_window.hpp
    include/ros2_qt_app/topic_monitor.hpp
    include/ros2_qt_app/service_caller.hpp
    include/ros2_qt_app/param_editor.hpp
)

# 创建可执行文件
qt_add_executable(ros2_qt_app
    ${SOURCES}
    ${HEADERS}
)

# 链接库
target_link_libraries(ros2_qt_app PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    rclcpp::rclcpp
    std_msgs::std_msgs
    geometry_msgs::geometry_msgs
)

# 安装
install(TARGETS ros2_qt_app DESTINATION lib/${PROJECT_NAME})

# 插件注册
ament_export_dependencies(ament_cmake)
ament_export_dependencies(rclcpp)
```

### 7.2 package.xml 配置

```xml
<?xml version="1.0"?>
<?xml-model href="http://download.ros.org/schema/package_format3.xsd" schematypens="http://www.w3.org/2001/XMLSchema"?>
<package format="3">
    <name>ros2_qt_app</name>
    <version>0.1.0</version>
    <description>Qt + ROS2 Visualization Application</description>
    <maintainer email="user@example.com">User</maintainer>
    <license>Apache-2.0</license>

    <buildtool_depend>ament_cmake</buildtool_depend>
    <buildtool_depend>qt6-cmake</buildtool_depend>

    <depend>rclcpp</depend>
    <depend>std_msgs</depend>
    <depend>geometry_msgs</depend>
    <depend>sensor_msgs</depend>

    <depend>qt6base</depend>
    <depend>qt6core</depend>
    <depend>qt6widgets</depend>

    <test_depend>ament_lint_auto</test_depend>
    <test_depend>ament_lint_common</test_depend>

    <export>
        <build_type>ament_cmake</build_type>
    </export>
</package>
```

---

## 8. 实时数据可视化组件

### 8.1 数值曲线图表

```python
from PySide6.QtWidgets import QWidget, QVBoxLayout
from PySide6.QtCharts import QChart, QChartView, QLineSeries
from PySide6.QtCore import QTimer
from PySide6.QtGui import QPainter


class ChartWidget(QWidget):
    """实时数值曲线图表"""

    def __init__(self, title: str = "数据曲线", max_points: int = 100):
        super().__init__()
        self.max_points = max_points
        self.data_buffer = []

        # 创建图表
        self.chart = QChart()
        self.chart.setTitle(title)

        # 创建数据系列
        self.series = QLineSeries()
        self.chart.addSeries(self.series)

        # 创建图表视图
        self.chart_view = QChartView(self.chart)
        self.chart_view.setRenderHint(QPainter.RenderHint.Antialiasing)

        # 布局
        layout = QVBoxLayout(self)
        layout.addWidget(self.chart_view)

    def add_point(self, value: float):
        """添加数据点"""
        self.data_buffer.append(value)

        # 保持最大点数
        if len(self.data_buffer) > self.max_points:
            self.data_buffer.pop(0)

        # 更新图表
        self.series.clear()
        for i, v in enumerate(self.data_buffer):
            self.series.append(i, v)

    def set_data(self, values: list):
        """设置数据数组"""
        self.data_buffer = values[-self.max_points:]
        self.series.clear()
        for i, v in enumerate(self.data_buffer):
            self.series.append(i, v)
```

### 8.2 图像显示组件

```python
from PySide6.QtWidgets import QLabel
from PySide6.QtGui import QImage, QPixmap
from PySide6.QtCore import Qt


class ImageDisplayWidget(QLabel):
    """ROS2 图像话题显示组件"""

    def __init__(self):
        super().__init__()
        self.setAlignment(Qt.AlignmentFlag.AlignCenter)
        self.setStyleSheet("background-color: black;")
        self.setText("等待图像...")

    def update_image(self, data: bytes, width: int, height: int, encoding: str = "rgb8"):
        """更新显示图像"""
        if encoding == "rgb8":
            # RGB8 格式
            img = QImage(data, width, height, QImage.Format.Format_RGB888)
        elif encoding == "bgr8":
            # BGR 转 RGB
            import numpy as np
            arr = np.frombuffer(data, dtype=np.uint8).reshape(height, width, 3)
            arr = arr[:, :, ::-1]  # BGR to RGB
            img = QImage(arr.data, width, height, QImage.Format.Format_RGB888)
        elif encoding == "mono8":
            img = QImage(data, width, height, QImage.Format.Format_Grayscale8)
        else:
            self.setText(f"不支持格式: {encoding}")
            return

        # 缩放以适应窗口
        pixmap = QPixmap.fromImage(img)
        scaled_pixmap = pixmap.scaled(
            self.size(), Qt.AspectRatioMode.KeepAspectRatio,
            Qt.TransformationMode.SmoothTransformation
        )
        self.setPixmap(scaled_pixmap)
```

---

## 9. 常见应用场景

### 9.1 机器人监控面板

```
┌─────────────────────────────────────────────────────────────────┐
│                     机器人监控面板                                │
├─────────────────────────────────────────────────────────────────┤
│                                                                  │
│  ┌─────────────────┐  ┌─────────────────┐  ┌─────────────────┐ │
│  │   电池状态      │  │   连接状态      │  │   CPU 使用率    │ │
│  │     85%         │  │    ● 在线       │  │     23%         │ │
│  └─────────────────┘  └─────────────────┘  └─────────────────┘ │
│                                                                  │
│  ┌──────────────────────────────────────────────────────────┐   │
│  │                      速度曲线                              │   │
│  │     ─────────────────────────────────────────────        │   │
│  │                          ↑                               │   │
│  │                        /                                 │   │
│  │                      /                                   │   │
│  │                    /                                     │   │
│  │                  /                                       │   │
│  │              ─/                                         │   │
│  │                                                             │   │
│  └──────────────────────────────────────────────────────────┘   │
│                                                                  │
│  ┌────────────────────────┐  ┌────────────────────────────────┐ │
│  │     话题列表           │  │         详细数据               │ │
│  │  /cmd_vel    →       │  │  linear.x: 0.5                 │ │
│  │  /odom       →       │  │  linear.y: 0.0                 │ │
│  │  /scan       →       │  │  linear.z: 0.0                 │ │
│  └────────────────────────┘  └────────────────────────────────┘ │
│                                                                  │
└─────────────────────────────────────────────────────────────────┘
```

### 9.2 导航调试界面

- 实时显示机器人位置 (odom → tf)
- 显示激光雷达扫描数据
- 显示路径规划结果
- 控制机器人运动 (`/cmd_vel`)

---

## 10. 学习路径与进阶

```
┌─────────────────────────────────────────────────────────────┐
│               Qt + ROS2 可视化学习路径                        │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│   1. ROS2 基础 ──→ 2. Qt 信号与槽 ──→ 3. 基本 UI 控件        │
│                                                             │
│           ↓                                                 │
│                                                             │
│   4. 话题订阅 ──→ 5. 实时数据可视化 ──→ 6. 图表绘制          │
│                                                             │
│           ↓                                                 │
│                                                             │
│   7. 服务调用 ──→ 8. 参数管理 ──→ 9. 多线程处理              │
│                                                             │
│           ↓                                                 │
│                                                             │
│   10. RViz 插件开发 ──→ 11. 自定义面板 ──→ 12. 发布应用       │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

---

## 11. 相关资源

> [!NOTE] 相关文档
> - [[Qt6_learning]] - Qt6 C++ 学习指南
> - [[Qt5_framework_development]] - Qt5 框架开发指南
> - [[PyQt6_learning]] - PyQt6 Python 绑定详解
> - [[QSqlite_development]] - 数据库操作（用于存储历史数据）

> [!TIP] 官方资源
> - [ROS2 官方文档](https://docs.ros.org/en/humble/)
> - [rclpy 文档](https://docs.ros.org/en/humble/p/rclpy/)
> - [Qt6 官方文档](https://doc.qt.io/qt-6/)
> - [PySide6 文档](https://doc.qt.io/qtforpython/)

---

> [!NOTE] 更新日志
> - **2026-03-30**: 创建 Qt + ROS2 可视化文档
> - **2026-03-30**: 新增话题订阅与监控实现
> - **2026-03-30**: 新增服务调用器实现
> - **2026-03-30**: 新增参数编辑器实现
> - **2026-03-30**: 添加 Qt6 + ROS2 CMake 配置