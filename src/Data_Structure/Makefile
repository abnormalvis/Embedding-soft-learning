CURDIR=$(shell pwd)
INCDIR=$(CURDIR)/include
SRCDIR=$(CURDIR)/source
BUILDDIR=$(CURDIR)/build

CFLAGS=-I$(INCDIR) -Wall -Wextra -g

CC = gcc

# 目标程序（每个可执行文件只允许一个 main）
SEQ_TEST_TARGET=$(BUILDDIR)/test_sequential_list
DLL_TEST_TARGET=$(BUILDDIR)/test_doubly_linked_list
LL_TEST_TARGET=$(BUILDDIR)/test_linked_list
MAIN_TARGET=$(BUILDDIR)/main

# 模块源文件（按需链接，避免把其它 demo/main 一起链接进来导致 multiple definition）
SEQ_MODULE_SRCS = source/sequential_list.c
DLL_MODULE_SRCS = source/doubly_linked_list.c
LL_MODULE_SRCS  = source/linked_list.c

# 各目标的源文件
SEQ_TEST_SRCS = test_sequential_list.c $(SEQ_MODULE_SRCS)
DLL_TEST_SRCS = test_doubly_linked_list.c $(DLL_MODULE_SRCS)
LL_TEST_SRCS  = test_linked_list.c $(LL_MODULE_SRCS)

# 主程序：只链接 main.c + 你希望在主程序里使用的模块
# 当前 main.c 只演示栈(list_stack)，因此只链接对应实现；避免引入其它模块造成命名冲突。
MAIN_SRCS = main.c source/list_stack.c

# 目标文件（保留相对路径，避免同名 .c 在 build/ 下生成同名 .o 互相覆盖）
SEQ_TEST_OBJS = $(addprefix $(BUILDDIR)/,$(SEQ_TEST_SRCS:.c=.o))
DLL_TEST_OBJS = $(addprefix $(BUILDDIR)/,$(DLL_TEST_SRCS:.c=.o))
LL_TEST_OBJS  = $(addprefix $(BUILDDIR)/,$(LL_TEST_SRCS:.c=.o))
MAIN_OBJS     = $(addprefix $(BUILDDIR)/,$(MAIN_SRCS:.c=.o))


.PHONY: all test dll-test linked-test main clean mkdir

all: mkdir $(SEQ_TEST_TARGET) $(DLL_TEST_TARGET) $(LL_TEST_TARGET)

# 主程序需要的头文件目前存在同名符号冲突（node/is_empty），因此改为显式构建。
main: $(MAIN_TARGET)

# 测试程序
test: $(SEQ_TEST_TARGET)
	@echo "Running tests..."
	@$(SEQ_TEST_TARGET)

dll-test: $(DLL_TEST_TARGET)
	@echo "Running DLL tests..."
	@$(DLL_TEST_TARGET)

linked-test: $(LL_TEST_TARGET)
	@echo "Running linked list tests..."
	@$(LL_TEST_TARGET)

# 编译测试程序
$(SEQ_TEST_TARGET): $(SEQ_TEST_OBJS)
	$(CC) $(CFLAGS) -o $@ $^
	@echo "Built test program: $@"

$(DLL_TEST_TARGET): $(DLL_TEST_OBJS)
	$(CC) $(CFLAGS) -o $@ $^
	@echo "Built DLL test program: $@"

$(LL_TEST_TARGET): $(LL_TEST_OBJS)
	$(CC) $(CFLAGS) -o $@ $^
	@echo "Built linked list test program: $@"

# 编译主程序
$(MAIN_TARGET): $(MAIN_OBJS)
	$(CC) $(CFLAGS) -o $@ $^
	@echo "Built main program: $@"

# 编译目标文件：把源文件的相对路径映射到 build/ 下的相同路径
$(BUILDDIR)/%.o: %.c | mkdir
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<

# 创建构建目录
mkdir:
	@mkdir -p $(BUILDDIR)

clean:
	rm -rf $(BUILDDIR)
	@echo "Cleaned build directory"