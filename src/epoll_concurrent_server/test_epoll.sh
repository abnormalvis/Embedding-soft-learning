#!/bin/bash

# 编译程序
echo "编译程序..."
gcc -o server server.c
gcc -o client client.c

# 启动服务器
echo "启动服务器..."
./server > server.log 2>&1 &
SERVER_PID=$!

sleep 1

# 运行客户端测试
echo "运行客户端测试..."
./client

# 等待一下让服务器处理
sleep 2

# 杀掉服务器
kill $SERVER_PID 2>/dev/null
wait $SERVER_PID 2>/dev/null

echo "=== 服务器日志 ==="
cat server.log

echo "测试完成"
