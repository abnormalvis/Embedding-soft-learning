CC := gcc
CFLAGS := -Wall -Wextra -Werror -std=c11 $(shell mysql_config --cflags)
LDFLAGS := $(shell mysql_config --libs)
CFLAGS_TEST := $(CFLAGS) -lm

TARGETS := server demo_01_connect_template demo_02_schema_seed_template demo_03_crud_menu_template
TEST_TARGETS := test_demo_02_schema_seed test_demo_03_crud_menu

.PHONY: all clean help test run-tests

all: $(TARGETS)

server: server.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

demo_01_connect_template: demo_01_connect_template.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

demo_02_schema_seed_template: demo_02_schema_seed_template.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

demo_03_crud_menu_template: demo_03_crud_menu_template.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

# 编译测试程序
test: $(TEST_TARGETS)

test_demo_02_schema_seed: test_demo_02_schema_seed.c
	$(CC) $(CFLAGS_TEST) $< -o $@ $(LDFLAGS)

test_demo_03_crud_menu: test_demo_03_crud_menu.c
	$(CC) $(CFLAGS_TEST) $< -o $@ $(LDFLAGS)

# 运行测试
run-tests: test
	./test_demo_02_schema_seed
	./test_demo_03_crud_menu

clean:
	rm -f $(TARGETS) $(TEST_TARGETS) test_demo_03_crud_menu

help:
	@echo "Usage:"
	@echo "  make           # 编译全部示例"
	@echo "  make server    # 只编译学生系统主文件"
	@echo "  make test      # 编译测试程序 (包括 demo_02 和 demo_03)"
	@echo "  make run-tests # 编译并运行所有测试"
	@echo "  make clean     # 清理可执行文件"
