#include "MainWindow.h"

#include <QApplication>
#include <QComboBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QHeaderView>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QSpinBox>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVector>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_nameEdit(new QLineEdit(this)),
      m_phoneEdit(new QLineEdit(this)),
      m_cityBox(new QComboBox(this)),
      m_ageSpin(new QSpinBox(this)),
      m_table(new QTableWidget(this)),
      m_addButton(new QPushButton("Add", this)),
      m_updateButton(new QPushButton("Update", this)),
      m_deleteButton(new QPushButton("Delete", this)),
      m_themeBox(new QComboBox(this)) {
    setWindowTitle("Qt6 Widgets UI Design Teaching Demo");

    auto *central = new QWidget(this);
    auto *rootLayout = new QHBoxLayout(central);

    auto *leftPanel = new QVBoxLayout();
    auto *formBox = new QGroupBox("Student Form", this);
    auto *formLayout = new QFormLayout(formBox);

    m_cityBox->addItems({"Beijing", "Shanghai", "Shenzhen", "Hangzhou", "Chengdu", "Guangzhou"});
    m_ageSpin->setRange(1, 130);
    m_ageSpin->setValue(20);

    formLayout->addRow("Name", m_nameEdit);
    formLayout->addRow("Phone", m_phoneEdit);
    formLayout->addRow("City", m_cityBox);
    formLayout->addRow("Age", m_ageSpin);

    auto *buttonRow = new QHBoxLayout();
    buttonRow->addWidget(m_addButton);
    buttonRow->addWidget(m_updateButton);
    buttonRow->addWidget(m_deleteButton);

    auto *themeBox = new QGroupBox("Theme", this);
    auto *themeLayout = new QVBoxLayout(themeBox);
    m_themeBox->addItems({"Light", "Dark", "Sky"});
    themeLayout->addWidget(new QLabel("Choose stylesheet preset", this));
    themeLayout->addWidget(m_themeBox);

    leftPanel->addWidget(formBox);
    leftPanel->addLayout(buttonRow);
    leftPanel->addWidget(themeBox);
    leftPanel->addStretch();

    m_table->setColumnCount(5);
    m_table->setHorizontalHeaderLabels({"ID", "Name", "Phone", "City", "Age"});
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    m_table->setColumnHidden(0, true);
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);

    rootLayout->addLayout(leftPanel, 1);
    rootLayout->addWidget(m_table, 2);

    setCentralWidget(central);

    connect(m_addButton, &QPushButton::clicked, this, &MainWindow::addStudent);
    connect(m_updateButton, &QPushButton::clicked, this, &MainWindow::updateStudent);
    connect(m_deleteButton, &QPushButton::clicked, this, &MainWindow::removeStudent);
    connect(m_table, &QTableWidget::itemSelectionChanged, this, &MainWindow::loadSelectedRow);
    connect(m_themeBox, qOverload<int>(&QComboBox::currentIndexChanged), this, &MainWindow::applyTheme);

    applyTheme(0);

    if (!m_database.initialize()) {
        const QString detail = m_database.lastError();
        QMessageBox::critical(this,
                              "Database Error",
                              "Failed to initialize MySQL database.\n"
                              "Detail: " + (detail.isEmpty() ? QString("Unknown error") : detail) + "\n\n"
                              "Check env vars: DB_HOST, DB_PORT, DB_NAME, DB_USER, DB_PASSWORD, DB_SOCKET.");
        return;
    }

    QString loadError;
    if (!reloadTable(&loadError)) {
        QMessageBox::critical(this, "Database Error", "Failed to load students: " + loadError);
    }
}

void MainWindow::addStudent() {
    QString name;
    QString phone;
    QString city;
    int age = 0;
    if (!readForm(&name, &phone, &city, &age)) {
        return;
    }

    QString error;
    if (!m_database.addStudent(name, phone, city, age, &error)) {
        QMessageBox::warning(this, "Add failed", error);
        return;
    }

    if (!reloadTable(&error)) {
        QMessageBox::warning(this, "Refresh failed", error);
    }
}

void MainWindow::updateStudent() {
    const int id = selectedStudentId();
    if (id <= 0) {
        QMessageBox::information(this, "Tip", "Please select one row first.");
        return;
    }

    QString name;
    QString phone;
    QString city;
    int age = 0;
    if (!readForm(&name, &phone, &city, &age)) {
        return;
    }

    QString error;
    if (!m_database.updateStudent(id, name, phone, city, age, &error)) {
        QMessageBox::warning(this, "Update failed", error);
        return;
    }

    if (!reloadTable(&error)) {
        QMessageBox::warning(this, "Refresh failed", error);
    }
}

