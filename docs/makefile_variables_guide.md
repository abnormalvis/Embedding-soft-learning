# Makefile 变量规则指南

Makefile 中的变量类似于编程语言中的变量，可以存储文本字符串（如文件名列表、编译器选项等）。合理使用变量可以使 Makefile 更简洁、易维护。

## 1. 变量的基础

### 定义与引用
变量定义通常使用赋值操作符。引用变量时，使用 `$(VAR_NAME)` 或 `${VAR_NAME}`。

```makefile
# 定义
CC = gcc
FILES = main.c utils.c

# 引用
all:
	$(CC) $(FILES) -o app
```

**注意**：单字符变量名可以不加括号（如 `$@`），但为了安全和一致性，建议自定义变量始终使用括号。

## 2. 变量赋值操作符

Makefile 提供了多种赋值方式，理解它们的区别至关重要。

### 2.1 递归展开赋值 (`=`)
变量的值在**使用时**才被展开（计算）。这意味着它可以引用后面定义的变量。

```makefile
foo = $(bar)
bar = Huh?

all:
	echo $(foo)
# 输出: Huh?
```
**缺点**：如果引用自身（如 `CFLAGS = $(CFLAGS) -O`），会导致无限递归错误。

### 2.2 简单扩展赋值 (`:=` 或 `::=`)
变量的值在**定义时**立即展开。这是最常用的赋值方式，行为类似大多数编程语言。

```makefile
x := foo
y := $(x) bar
x := later

all:
	echo $(y)
# 输出: foo bar (不受 x 后来改变的影响)
```

### 2.3 条件赋值 (`?=`)
只有当变量**尚未定义**时，才进行赋值。如果已经定义（包括定义为空），则忽略该赋值。

```makefile
CC ?= gcc
# 如果环境变量或前面已经定义了 CC，则这行无效；否则 CC 设为 gcc。
```

### 2.4 追加赋值 (`+=`)
向现有变量添加值。

```makefile
CFLAGS := -Wall
CFLAGS += -g
# CFLAGS 现在是: -Wall -g
```
`+=` 会自动处理空格。

## 3. 特殊变量

### 3.1 自动化变量 (Automatic Variables)
这些变量在规则命令中自动设定，针对每条规则上下文不同。

- `$@`: 目标文件名。
- `$<`: 第一个依赖文件名。
- `$^`: 所有依赖文件名（去重）。
- `$?`: 所有比目标新的依赖文件名。
- `$*`: 模式匹配的茎（stem），即 `%` 匹配的部分。

### 3.2 环境变量
make 启动时会自动将系统环境变量载入为 Makefile 变量。
如果在 Makefile 中重新定义了同名变量，会覆盖环境变量（除非使用了 `make -e`）。

## 4. 变量的高级用法

### 4.1 替换引用
可以将变量值中的后缀进行替换。
格式：`$(VAR:suffix=replacement)`

```makefile
SRCS = main.c foo.c
OBJS = $(SRCS:.c=.o)
# OBJS 变为: main.o foo.o
```

### 4.2 目标特定变量 (Target-specific Variable)
可以为某个特定目标定义变量，该变量仅在该目标（及其依赖）的规则中有效。

```makefile
# 全局 CFLAGS
CFLAGS = -Wall

# 仅在编译 main.o 时添加 -g
main.o: CFLAGS += -g

main.o: main.c
	$(CC) $(CFLAGS) -c main.c
```

### 4.3 override 指示符
通常命令行参数定义的变量（如 `make CFLAGS=-O2`）会覆盖 Makefile 中的定义。如果你希望 Makefile 中的定义优先，可以使用 `override`。

```makefile
override CFLAGS += -g
```

## 5. 总结建议

1.  **优先使用 `:=`**：除非你确实需要递归展开的特性，否则默认使用 `:=` 可以避免很多意外行为和性能问题。
2.  **大写命名**：按照惯例，常量和配置项（如 `CC`, `CFLAGS`）使用大写；内部临时变量可以使用小写。
3.  **善用 `+=`**：在构建选项列表时非常有用。
