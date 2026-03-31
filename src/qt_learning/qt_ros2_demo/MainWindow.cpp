#include "MainWindow.h"

#include <QDateTime>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>

#include "RosBridge.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      m_bridge(new RosBridge(this)),
      m_statusLabel(new QLabel("Status: idle", this)),
      m_toggleButton(new QPushButton("Start Bridge", this)),
      m_logEdit(new QTextEdit(this)) {
    setWindowTitle("Qt + ROS2 Teaching Demo");

    m_logEdit->setReadOnly(true);

    auto *container = new QWidget(this);
    auto *layout = new QVBoxLayout(container);

    auto *headerLayout = new QHBoxLayout();
    headerLayout->addWidget(m_statusLabel);
    headerLayout->addStretch();
    headerLayout->addWidget(m_toggleButton);

    layout->addLayout(headerLayout);
    layout->addWidget(m_logEdit);

    setCentralWidget(container);

    connect(m_toggleButton, &QPushButton::clicked, this, &MainWindow::onToggleClicked);
    connect(m_bridge, &RosBridge::statusChanged, this, &MainWindow::onStatusChanged);
    connect(m_bridge, &RosBridge::messageReceived, this, &MainWindow::onMessageReceived);

    m_logEdit->append("Teaching notes:");
    m_logEdit->append("1) Click Start Bridge.");
    m_logEdit->append("2) In ROS2 mode, this node publishes/subscribes on topic /chatter.");
    m_logEdit->append("3) Without ROS2, simulated messages are emitted every second.");
}

void MainWindow::onToggleClicked() {
    if (m_bridge->isRunning()) {
        m_bridge->stop();
        m_toggleButton->setText("Start Bridge");
    } else {
        m_bridge->start();
        m_toggleButton->setText("Stop Bridge");
    }
}

void MainWindow::onStatusChanged(const QString &status) {
    m_statusLabel->setText("Status: " + status);
    m_logEdit->append("[" + QDateTime::currentDateTime().toString("hh:mm:ss") + "] " + status);
}

void MainWindow::onMessageReceived(const QString &message) {
    m_logEdit->append("[msg] " + message);
}
