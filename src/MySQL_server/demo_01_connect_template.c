#include <mysql.h>

#include <stdio.h>
#include <stdlib.h>

/*
 * demo_01: 连接数据库最小模板
 * 目标：
 * 1) 学会 mysql_init / mysql_real_connect / mysql_close
 * 2) 学会打印 mysql_error
 */
int main(void) {
	MYSQL *conn = mysql_init(NULL);
	if (conn == NULL) {
		fprintf(stderr, "mysql_init failed\n");
		return 1;
	}

	/* TODO:
	 * 把下面参数改成你本机 MySQL 配置
	 * host, user, password, db, port
	 */
	if (mysql_real_connect(conn, "127.0.0.1", "root", "123456", "student_info", 3306, NULL, 0) == NULL) {
		fprintf(stderr, "connect failed: %s\n", mysql_error(conn));
		mysql_close(conn);
		return 1;
	}

	printf("[demo_01] connect success\n");

	/* TODO(进阶):
	 * 1) 调用 mysql_set_character_set(conn, "utf8mb4")
	 * 2) 调用 mysql_ping(conn) 检查连接状态
	 */

	mysql_close(conn);
	return 0;
}
