cmake_minimum_required(VERSION 3.17)
project(QSqlited_demo LANGUAGES CXX)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_AUTOMOC ON)
SET(CMAKE_AUTORCC ON)
SET(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Sql Qml Quick QuickControls2)

qt_add_executable(QSqlited_demo
    main.cpp
    DatabaseService.cpp
    StudentModel.cpp
)

qt_add_qml_module(QSqlited_demo
    URI QSqlitedDemo
    VERSION 1.0
    QML_FILES
        qml/Main.qml
)

target_link_libraries(QSqlited_demo PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Sql
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
)