#include "widget.h"
#include "SubWindow.h"
#include <QVBoxLayout>
#include <QStackedLayout>
#include <QTreeWidget>
#include <QLineEdit>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    // --- ROS2 参数可视化与编辑 ---
    m_stackedLayout = new QStackedLayout();

    // --- 第一页：参数列表页 ---
    m_pageList = new QWidget();
    QVBoxLayout *listLayout = new QVBoxLayout(m_pageList);
    m_paramTree = new QTreeWidget();
    m_paramTree->setHeaderLabels({"参数名", "类型", "当前值"});
    listLayout->addWidget(new QLabel("ROS 2 参数列表"));
    listLayout->addWidget(m_paramTree);

    // --- 第二页：参数修改页 ---
    m_pageEdit = new QWidget();
    QVBoxLayout *editLayout = new QVBoxLayout(m_pageEdit);
    editLayout->addWidget(new QLabel("编辑参数值"));
    m_valInput = new QLineEdit();
    m_saveBtn = new QPushButton("更新到 ROS 2");
    editLayout->addWidget(m_valInput);
    editLayout->addWidget(m_saveBtn);
    editLayout->addStretch();

    // --- 把两页加入堆栈 ---
    m_stackedLayout->addWidget(m_pageList); // Index 0
    m_stackedLayout->addWidget(m_pageEdit); // Index 1

    // --- 将堆栈布局放入主容器 ---
    mainLayout->addLayout(m_stackedLayout);

    // TODO: 参数获取与填充 m_paramTree
    // TODO: 信号槽连接（如双击参数进入编辑页、保存后返回列表页等）
}

void Widget::onLabelPointerReceived(QLabel* labelptr)
{
    if(labelptr)
    {
        labelptr->setText("clicked!");
        labelptr->setStyleSheet("background-color: yellow; color: red;");
    }
}
Widget::~Widget() {}
