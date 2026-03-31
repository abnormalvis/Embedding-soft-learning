cmake_minimum_required(VERSION 3.16)
project(qt_quick3d_demo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick QuickControls2)
find_package(Qt6Quick3D QUIET)

qt_add_executable(qt_quick3d_demo
    main.cpp
)

qt_add_qml_module(qt_quick3d_demo
    URI Quick3DLesson
    VERSION 1.0
    QML_FILES
        qml/MainFallback.qml
        qml/MainQuick3D.qml
)

target_link_libraries(qt_quick3d_demo PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
)

if(TARGET Qt6::Quick3D)
    target_compile_definitions(qt_quick3d_demo PRIVATE HAS_QUICK3D=1)
    target_link_libraries(qt_quick3d_demo PRIVATE Qt6::Quick3D)
endif()
