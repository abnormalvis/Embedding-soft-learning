#!/bin/bash
# MySQL 配置脚本 - 设置 root 密码认证

echo "=== MySQL 配置脚本 ==="
echo "此脚本将："
echo "1. 为 root@localhost 设置密码: 123456"
echo "2. 创建 student_info 数据库"
echo ""

# 设置密码
echo "步骤 1: 设置 root 密码..."
sudo mysql -e "ALTER USER 'root'@'localhost' IDENTIFIED WITH mysql_native_password BY '123456';" || {
    echo "错误: 无法设置密码"
    exit 1
}

echo "步骤 2: 刷新权限..."
sudo mysql -e "FLUSH PRIVILEGES;" || {
    echo "错误: 无法刷新权限"
    exit 1
}

echo "步骤 3: 创建数据库..."
mysql -u root -p123456 -e "CREATE DATABASE IF NOT EXISTS student_info;" || {
    echo "错误: 无法创建数据库"
    exit 1
}

echo "步骤 4: 验证连接..."
mysql -u root -p123456 -e "SELECT 'MySQL 配置成功!' AS status;" || {
    echo "错误: 连接验证失败"
    exit 1
}

echo ""
echo "✅ MySQL 配置完成！"
echo "   用户名: root"
echo "   密码: 123456"
echo "   数据库: student_info"
