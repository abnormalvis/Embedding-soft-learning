#include <mysql.h>

#include <stdio.h>

/*
 * demo_02: 建库建表与初始化数据模板
 * 目标：
 * 1) 学会 DDL (CREATE TABLE)
 * 2) 学会 DML (INSERT)
 */
int main(void) {
    MYSQL *conn = mysql_init(NULL);
    const char *create_table_sql =
        "CREATE TABLE IF NOT EXISTS students ("
        "id INT PRIMARY KEY AUTO_INCREMENT,"
        "name VARCHAR(64) NOT NULL,"
        "phone_number VARCHAR(32) NOT NULL,"
        "city VARCHAR(64) NOT NULL,"
        "age INT NOT NULL"
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4";

    if (conn == NULL) {
        fprintf(stderr, "mysql_init failed\n");
        return 1;
    }

    if (mysql_real_connect(conn, "127.0.0.1", "root", "123456", "student_info", 3306, NULL, 0) == NULL) {
        fprintf(stderr, "connect failed: %s\n", mysql_error(conn));
        mysql_close(conn);
        return 1;
    }

    if (mysql_query(conn, create_table_sql) != 0) {
        fprintf(stderr, "create table failed: %s\n", mysql_error(conn));
        mysql_close(conn);
        return 1;
    }

    printf("[demo_02] create table success\n");

    /* TODO:
     * 写 2-3 条 INSERT 语句，向 students 写入测试数据
     * 例如: amy/bobo/cici
     */

    /* TODO(进阶):
     * 1) 使用事务 BEGIN/COMMIT 包裹批量插入
     * 2) 发生错误时执行 ROLLBACK
     */

    mysql_close(conn);
    return 0;
}
