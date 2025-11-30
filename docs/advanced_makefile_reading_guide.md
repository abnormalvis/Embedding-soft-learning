# 复杂 Makefile 阅读指南

当你接手一个大型项目时，Makefile 往往非常复杂。本指南将介绍一些高级特性，帮助你读懂复杂的 Makefile。

## 1. 自动化变量 (Automatic Variables)

在规则的命令中，经常会看到 `$@`, `$<`, `$^` 等符号，它们是自动化变量，代表规则中的不同部分。

- **`$@`**: 规则的目标文件名 (Target)。
- **`$<`**: 规则的第一个依赖文件名 (First Prerequisite)。
- **`$^`**: 规则的所有依赖文件列表 (All Prerequisites)，去重。
- **`$?`**: 所有比目标新的依赖文件列表。

**示例：**

```makefile
app: main.o hello.o
	gcc $^ -o $@
# 等价于: gcc main.o hello.o -o app

main.o: main.c
	gcc -c $< -o $@
# 等价于: gcc -c main.c -o main.o
```

## 2. 模式规则 (Pattern Rules)

为了避免为每个文件都写一条规则，Makefile 使用 `%` 作为通配符来定义模式规则。

```makefile
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
```

这条规则的意思是：对于任何以 `.o` 结尾的目标，都依赖于对应的 `.c` 文件。命令是将 `.c` 编译为 `.o`。这是 Makefile 中最常见的通用规则。

## 3. 常用函数

Makefile 提供了许多内置函数，调用格式为 `$(function arguments)`。

### wildcard
获取匹配模式的文件名列表。
```makefile
SRC = $(wildcard *.c)
# 结果: main.c hello.c (当前目录下所有 .c 文件)
```

### patsubst
模式替换。
```makefile
OBJ = $(patsubst %.c, %.o, $(SRC))
# 结果: main.o hello.o (将 SRC 中的 .c 替换为 .o)
```

### shell
执行 Shell 命令并获取结果。
```makefile
CURRENT_DIR = $(shell pwd)
DATE = $(shell date)
```

### addprefix / addsuffix
加前缀或后缀。
```makefile
LIBS = $(addprefix -l, m pthread)
# 结果: -lm -lpthread
```

## 4. 变量赋值的不同方式

- **`=` (递归展开)**: 变量的值在使用时才计算。如果引用了后面定义的变量，也能获取到值。
- **`:=` (立即展开)**: 变量的值在定义时立即计算。这是最常用的赋值方式，行为更像普通编程语言。
- **`?=` (条件赋值)**: 如果变量未定义，则赋值；否则保持原值。
- **`+=` (追加赋值)**: 向变量追加值。

## 5. 包含其他文件 (include)

大型项目的 Makefile 通常会拆分成多个文件。

```makefile
include config.mk
include src/Makefile
```

如果在行首看到 `-include` (如 `-include .depend`)，表示如果文件不存在也不报错，继续执行。

## 6. 条件判断

Makefile 支持类似 C 语言预处理的条件判断。

```makefile
ifeq ($(DEBUG), 1)
    CFLAGS += -g
else
    CFLAGS += -O2
endif
```

## 7. 调试技巧

如果你看不懂 Makefile 是如何执行的，可以使用以下命令：

- **`make -n`**: 只打印要执行的命令，但不实际执行。这对于查看构建流程非常有帮助。
- **`make -p`**: 打印 make 的内部数据库，包括所有的规则和变量（包括默认规则）。
- **`info/warning/error`**: 在 Makefile 中添加打印信息。
  ```makefile
  $(info The compiler is $(CC))
  ```

## 8. 综合阅读策略

1. **找目标**: 先看 `all` 目标或第一个目标，这是入口。
2. **看变量**: 理解 `SRC`, `OBJ`, `CFLAGS`, `LDFLAGS` 等核心变量的定义。
3. **追依赖**: 顺着目标的依赖链往下看，结合模式规则 `%.o: %.c` 理解文件是如何生成的。
4. **查函数**: 遇到不懂的 `$(...)`，查阅函数文档或通过 `$(info ...)` 打印出来看看。
