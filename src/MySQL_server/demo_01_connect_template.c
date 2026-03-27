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

	/*
	 * Socket 方式连接本机 MySQL（auth_socket 认证）
	 * host: localhost (或 NULL)
	 * user: root
	 * password: NULL (socket 不需要密码)
	 * db: student_info
	 * port: 0 (socket 方式时不使用 TCP 端口)
	 * unix_socket: /var/run/mysqld/mysqld.sock
	 */
	if (mysql_real_connect(conn, "localhost", "root", NULL, "student_info", 0, "/var/run/mysqld/mysqld.sock", 0) == NULL) {
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
