CURDIR=$(shell pwd)
INCDIR=$(CURDIR)/include
SRCDIR=$(CURDIR)/source
BUILDDIR=$(CURDIR)/build

CFLAGS=-I$(INCDIR) -Wall -Wextra -g

CC = gcc

# 目标程序
TEST_TARGET=$(BUILDDIR)/test_sequential_list
MAIN_TARGET=$(BUILDDIR)/main

# 源文件
TEST_SRCS = test_sequential_list.c 
TEST_SRCS += $(wildcard source/*.c)
MAIN_SRCS = main.c 
MAIN_SRCS += $(wildcard source/*.c)

# 目标文件
TEST_OBJS = $(addprefix $(BUILDDIR)/, $(notdir $(TEST_SRCS:.c=.o)))
MAIN_OBJS = $(addprefix $(BUILDDIR)/, $(notdir $(MAIN_SRCS:.c=.o)))

.PHONY: all test clean mkdir

all: mkdir $(TEST_TARGET) $(MAIN_TARGET)

# 测试程序
test: $(TEST_TARGET)
	@echo "Running tests..."
	@$(TEST_TARGET)

# 编译测试程序
$(TEST_TARGET): $(TEST_OBJS)
	$(CC) $(CFLAGS) -o $@ $^
	@echo "Built test program: $@"

# 编译主程序
$(MAIN_TARGET): $(MAIN_OBJS)
	$(CC) $(CFLAGS) -o $@ $^
	@echo "Built main program: $@"

# 编译目标文件
$(BUILDDIR)/%.o: %.c | mkdir
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILDDIR)/%.o: source/%.c | mkdir
	$(CC) $(CFLAGS) -c -o $@ $<

# 创建构建目录
mkdir:
	@mkdir -p $(BUILDDIR)

clean:
	rm -rf $(BUILDDIR)
	@echo "Cleaned build directory"