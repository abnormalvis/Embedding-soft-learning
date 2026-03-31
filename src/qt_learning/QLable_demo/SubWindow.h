#ifndef SUBWINDOW_H
#define SUBWINDOW_H
#include <QWidget>
#include <QLabel>
class SubWindow : public QWidget
{
    Q_OBJECT
public slots:
    void handleExternalLabel(QLabel* label)
    {
        if (label)
        {
            label->setText("子窗口修改了主窗口的标签！");
        }
    }
};
#endif
