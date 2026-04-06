cmake_minimum_required(VERSION 3.16)
project(qt6widget_ui_demo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Sql)

qt_add_executable(qt6widget_ui_demo
    main.cpp
    MainWindow.cpp
    DatabaseService.cpp
)

target_link_libraries(qt6widget_ui_demo PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Sql
)
