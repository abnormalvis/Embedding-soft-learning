#include "RosBridge.h"

#include <QDateTime>
#include <QTimer>

RosBridge::RosBridge(QObject *parent)
    : QObject(parent) {
}

RosBridge::~RosBridge() {
    stop();
}

void RosBridge::start() {
#ifdef HAS_ROS2
    if (m_running) {
        return;
    }

    if (!rclcpp::ok()) {
        int argc = 0;
        char **argv = nullptr;
        rclcpp::init(argc, argv);
    }

    m_node = std::make_shared<rclcpp::Node>("qt_ros2_demo_node");
    m_subscriber = m_node->create_subscription<std_msgs::msg::String>(
        "chatter", 10,
        [this](const std_msgs::msg::String::SharedPtr msg) {
            emit messageReceived(QString::fromStdString(msg->data));
        });

    m_publisher = m_node->create_publisher<std_msgs::msg::String>("chatter", 10);
    m_publishTimer = m_node->create_wall_timer(
        std::chrono::seconds(1),
        [this]() {
            std_msgs::msg::String msg;
            msg.data = "hello from qt_ros2_demo at " + std::to_string(m_node->now().seconds());
            m_publisher->publish(msg);
        });

    m_running = true;
    m_spinThread = std::thread(&RosBridge::runRosSpin, this);

    emit statusChanged("running with ROS2");
#else
    if (m_simTimer != nullptr) {
        return;
    }

    m_simTimer = new QTimer(this);
    connect(m_simTimer, &QTimer::timeout, this, [this]() {
        const QString text = "simulated message at " + QDateTime::currentDateTime().toString("hh:mm:ss");
        emit messageReceived(text);
    });
    m_simTimer->start(1000);

    emit statusChanged("running in simulation mode (ROS2 not found)");
#endif
}

void RosBridge::stop() {
#ifdef HAS_ROS2
    if (!m_running) {
        return;
    }

    m_running = false;
    m_publishTimer.reset();

    if (m_spinThread.joinable()) {
        m_spinThread.join();
    }

    m_publisher.reset();
    m_subscriber.reset();
    m_node.reset();

    emit statusChanged("stopped");
#else
    if (m_simTimer == nullptr) {
        return;
    }

    m_simTimer->stop();
    m_simTimer->deleteLater();
    m_simTimer = nullptr;
    emit statusChanged("stopped");
#endif
}

bool RosBridge::isRunning() const {
#ifdef HAS_ROS2
    return m_running;
#else
    return m_simTimer != nullptr;
#endif
}

#ifdef HAS_ROS2
void RosBridge::runRosSpin() {
    rclcpp::executors::SingleThreadedExecutor executor;
    executor.add_node(m_node);

    while (m_running && rclcpp::ok()) {
        executor.spin_some();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    executor.remove_node(m_node);
}
#endif
