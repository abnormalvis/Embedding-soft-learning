#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QLabel;
class QPushButton;
class QTextEdit;

class RosBridge;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void onToggleClicked();
    void onStatusChanged(const QString &status);
    void onMessageReceived(const QString &message);

private:
    RosBridge *m_bridge;
    QLabel *m_statusLabel;
    QPushButton *m_toggleButton;
    QTextEdit *m_logEdit;
};

#endif
