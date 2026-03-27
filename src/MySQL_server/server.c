#include <mysql.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUT_BUF_SIZE 256

typedef struct Student {
    int id;
    char name[64];
    char phone_number[32];
    char city[64];
    int age;
} Student;

static void print_menu(void);
static int connect_mysql(
    MYSQL** out_conn,
    const char* host,
    const char* user,
    const char* password,
    const char* db,
    unsigned int port
);
static int ensure_schema(MYSQL* conn);
static int run_student_system(MYSQL* conn);

static int handle_insert_student(MYSQL* conn);
static int handle_find_student(MYSQL* conn);
static int handle_update_student_city(MYSQL* conn);
static int handle_delete_student(MYSQL* conn);
static int handle_show_all_students(MYSQL* conn);

int main(int argc, char** argv) {
    MYSQL* conn = NULL;

    /*
     * 参数约定：
     * argv[1] host (默认 127.0.0.1)
     * argv[2] user (默认 root)
     * argv[3] password (默认 123456)
     * argv[4] db (默认 student_info)
     * argv[5] port (默认 3306)
     */
    const char* host = (argc > 1) ? argv[1] : "127.0.0.1";
    const char* user = (argc > 2) ? argv[2] : "root";
    const char* password = (argc > 3) ? argv[3] : "123456";
    const char* db = (argc > 4) ? argv[4] : "student_info";
    unsigned int port = (argc > 5) ? (unsigned int)atoi(argv[5]) : 3306;

    if (connect_mysql(&conn, host, user, password, db, port) != 0) {
        fprintf(stderr, "[ERROR] MySQL 连接失败\n");
        return 1;
    }

    if (ensure_schema(conn) != 0) {
        fprintf(stderr, "[ERROR] 初始化数据库结构失败\n");
        mysql_close(conn);
        return 1;
    }

    if (run_student_system(conn) != 0) {
        fprintf(stderr, "[WARN] 业务循环异常退出\n");
    }

    mysql_close(conn);
    printf("Bye.\n");
    return 0;
}

static int connect_mysql(
    MYSQL** out_conn,
    const char* host,
    const char* user,
    const char* password,
    const char* db,
    unsigned int port
) {
    MYSQL* conn = mysql_init(NULL);
    if (conn == NULL) {
        fprintf(stderr, "mysql_init failed\n");
        return -1;
    }

    /* TODO(练习1): 试着改成 mysql_options 设置字符集和自动重连 */
    if (mysql_real_connect(conn, host, user, password, db, port, NULL, 0) == NULL) {
        fprintf(stderr, "mysql_real_connect failed: %s\n", mysql_error(conn));
        mysql_close(conn);
        return -1;
    }

    printf("Connection success!\n");
    *out_conn = conn;
    return 0;
}

static int ensure_schema(MYSQL* conn) {
    const char* create_table_sql =
        "CREATE TABLE IF NOT EXISTS students ("
        "id INT PRIMARY KEY AUTO_INCREMENT,"
        "name VARCHAR(64) NOT NULL,"
        "phone_number VARCHAR(32) NOT NULL,"
        "city VARCHAR(64) NOT NULL,"
        "age INT NOT NULL"
        ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4";

    if (mysql_query(conn, create_table_sql) != 0) {
        fprintf(stderr, "create table failed: %s\n", mysql_error(conn));
        return -1;
    }

    return 0;
}

static int run_student_system(MYSQL* conn) {
    char input[INPUT_BUF_SIZE];

    while (1) {
        int choice = -1;

        print_menu();
        printf("请输入功能编号: ");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            return -1;
        }
        choice = atoi(input);

        switch (choice) {
            case 1:
                handle_insert_student(conn);
                break;
            case 2:
                handle_find_student(conn);
                break;
            case 3:
                handle_update_student_city(conn);
                break;
            case 4:
                handle_delete_student(conn);
                break;
            case 5:
                handle_show_all_students(conn);
                break;
            case 0:
                return 0;
            default:
                printf("无效输入，请重试。\n");
                break;
        }
    }
}

static int handle_insert_student(MYSQL* conn) {
    (void)conn;
    /*
     * TODO(练习2): 实现插入逻辑，推荐步骤：
     * 1. 从 stdin 读取 name/phone_number/city/age
     * 2. 使用 snprintf 拼接 INSERT SQL
     * 3. 调用 mysql_query 执行
     * 4. 打印 mysql_affected_rows(conn)
     *
     * 进阶：改成 mysql_stmt_prepare + mysql_stmt_bind_param
     */
    printf("[TODO] insert student\n");
    return 0;
}

static int handle_find_student(MYSQL* conn) {
    (void)conn;
    /*
     * TODO(练习3): 实现精确查询逻辑
     * 1. 输入查询字段和值（例如 phone_number=123）
     * 2. 组装 SELECT SQL
     * 3. mysql_query + mysql_store_result
     * 4. mysql_num_fields + mysql_fetch_row 打印每一行
     * 5. mysql_free_result 释放结果
     */
    printf("[TODO] find student\n");
    return 0;
}

static int handle_update_student_city(MYSQL* conn) {
    (void)conn;
    /*
     * TODO(练习4): 实现更新逻辑
     * 示例目标：把 name=han 的 city 更新为 dongguan
     * 建议附加：先 SELECT 再 UPDATE，验证更新前后变化
     */
    printf("[TODO] update student city\n");
    return 0;
}

static int handle_delete_student(MYSQL* conn) {
    (void)conn;
    /*
     * TODO(练习5): 实现删除逻辑
     * 1. 输入删除条件（例如 name=han）
     * 2. 执行 DELETE FROM students WHERE ...
     * 3. 打印受影响行数
     */
    printf("[TODO] delete student\n");
    return 0;
}

static int handle_show_all_students(MYSQL* conn) {
    const char* sql = "SELECT id, name, phone_number, city, age FROM students";
    MYSQL_RES* res = NULL;
    MYSQL_ROW row;

    if (mysql_query(conn, sql) != 0) {
        fprintf(stderr, "query failed: %s\n", mysql_error(conn));
        return -1;
    }

    res = mysql_store_result(conn);
    if (res == NULL) {
        fprintf(stderr, "store_result failed: %s\n", mysql_error(conn));
        return -1;
    }

    printf("id\tname\tphone_number\tcity\tage\n");
    while ((row = mysql_fetch_row(res)) != NULL) {
        printf(
            "%s\t%s\t%s\t%s\t%s\n",
            row[0] ? row[0] : "NULL",
            row[1] ? row[1] : "NULL",
            row[2] ? row[2] : "NULL",
            row[3] ? row[3] : "NULL",
            row[4] ? row[4] : "NULL"
        );
    }

    mysql_free_result(res);
    return 0;
}

static void print_menu(void) {
    printf("\n+-------- student system --------+\n");
    printf("1. insert\n");
    printf("2. find\n");
    printf("3. update\n");
    printf("4. delete\n");
    printf("5. show_all\n");
    printf("0. exit\n");
    printf("+--------------------------------+\n");
}