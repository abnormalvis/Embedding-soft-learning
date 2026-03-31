#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <qlabel.h>
#include <QPushButton>
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
    Ui::Widget *ui;
    QLabel *m_label;
    QPushButton *m_button;
    // std::unique_ptr<QLabel> m_label;
    // std::unique_ptr<QPushButton> m_button;
};
#endif // WIDGET_H