void MainWindow::removeStudent() {
    const int id = selectedStudentId();
    if (id <= 0) {
        QMessageBox::information(this, "Tip", "Please select one row first.");
        return;
    }

    QString error;
    if (!m_database.deleteStudent(id, &error)) {
        QMessageBox::warning(this, "Delete failed", error);
        return;
    }

    if (!reloadTable(&error)) {
        QMessageBox::warning(this, "Refresh failed", error);
    }
}

void MainWindow::loadSelectedRow() {
    const int row = m_table->currentRow();
    if (row < 0) {
        return;
    }

    if (m_table->item(row, 1) == nullptr || m_table->item(row, 2) == nullptr ||
        m_table->item(row, 3) == nullptr || m_table->item(row, 4) == nullptr) {
        return;
    }

    m_nameEdit->setText(m_table->item(row, 1)->text());
    m_phoneEdit->setText(m_table->item(row, 2)->text());

    const QString city = m_table->item(row, 3)->text();
    const int cityIndex = m_cityBox->findText(city);
    if (cityIndex >= 0) {
        m_cityBox->setCurrentIndex(cityIndex);
    }

    m_ageSpin->setValue(m_table->item(row, 4)->text().toInt());
}

void MainWindow::applyTheme(int index) {
    QString qss;

    if (index == 1) {
        qss = "QWidget{background:#18212c;color:#eaf1fb;}"
              "QLineEdit,QComboBox,QSpinBox,QTableWidget{background:#243140;color:#f5f8ff;border:1px solid #4f6377;border-radius:4px;padding:4px;}"
              "QPushButton{background:#36618f;color:white;border:none;border-radius:6px;padding:6px 10px;}"
              "QPushButton:hover{background:#4a79ab;}"
              "QHeaderView::section{background:#2b3c50;color:#dbe7f4;padding:6px;border:none;}";
    } else if (index == 2) {
        qss = "QWidget{background:#edf7ff;color:#16324b;}"
              "QLineEdit,QComboBox,QSpinBox,QTableWidget{background:white;border:1px solid #9ec7e5;border-radius:4px;padding:4px;}"
              "QPushButton{background:#2f9fe6;color:white;border:none;border-radius:6px;padding:6px 10px;}"
              "QPushButton:hover{background:#1f87c8;}"
              "QHeaderView::section{background:#d9efff;color:#24526f;padding:6px;border:none;}";
    } else {
        qss = "QPushButton{padding:6px 10px;}";
    }

    qApp->setStyleSheet(qss);
}

bool MainWindow::readForm(QString *name, QString *phone, QString *city, int *age) const {
    if (name == nullptr || phone == nullptr || city == nullptr || age == nullptr) {
        return false;
    }

    *name = m_nameEdit->text().trimmed();
    *phone = m_phoneEdit->text().trimmed();
    *city = m_cityBox->currentText();
    *age = m_ageSpin->value();

    if (name->isEmpty()) {
        QMessageBox::warning(const_cast<MainWindow *>(this), "Validation", "Name cannot be empty.");
        return false;
    }

    return true;
}

void MainWindow::fillTableRow(int row, int id, const QString &name, const QString &phone, const QString &city, int age) {
    m_table->setItem(row, 0, new QTableWidgetItem(QString::number(id)));
    m_table->setItem(row, 1, new QTableWidgetItem(name));
    m_table->setItem(row, 2, new QTableWidgetItem(phone));
    m_table->setItem(row, 3, new QTableWidgetItem(city));
    m_table->setItem(row, 4, new QTableWidgetItem(QString::number(age)));
}

bool MainWindow::reloadTable(QString *errorMessage) {
    const QVector<Student> students = m_database.fetchAll(errorMessage);
    if (errorMessage != nullptr && !errorMessage->isEmpty()) {
        return false;
    }

    m_table->setRowCount(0);
    for (const Student &student : students) {
        const int row = m_table->rowCount();
        m_table->insertRow(row);
        fillTableRow(row, student.id, student.name, student.phone, student.city, student.age);
    }
    return true;
}

int MainWindow::selectedStudentId() const {
    const int row = m_table->currentRow();
    if (row < 0 || m_table->item(row, 0) == nullptr) {
        return -1;
    }
    return m_table->item(row, 0)->text().toInt();
}
