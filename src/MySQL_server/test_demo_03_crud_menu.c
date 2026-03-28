#include <mysql.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

/*
 * 单元测试：针对 demo_03_crud_menu_template.c 的功能测试
 */

// 定义要使用的数据库名和连接参数
#define DB_HOST "127.0.0.1"
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
    
    // 创建测试表
    const char *create_table_sql = "CREATE TABLE IF NOT EXISTS students ("
                                   "id INT PRIMARY KEY AUTO_INCREMENT,"
                                   "name VARCHAR(64) NOT NULL,"
                                   "phone_number VARCHAR(32) NOT NULL,"
                                   "city VARCHAR(64) NOT NULL,"
                                   "age INT NOT NULL"
                                   ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4";

    if (mysql_query(test_conn, create_table_sql) != 0) {
        fprintf(stderr, "create table failed: %s\n", mysql_error(test_conn));
        mysql_close(test_conn);
        return 0;
    }
    
    // 清空表并插入测试数据
    mysql_query(test_conn, "DELETE FROM students");
    
    const char *insert_sql = 
        "INSERT INTO students (name, phone_number, city, age) "
        "VALUES('test_user1', '1234567890', 'Beijing', 20), "
        "      ('test_user2', '9876543210', 'Shanghai', 22), "
        "      ('test_user3', '5555555555', 'Guangzhou', 21)";
    
    if (mysql_query(test_conn, insert_sql) != 0) {
        fprintf(stderr, "insert test data failed: %s\n", mysql_error(test_conn));
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

// 测试查询功能
int test_query_all() {
    const char *query_sql = "SELECT id, name, phone_number, city, age FROM students";
    
    if (mysql_query(test_conn, query_sql) != 0) {
        fprintf(stderr, "query failed: %s\n", mysql_error(test_conn));
        return 0;
    }
    
    MYSQL_RES *result = mysql_store_result(test_conn);
    if (result == NULL) {
        fprintf(stderr, "store result failed: %s\n", mysql_error(test_conn));
        return 0;
    }
    
    int num_rows = mysql_num_rows(result);
    mysql_free_result(result);
    
    // 应该有3条记录
    if (num_rows != 3) {
        fprintf(stderr, "expected 3 records, got %d\n", num_rows);
        return 0;
    }
    
    printf("test_query_all: PASSED\n");
    return 1;
}

// 测试更新功能
int test_update_city_by_name() {
    // 先检查更新前的值
    const char *check_sql = "SELECT city FROM students WHERE name='test_user1'";
    
    if (mysql_query(test_conn, check_sql) != 0) {
        fprintf(stderr, "check query failed: %s\n", mysql_error(test_conn));
        return 0;
    }
    
    MYSQL_RES *result = mysql_store_result(test_conn);
    if (result == NULL) {
        fprintf(stderr, "store result failed: %s\n", mysql_error(test_conn));
        return 0;
    }
    
    MYSQL_ROW row = mysql_fetch_row(result);
    if (row == NULL || strcmp(row[0], "Beijing") != 0) {
        fprintf(stderr, "initial city is not Beijing\n");
        mysql_free_result(result);
        return 0;
    }
    mysql_free_result(result);
    
    // 执行更新
    char update_sql[512];
    char escaped_city[128];
    mysql_real_escape_string(test_conn, escaped_city, "Shenzhen", strlen("Shenzhen"));
    snprintf(update_sql, sizeof(update_sql), 
             "UPDATE students SET city='%s' WHERE name='test_user1'", escaped_city);
    
    if (mysql_query(test_conn, update_sql) != 0) {
        fprintf(stderr, "update failed: %s\n", mysql_error(test_conn));
        return 0;
    }
    
    // 验证更新结果
    if (mysql_query(test_conn, check_sql) != 0) {
        fprintf(stderr, "check query after update failed: %s\n", mysql_error(test_conn));
        return 0;
    }
    
    result = mysql_store_result(test_conn);
    if (result == NULL) {
        fprintf(stderr, "store result after update failed: %s\n", mysql_error(test_conn));
        return 0;
    }
    
    row = mysql_fetch_row(result);
    if (row == NULL || strcmp(row[0], "Shenzhen") != 0) {
        fprintf(stderr, "update did not work, city is still %s\n", row ? row[0] : "NULL");
        mysql_free_result(result);
        return 0;
    }
    mysql_free_result(result);
    
    printf("test_update_city_by_name: PASSED\n");
    return 1;
}

// 测试删除功能
int test_delete_by_name() {
    // 先检查删除前的总记录数
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
    int count_before = atoi(row[0]);
    mysql_free_result(result);
    
    // 如果不是恰好3条记录，则测试环境可能有问题
    if (count_before != 3) {
        fprintf(stderr, "unexpected record count before delete: %d\n", count_before);
        return 0;
    }
    
    // 执行删除
    const char *delete_sql = "DELETE FROM students WHERE name='test_user2'";
    
    if (mysql_query(test_conn, delete_sql) != 0) {
        fprintf(stderr, "delete failed: %s\n", mysql_error(test_conn));
        return 0;
    }
    
    // 验证删除结果
    if (mysql_query(test_conn, "SELECT COUNT(*) FROM students") != 0) {
        fprintf(stderr, "count query after delete failed: %s\n", mysql_error(test_conn));
        return 0;
    }
    
    result = mysql_store_result(test_conn);
    if (result == NULL) {
        fprintf(stderr, "store result after delete failed: %s\n", mysql_error(test_conn));
        return 0;
    }
    
    row = mysql_fetch_row(result);
    int count_after = atoi(row[0]);
    mysql_free_result(result);
    
    if (count_after != 2) {
        fprintf(stderr, "expected 2 records after delete, got %d\n", count_after);
        return 0;
    }
    
    printf("test_delete_by_name: PASSED\n");
    return 1;
}

// 测试删除不存在的记录
int test_delete_nonexistent_record() {
    // 尝试删除一个不存在的记录
    const char *delete_sql = "DELETE FROM students WHERE name='nonexistent_user'";
    
    if (mysql_query(test_conn, delete_sql) != 0) {
        fprintf(stderr, "delete nonexistent record failed: %s\n", mysql_error(test_conn));
        return 0;
    }
    
    // 影响的行数应该是0
    int affected_rows = mysql_affected_rows(test_conn);
    if (affected_rows != 0) {
        fprintf(stderr, "expected 0 affected rows for nonexistent record, got %d\n", affected_rows);
        return 0;
    }
    
    printf("test_delete_nonexistent_record: PASSED\n");
    return 1;
}

int main(void) {
    printf("Running unit tests for demo_03_crud_menu_template...\n");
    
    if (!setup_test_env()) {
        fprintf(stderr, "Setup test environment failed\n");
        return 1;
    }
    
    int all_tests_passed = 1;
    
    // 运行各个测试
    if (!test_query_all()) {
        all_tests_passed = 0;
    }
    
    if (!test_update_city_by_name()) {
        all_tests_passed = 0;
    }
    
    if (!test_delete_by_name()) {
        all_tests_passed = 0;
    }
    
    if (!test_delete_nonexistent_record()) {
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