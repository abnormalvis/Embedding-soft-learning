CURDIR=$(shell pwd)
INCDIR=$(CURDIR)/include
SRCDIR=$(CURDIR)/source

CPPFLAGS=-I$(INCDIR)
# 指定编译所用的编译器
CC = gcc
TARGET = main  # 最终生成的可执行文件名
SRCS = main.c  # 只包含main.c，排除migrate_data.c
SRCS += $(foreach dir,$(SRCDIR),$(wildcard $(dir)/*.c)) # 添加源文件目录下的所有C源文件
OBJS = $(patsubst %.c,%.o, $(SRCS)) # 将源文件名转换为对应的目标文件名

# all : $(TARGET) 定义默认构建目标 all，依赖于 $(TARGET)（通常是最终生成的可执行文件或库）
all: $(TARGET)
$(TARGET):$(OBJS)	# 定义 $(TARGET)的构建规则，依赖于所有 .o文件（$(OBJS)）
	$(CC)  $(OBJS) -o $@	# 相当于 gcc *.o -o main

%.o:%.c
	$(CC) $(CPPFLAGS) -c $< -o $@ 

clean:
	rm -rf ./*.o
	rm -rf $(SRCDIR)/*.o
	rm -f ./main
	-make clean -C source 2>/dev/null