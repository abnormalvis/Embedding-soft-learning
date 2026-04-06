#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <qlabel.h>
#include <QPushButton>
#include <QStackedLayout>
#include <QTreeWidget>
#include <QLineEdit>
#include <QVBoxLayout>

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

signals:
    void LabelPointerSignal(QLabel* labelptr);

private slots:
    void onLabelPointerReceived(QLabel* labelptr);
private:
    QLabel *m_label;
    QPushButton *m_button;
    // ROS2 参数可视化与编辑相关
    QStackedLayout *m_stackedLayout;
    QWidget *m_pageList;
    QWidget *m_pageEdit;
    QTreeWidget *m_paramTree;
    QLineEdit *m_valInput;
    QPushButton *m_saveBtn;
};
#endif // WIDGET_H
