#include <mysql.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

// 辅助函数：打印查询结果
static void print_result(MYSQL *conn) {
    MYSQL_RES *result = mysql_store_result(conn);
    if (result == NULL) {
        fprintf(stderr, "store result failed: %s\n", mysql_error(conn));
        return;
    }

    int num_fields = mysql_num_fields(result);
    MYSQL_ROW row;

    // 打印列标题
    printf("\n%-5s %-15s %-20s %-15s %-5s\n", "ID", "Name", "Phone", "City", "Age");
    printf("%-5s %-15s %-20s %-15s %-5s\n", "---", "---------------", "--------------------", "---------------", "-----");

    // 打印每一行数据
    while ((row = mysql_fetch_row(result))) {
        //unsigned long *lengths = mysql_fetch_lengths(result);  // 此变量未使用，已注释
        for (int i = 0; i < num_fields; i++) {
            if (i == 0) {  // ID列
                printf("%-5s ", row[i] ? row[i] : "NULL");
            } else if (i == 1) {  // Name列
                printf("%-15s ", row[i] ? row[i] : "NULL");
            } else if (i == 2) {  // Phone列
                printf("%-20s ", row[i] ? row[i] : "NULL");
            } else if (i == 3) {  // City列
                printf("%-15s ", row[i] ? row[i] : "NULL");
            } else if (i == 4) {  // Age列
                printf("%-5s ", row[i] ? row[i] : "NULL");
            }
        }
        printf("\n");
    }

    mysql_free_result(result);
}

int main(void) {
  MYSQL *conn = mysql_init(NULL);
  char line[64];

  if (conn == NULL) {
    fprintf(stderr, "mysql_init failed\n");
    return 1;
  }

  if (mysql_real_connect(conn, "127.0.0.1", "root", "88143208", "student_info",
                         3306, NULL, 0) == NULL) {
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
      // 查询所有学生信息
      const char *query_sql = "SELECT id, name, phone_number, city, age FROM students";
      
      if (mysql_query(conn, query_sql) != 0) {
        fprintf(stderr, "query failed: %s\n", mysql_error(conn));
      } else {
        printf("\n--- Students List ---\n");
        print_result(conn);
      }
    } else if (op == 2) {
      // 更新城市信息
      char name[64];
      char new_city[64];
      
      printf("Enter name to update: ");
      if (fgets(name, sizeof(name), stdin) == NULL) {
        printf("Error reading name\n");
        continue;
      }
      // 移除换行符
      name[strcspn(name, "\n")] = 0;
      
      printf("Enter new city: ");
      if (fgets(new_city, sizeof(new_city), stdin) == NULL) {
        printf("Error reading city\n");
        continue;
      }
      // 移除换行符
      new_city[strcspn(new_city, "\n")] = 0;
      
      // 构造更新SQL（防止SQL注入，使用转义字符）
      char escaped_name[128];
      char escaped_city[128];
      mysql_real_escape_string(conn, escaped_name, name, strlen(name));
      mysql_real_escape_string(conn, escaped_city, new_city, strlen(new_city));
      
      char update_sql[512]; // 增大缓冲区大小以避免溢出
      snprintf(update_sql, sizeof(update_sql), 
               "UPDATE students SET city='%s' WHERE name='%s'", 
               escaped_city, escaped_name);
      
      if (mysql_query(conn, update_sql) != 0) {
        fprintf(stderr, "update failed: %s\n", mysql_error(conn));
      } else {
        int affected_rows = mysql_affected_rows(conn);
        if (affected_rows > 0) {
          printf("Update successful! %d row(s) affected.\n", affected_rows);
        } else {
          printf("No rows were updated. Check if the name exists.\n");
        }
      }
    } else if (op == 3) {
      // 删除指定姓名的学生
      char name_to_delete[64];
      
      printf("Enter name to delete: ");
      if (fgets(name_to_delete, sizeof(name_to_delete), stdin) == NULL) {
        printf("Error reading name\n");
        continue;
      }
      // 移除换行符
      name_to_delete[strcspn(name_to_delete, "\n")] = 0;
      
      // 转义输入以防止SQL注入
      char escaped_name[128];
      mysql_real_escape_string(conn, escaped_name, name_to_delete, strlen(name_to_delete));
      
      char delete_sql[512]; // 增大缓冲区大小以避免溢出
      snprintf(delete_sql, sizeof(delete_sql), 
               "DELETE FROM students WHERE name='%s'", 
               escaped_name);
      
      if (mysql_query(conn, delete_sql) != 0) {
        fprintf(stderr, "delete failed: %s\n", mysql_error(conn));
      } else {
        int affected_rows = mysql_affected_rows(conn);
        if (affected_rows > 0) {
          printf("Delete successful! %d row(s) affected.\n", affected_rows);
        } else {
          printf("No rows were deleted. Check if the name exists.\n");
        }
      }
    } else {
      printf("invalid option\n");
    }
  }

  mysql_close(conn);
  return 0;
}