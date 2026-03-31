# Qt + ROS2 Teaching Demo

## What this demo teaches
- How to combine a Qt Widgets UI with a ROS2 node.
- How to move ROS2 messages into Qt UI updates through a QObject bridge.
- How to keep the app runnable even if ROS2 is not installed (simulation mode).

## Build
```bash
cmake -S . -B build
cmake --build build -j
```

## Run
```bash
./build/qt_ros2_demo
```

## ROS2 mode
If ROS2 packages (`rclcpp`, `std_msgs`) are found, this demo will:
- Create a node named `qt_ros2_demo_node`
- Publish to topic `chatter`
- Subscribe to topic `chatter`
- Show messages in the Qt log area

## Simulation mode
If ROS2 is not found, the app still runs and emits one fake message per second.

## Notes
- To force simulation mode: configure with `-DENABLE_ROS2=OFF`.
- To use real ROS2 messages from other nodes, publish to `/chatter`.
