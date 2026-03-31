#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QComboBox;
class QLineEdit;
class QPushButton;
class QSpinBox;
class QTableWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void addStudent();
    void updateStudent();
    void removeStudent();
    void loadSelectedRow();
    void applyTheme(int index);

private:
    bool readForm(QString *name, QString *phone, QString *city, int *age) const;
    void fillTableRow(int row, const QString &name, const QString &phone, const QString &city, int age);

    QLineEdit *m_nameEdit;
    QLineEdit *m_phoneEdit;
    QComboBox *m_cityBox;
    QSpinBox *m_ageSpin;
    QTableWidget *m_table;
    QPushButton *m_addButton;
    QPushButton *m_updateButton;
    QPushButton *m_deleteButton;
    QComboBox *m_themeBox;
};

#endif
