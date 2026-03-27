CC := gcc
CFLAGS := -Wall -Wextra -Werror -std=c11 $(shell mysql_config --cflags)
LDFLAGS := $(shell mysql_config --libs)

TARGETS := server demo_01_connect_template demo_02_schema_seed_template demo_03_crud_menu_template

.PHONY: all clean help

all: $(TARGETS)

server: server.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

demo_01_connect_template: demo_01_connect_template.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

demo_02_schema_seed_template: demo_02_schema_seed_template.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

demo_03_crud_menu_template: demo_03_crud_menu_template.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

clean:
	rm -f $(TARGETS)

help:
	@echo "Usage:"
	@echo "  make           # 编译全部示例"
	@echo "  make server    # 只编译学生系统主文件"
	@echo "  make clean     # 清理可执行文件"
