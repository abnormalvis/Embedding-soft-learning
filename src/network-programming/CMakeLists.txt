cmake_minimum_required(VERSION 3.12)
project(NetworkProgrammingDemo VERSION 1.0.0 LANGUAGES C)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# 设置源文件目录
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

# 查找源文件
file(GLOB SRC_FILES "${SRC_DIR}/*.c")

# 包含头文件目录
include_directories(${INCLUDE_DIR})

# 创建服务器可执行文件
add_executable(server_demo ${SRC_DIR}/server_demo.c)

# 创建客户端可执行文件
add_executable(client_demo ${SRC_DIR}/client_demo.c)

# 链接必要的库 (Linux 下网络库不需要额外链接，socket 库在 Windows 上需要)
