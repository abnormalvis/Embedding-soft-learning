cmake_minimum_required(VERSION 3.16)
project(qt_ros2_demo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

option(ENABLE_ROS2 "Enable ROS2 integration if dependencies are available" ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

set(ROS2_AVAILABLE OFF)
if(ENABLE_ROS2)
    find_package(rclcpp QUIET)
    find_package(std_msgs QUIET)
    if(rclcpp_FOUND AND std_msgs_FOUND)
        set(ROS2_AVAILABLE ON)
    endif()
endif()

qt_add_executable(qt_ros2_demo
    main.cpp
    MainWindow.cpp
    RosBridge.cpp
)

target_link_libraries(qt_ros2_demo PRIVATE
    Qt6::Core
    Qt6::Widgets
)

if(ROS2_AVAILABLE)
    target_compile_definitions(qt_ros2_demo PRIVATE HAS_ROS2=1)
    target_link_libraries(qt_ros2_demo PRIVATE rclcpp::rclcpp)
    if(TARGET std_msgs::std_msgs__rosidl_typesupport_cpp)
        target_link_libraries(qt_ros2_demo PRIVATE std_msgs::std_msgs__rosidl_typesupport_cpp)
    elseif(TARGET std_msgs::std_msgs__rosidl_typesupport_fastrtps_cpp)
        target_link_libraries(qt_ros2_demo PRIVATE std_msgs::std_msgs__rosidl_typesupport_fastrtps_cpp)
    elseif(TARGET std_msgs::std_msgs)
        target_link_libraries(qt_ros2_demo PRIVATE std_msgs::std_msgs)
    endif()
else()
    message(STATUS "ROS2 dependencies not found, building in simulation mode.")
endif()
