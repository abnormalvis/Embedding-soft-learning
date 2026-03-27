#include <mysql.h>

#include <stdio.h>
#include <stdlib.h>

/*
 * demo_03: CRUD 菜单模板
 * 目标：
 * 1) 按菜单分发查询、修改、删除
 * 2) 使用 mysql_store_result / mysql_fetch_row 打印结果
 */

static void menu(void) {
    printf("\n=== demo_03 menu ===\n");
    printf("1. query all\n");
    printf("2. update city by name\n");
    printf("3. delete by name\n");
    printf("0. exit\n");
}

int main(void) {
    MYSQL *conn = mysql_init(NULL);
    char line[64];

    if (conn == NULL) {
        fprintf(stderr, "mysql_init failed\n");
        return 1;
    }

    if (mysql_real_connect(conn, "127.0.0.1", "root", "123456", "student_info", 3306, NULL, 0) == NULL) {
        fprintf(stderr, "connect failed: %s\n", mysql_error(conn));
        mysql_close(conn);
        return 1;
    }

    while (1) {
        int op = -1;
        menu();
        printf("choose> ");
        if (fgets(line, sizeof(line), stdin) == NULL) {
            break;
        }
        op = atoi(line);

        if (op == 0) {
            break;
        }

        if (op == 1) {
            /* TODO:
             * SELECT id, name, phone_number, city, age FROM students;
             * 然后打印每行
             */
            printf("[TODO] query all\n");
        } else if (op == 2) {
            /* TODO:
             * 从输入读取 name 和 new_city
             * 执行 UPDATE students SET city='...' WHERE name='...';
             */
            printf("[TODO] update city by name\n");
        } else if (op == 3) {
            /* TODO:
             * 从输入读取 name
             * 执行 DELETE FROM students WHERE name='...';
             */
            printf("[TODO] delete by name\n");
        } else {
            printf("invalid option\n");
        }
    }

    mysql_close(conn);
    return 0;
}
