workspace=$(shell pwd)
INC_DIRS:=$(workspace)/include
SRC_DIRS:=$(workspace)/source
BIN_DIR:=$(workspace)/bin
OBJ_DIR:=$(BIN_DIR)/build

CFLAGS=-I$(INC_DIRS) -g -Wall -Wextra

CC=gcc

# 定义可执行目标
TARGETS=main

# 默认目标：只编译 main
all: $(BIN_DIR)/main

# 编译 main (空程序)
$(BIN_DIR)/main: $(OBJ_DIR)/main.o
	@mkdir -p $(BIN_DIR)
	$(CC) -o $@ $^ $(CFLAGS)

# 通用编译规则
$(OBJ_DIR)/%.o: $(SRC_DIRS)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BIN_DIR)
	@echo "编译文件已清理"

.PHONY: all clean
