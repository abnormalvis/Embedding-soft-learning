#!/bin/bash

echo "======================================"
echo "网络编程综合练习 - 自动化测试脚本"
echo "======================================"
echo ""

# 进入项目目录
cd "$(dirname "$0")"

# 检查文件是否存在
echo "1. 检查测试文件..."
if [ ! -f "./files/test.txt" ]; then
    echo "错误：test.txt 不存在!"
    exit 1
fi
if [ ! -f "./files/test_image.bmp" ]; then
    echo "错误：test_image.bmp 不存在!"
    exit 1
fi
if [ ! -f "./files/test_video.mp4" ]; then
    echo "错误：test_video.mp4 不存在!"
    exit 1
fi
echo "✓ 所有测试文件存在"
echo ""

# 显示文件大小
echo "2. 测试文件大小:"
ls -lh files/
echo ""

# 启动服务器
echo "3. 启动服务器..."
./build/server_demo &
SERVER_PID=$!
sleep 2
echo "服务器已启动 (PID: $SERVER_PID)"
echo ""

# 创建客户端输入
echo "4. 测试客户端连接和命令..."
{
    echo "1"
    sleep 1
    echo "2"
    sleep 1
    echo "3"
    sleep 1
    echo "q"
} | ./build/client_demo &
CLIENT_PID=$!

# 等待客户端完成
sleep 8

# 清理进程
kill $SERVER_PID 2>/dev/null
kill $CLIENT_PID 2>/dev/null
wait 2>/dev/null

echo ""
echo "5. 检查下载的文件..."
if [ -d "./downloads" ]; then
    ls -lh downloads/
    echo ""
    
    # 验证文件是否下载成功
    if [ -f "./downloads/downloaded_file.txt" ]; then
        echo "✓ TXT 文件下载成功"
    else
        echo "✗ TXT 文件下载失败"
    fi
    
    if [ -f "./downloads/downloaded_image.bmp" ]; then
        echo "✓ 图片文件下载成功"
    else
        echo "✗ 图片文件下载失败"
    fi
    
    if [ -f "./downloads/downloaded_video.mp4" ]; then
        echo "✓ 视频文件下载成功"
    else
        echo "✗ 视频文件下载失败"
    fi
else
    echo "✗ 下载目录不存在"
fi

echo ""
echo "======================================"
echo "测试完成!"
echo "======================================"
