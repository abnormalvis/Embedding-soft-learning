#ifndef ROSBRIDGE_H
#define ROSBRIDGE_H

#include <QObject>
#include <QString>

class QTimer;

#ifdef HAS_ROS2
#include <memory>
#include <thread>

#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/string.hpp>
#endif

class RosBridge : public QObject {
    Q_OBJECT

public:
    explicit RosBridge(QObject *parent = nullptr);
    ~RosBridge() override;

    Q_INVOKABLE void start();
    Q_INVOKABLE void stop();
    bool isRunning() const;

signals:
    void statusChanged(const QString &status);
    void messageReceived(const QString &message);

private:
#ifdef HAS_ROS2
    void runRosSpin();

    bool m_running = false;
    std::shared_ptr<rclcpp::Node> m_node;
    rclcpp::Subscription<std_msgs::msg::String>::SharedPtr m_subscriber;
    rclcpp::Publisher<std_msgs::msg::String>::SharedPtr m_publisher;
    rclcpp::TimerBase::SharedPtr m_publishTimer;
    std::thread m_spinThread;
#else
    QTimer *m_simTimer = nullptr;
#endif
};

#endif
