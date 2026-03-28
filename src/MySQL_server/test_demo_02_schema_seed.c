#include <mysql.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

/*
 * 单元测试：针对 demo_02_schema_seed_template.c 的功能测试
 */

// 定义要使用的数据库名和连接参数
#define DB_HOST "localhost"
#define DB_USER "root"
#define DB_PASS "123456"
#define DB_NAME "student_info_test"

// 全局变量存储数据库连接
MYSQL *test_conn;

// 测试前准备工作（建立连接、创建测试数据库）
int setup_test_env() {
    test_conn = mysql_init(NULL);
    
    if (test_conn == NULL) {
        fprintf(stderr, "mysql_init failed\n");
        return 0;
    }

    if (mysql_real_connect(test_conn, DB_HOST, DB_USER, DB_PASS, NULL, 0, NULL, 0) == NULL) {
        fprintf(stderr, "connect failed: %s\n", mysql_error(test_conn));
        mysql_close(test_conn);
        return 0;
    }
    
    // 创建测试数据库
    char *create_db_sql = "CREATE DATABASE IF NOT EXISTS " DB_NAME;
    if (mysql_query(test_conn, create_db_sql) != 0) {
        fprintf(stderr, "create database failed: %s\n", mysql_error(test_conn));
        mysql_close(test_conn);
        return 0;
    }
    
    // 切换到测试数据库
    if (mysql_select_db(test_conn, DB_NAME) != 0) {
        fprintf(stderr, "select db failed: %s\n", mysql_error(test_conn));
        mysql_close(test_conn);
        return 0;
    }
    
    return 1;
}

// 清理工作（删除测试数据表、关闭连接）
void teardown_test_env() {
    if (test_conn) {
        // 删除测试表
        char *drop_table_sql = "DROP TABLE IF EXISTS students";
        mysql_query(test_conn, drop_table_sql);
        
        // 删除测试数据库
        char *drop_db_sql = "DROP DATABASE IF EXISTS " DB_NAME;
        mysql_query(test_conn, drop_db_sql);
        
        mysql_close(test_conn);
    }
}

// 测试表创建功能
int test_create_table() {
    const char *create_table_sql = "CREATE TABLE IF NOT EXISTS students ("
                                   "id INT PRIMARY KEY AUTO_INCREMENT,"
                                   "name VARCHAR(64) NOT NULL,"
                                   "phone_number VARCHAR(32) NOT NULL,"
                                   "city VARCHAR(64) NOT NULL,"
                                   "age INT NOT NULL"
                                   ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4";

    if (mysql_query(test_conn, create_table_sql) != 0) {
        fprintf(stderr, "create table failed: %s\n", mysql_error(test_conn));
        return 0;
    }

    // 验证表是否创建成功
    if (mysql_query(test_conn, "DESCRIBE students") != 0) {
        fprintf(stderr, "verify table failed: %s\n", mysql_error(test_conn));
        return 0;
    }
    
    MYSQL_RES *result = mysql_store_result(test_conn);
    if (result == NULL) {
        fprintf(stderr, "store result failed: %s\n", mysql_error(test_conn));
        return 0;
    }
    
    int num_fields = mysql_num_fields(result);
    mysql_free_result(result);
    
    // 表应该有5个字段
    if (num_fields != 5) {
        fprintf(stderr, "expected 5 fields, got %d\n", num_fields);
        return 0;
    }
    
    printf("test_create_table: PASSED\n");
    return 1;
}

// 测试数据插入功能
int test_insert_data() {
    // 确保表是空的开始测试
    mysql_query(test_conn, "DELETE FROM students");
    
    const char *insert_sql = 
        "INSERT INTO students (name, phone_number, city, age) "
        "VALUES('amy', '1234567890', 'Beijing', 20), "
        "      ('bobo', '9876543210', 'Shanghai', 22), "
        "      ('cici', '5555555555', 'Guangzhou', 21)";
    
    if (mysql_query(test_conn, insert_sql) != 0) {
        fprintf(stderr, "insert data failed: %s\n", mysql_error(test_conn));
        return 0;
    }
    
    // 验证数据是否正确插入
    if (mysql_query(test_conn, "SELECT COUNT(*) FROM students") != 0) {
        fprintf(stderr, "count query failed: %s\n", mysql_error(test_conn));
        return 0;
    }
    
    MYSQL_RES *result = mysql_store_result(test_conn);
    if (result == NULL) {
        fprintf(stderr, "store result failed: %s\n", mysql_error(test_conn));
        return 0;
    }
    
    MYSQL_ROW row = mysql_fetch_row(result);
    int count = atoi(row[0]);
    mysql_free_result(result);
    
    if (count != 3) {
        fprintf(stderr, "expected 3 records, got %d\n", count);
        return 0;
    }
    
    printf("test_insert_data: PASSED\n");
    return 1;
}

