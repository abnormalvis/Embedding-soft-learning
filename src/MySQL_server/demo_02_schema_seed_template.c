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
  const char *create_table_sql = "CREATE TABLE IF NOT EXISTS students ("
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

  if (mysql_real_connect(conn, "localhost", "root", "88143208", "student_info", 0,
                         NULL, 0) == NULL) {
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
  const char *insert_sql = 
      "INSERT INTO students (name, phone_number, city, age) "
      "VALUES('amy', '1234567890', 'Beijing', 20), "
      "      ('bobo', '9876543210', 'Shanghai', 22), "
      "      ('cici', '5555555555', 'Guangzhou', 21)";
  
  if (mysql_query(conn, insert_sql) != 0) {
    fprintf(stderr, "insert data failed: %s\n", mysql_error(conn));
    mysql_close(conn);
    return 1;
  }
  
  printf("[demo_02] insert data success: 3 students added\n");
  
  /* TODO(进阶):
   * 1) 使用事务 BEGIN/COMMIT 包裹批量插入
   * 2) 发生错误时执行 ROLLBACK
   */
  // 实现事务处理的完整示例
  if (mysql_query(conn, "BEGIN") != 0) {
    fprintf(stderr, "BEGIN transaction failed: %s\n", mysql_error(conn));
    mysql_close(conn);
    return 1;
  }
  
  const char *transaction_insert_sql = 
      "INSERT INTO students (name, phone_number, city, age) "
      "VALUES('david', '1112223333', 'Shenzhen', 23)";
  
  if (mysql_query(conn, transaction_insert_sql) != 0) {
    fprintf(stderr, "Insert in transaction failed: %s\nRolling back...", mysql_error(conn));
    if (mysql_query(conn, "ROLLBACK") != 0) {
      fprintf(stderr, "ROLLBACK failed: %s\n", mysql_error(conn));
    } else {
      printf("ROLLBACK successful\n");
    }
    mysql_close(conn);
    return 1;
  }
  
  if (mysql_query(conn, "COMMIT") != 0) {
    fprintf(stderr, "COMMIT failed: %s\n", mysql_error(conn));
    mysql_close(conn);
    return 1;
  }
  
  printf("[demo_02] transaction insert success: 1 student added with transaction\n");
  
  mysql_close(conn);
  return 0;
}