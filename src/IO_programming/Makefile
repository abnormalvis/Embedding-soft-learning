workspace := $(shell pwd)
INC_DIR := $(workspace)/include
SRC_DIR := $(workspace)/src

BUILD_DIR := $(workspace)/build
BIN_DIR := $(BUILD_DIR)/bin
OBJ_DIR := $(BUILD_DIR)/obj

CC := gcc
CFLAGS := -I$(INC_DIR) -g -Wall -Wextra -std=c11 -D_POSIX_C_SOURCE=200809L

# 通用库（不生成独立可执行文件）
COMMON_SRCS := $(SRC_DIR)/io_utils.c
COMMON_OBJS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(COMMON_SRCS))

# 自动发现 demo 源文件（排除通用库）
DEMO_SRCS := $(filter-out $(COMMON_SRCS),$(wildcard $(SRC_DIR)/*.c))
DEMO_NAMES := $(patsubst $(SRC_DIR)/%.c,%,$(DEMO_SRCS))
DEMO_BINS := $(addprefix $(BIN_DIR)/,$(DEMO_NAMES))
DEMO_OBJS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(DEMO_SRCS))

all: $(DEMO_BINS)
	@echo "Built: $(DEMO_NAMES)"

$(BIN_DIR)/%: $(OBJ_DIR)/%.o $(COMMON_OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) -o $@ $^ $(CFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)
	@echo "IO_programming build cleaned"

list-targets:
	@echo "可执行目标:"
	@for target in $(DEMO_NAMES); do echo "  $$target"; done

.PHONY: all clean list-targets

.SECONDARY: $(DEMO_OBJS) $(COMMON_OBJS)