// 测试数据内容正确性
int test_data_content() {
    const char *select_sql = "SELECT name, phone_number, city, age FROM students ORDER BY id";
    
    if (mysql_query(test_conn, select_sql) != 0) {
        fprintf(stderr, "select query failed: %s\n", mysql_error(test_conn));
        return 0;
    }
    
    MYSQL_RES *result = mysql_store_result(test_conn);
    if (result == NULL) {
        fprintf(stderr, "store result failed: %s\n", mysql_error(test_conn));
        return 0;
    }
    
    int row_count = 0;
    MYSQL_ROW row;
    const char *expected_data[][4] = {
        {"amy", "1234567890", "Beijing", "20"},
        {"bobo", "9876543210", "Shanghai", "22"},
        {"cici", "5555555555", "Guangzhou", "21"}
    };
    
    while ((row = mysql_fetch_row(result)) != NULL && row_count < 3) {
        for (int i = 0; i < 4; i++) {
            if (strcmp(row[i], expected_data[row_count][i]) != 0) {
                fprintf(stderr, "Data mismatch at row %d, col %d. Expected: %s, Got: %s\n", 
                        row_count, i, expected_data[row_count][i], row[i]);
                mysql_free_result(result);
                return 0;
            }
        }
        row_count++;
    }
    
    mysql_free_result(result);
    
    if (row_count != 3) {
        fprintf(stderr, "expected 3 rows, got %d\n", row_count);
        return 0;
    }
    
    printf("test_data_content: PASSED\n");
    return 1;
}

// 测试事务功能（仅在修复原代码后）
int test_transaction() {
    // 首先清理之前的数据
    mysql_query(test_conn, "DELETE FROM students WHERE name IN ('test_user1', 'test_user2')");
    
    // 开始事务
    if (mysql_query(test_conn, "BEGIN") != 0) {
        fprintf(stderr, "begin transaction failed: %s\n", mysql_error(test_conn));
        return 0;
    }
    
    // 插入测试数据
    const char *insert_sql = 
        "INSERT INTO students (name, phone_number, city, age) "
        "VALUES('test_user1', '1111111111', 'TestCity', 25)";
    
    if (mysql_query(test_conn, insert_sql) != 0) {
        fprintf(stderr, "insert in transaction failed: %s\n", mysql_error(test_conn));
        mysql_query(test_conn, "ROLLBACK");  // 回滚事务
        return 0;
    }
    
    // 在事务中查询，应能看到刚插入的数据
    if (mysql_query(test_conn, "SELECT COUNT(*) FROM students WHERE name='test_user1'") != 0) {
        fprintf(stderr, "select in transaction failed: %s\n", mysql_error(test_conn));
        mysql_query(test_conn, "ROLLBACK");
        return 0;
    }
    
    MYSQL_RES *result = mysql_store_result(test_conn);
    MYSQL_ROW row = mysql_fetch_row(result);
    int count = atoi(row[0]);
    mysql_free_result(result);
    
    if (count != 1) {
        fprintf(stderr, "expected 1 record in transaction, got %d\n", count);
        mysql_query(test_conn, "ROLLBACK");
        return 0;
    }
    
    // 提交事务
    if (mysql_query(test_conn, "COMMIT") != 0) {
        fprintf(stderr, "commit transaction failed: %s\n", mysql_error(test_conn));
        return 0;
    }
    
    // 验证提交后的数据
    if (mysql_query(test_conn, "SELECT COUNT(*) FROM students WHERE name='test_user1'") != 0) {
        fprintf(stderr, "verify commit failed: %s\n", mysql_error(test_conn));
        return 0;
    }
    
    result = mysql_store_result(test_conn);
    row = mysql_fetch_row(result);
    count = atoi(row[0]);
    mysql_free_result(result);
    
    if (count != 1) {
        fprintf(stderr, "expected 1 committed record, got %d\n", count);
        return 0;
    }
    
    printf("test_transaction: PASSED\n");
    return 1;
}

int main(void) {
    printf("Running unit tests for demo_02_schema_seed_template...\n");
    
    if (!setup_test_env()) {
        fprintf(stderr, "Setup test environment failed\n");
        return 1;
    }
    
    int all_tests_passed = 1;
    
    // 运行各个测试
    if (!test_create_table()) {
        all_tests_passed = 0;
    }
    
    if (!test_insert_data()) {
        all_tests_passed = 0;
    }
    
    if (!test_data_content()) {
        all_tests_passed = 0;
    }
    
    if (!test_transaction()) {
        all_tests_passed = 0;
    }
    
    teardown_test_env();
    
    if (all_tests_passed) {
        printf("ALL TESTS PASSED\n");
        return 0;
    } else {
        printf("SOME TESTS FAILED\n");
        return 1;
    }
}