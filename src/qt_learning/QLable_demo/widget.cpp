#include "widget.h"
#include "ui_widget.h"
#include "SubWindow.h"
#include <QVBoxLayout>
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    m_label = new QLabel("主窗口标签", this);
    m_button = new QPushButton("传给子窗口", this);
    SubWindow *sub = new SubWindow();

    sub->show();

    layout->addWidget(m_label);
    layout->addWidget(m_button);

    // connect(m_button, &QPushButton::clicked, this, [=](){
    //     emit LabelPointerSignal(m_label);
    // });

    // connect(this, &Widget::LabelPointerSignal, this, &Widget::onLabelPointerReceived);
    // //ui->setupUi(this);

    // 连接：点击按钮 -> 触发主窗口信号 -> 传递给子窗口槽函数
    connect(m_button, &QPushButton::clicked, this, [=]() {
        emit LabelPointerSignal(m_label);
    });

    connect(this, &Widget::LabelPointerSignal, sub, &SubWindow::handleExternalLabel);
}

void Widget::onLabelPointerReceived(QLabel* labelptr)
{
    if(labelptr)
    {
        labelptr->setText("clicked!");
        labelptr->setStyleSheet("background-color: yellow; color: red;");
    }
}
Widget::~Widget()
{
    delete ui;
}
