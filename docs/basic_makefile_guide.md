# 基础 Makefile 编写指南

Makefile 是 `make` 工具的配置文件，用于自动化编译和构建项目。本指南将带你了解如何编写一个基础的 Makefile。

## 1. Makefile 的基本结构

Makefile 由一系列规则（Rules）组成。每条规则的形式如下：

```makefile
target: dependencies
	command
	command
	...
```

- **target (目标)**: 通常是你想要生成的文件名（如可执行文件或对象文件），也可以是一个执行动作的名称（如 `clean`）。
- **dependencies (依赖)**: 生成目标所需要的文件列表。如果依赖文件比目标文件新，或者目标文件不存在，`make` 就会执行命令。
- **command (命令)**: 构建目标所要执行的 Shell 命令。**注意：命令前必须使用 Tab 键缩进，不能使用空格。**

## 2. 一个简单的示例

假设我们有一个简单的 C 程序，包含 `main.c` 和 `hello.c`，以及头文件 `hello.h`。

### 版本 1: 最简单的写法

```makefile
app: main.c hello.c
	gcc main.c hello.c -o app
```

运行 `make` 将执行 gcc 命令生成 `app`。

### 版本 2: 分离编译（更高效）

为了避免每次修改一个文件就重新编译所有文件，我们可以将编译过程分为两步：先生成对象文件（.o），再链接成可执行文件。

```makefile
app: main.o hello.o
	gcc main.o hello.o -o app

main.o: main.c hello.h
	gcc -c main.c

hello.o: hello.c hello.h
	gcc -c hello.c
```

## 3. 使用变量

为了让 Makefile 更易于维护，我们可以使用变量。

```makefile
CC = gcc
CFLAGS = -Wall -g
OBJS = main.o hello.o
TARGET = app

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

main.o: main.c hello.h
	$(CC) $(CFLAGS) -c main.c

hello.o: hello.c hello.h
	$(CC) $(CFLAGS) -c hello.c
```

- `CC`: 指定编译器。
- `CFLAGS`: 指定编译选项（如 `-Wall` 开启警告，`-g` 开启调试信息）。
- `$(VAR)`: 引用变量。

## 4. 伪目标 (.PHONY)

有些目标并不是真正的文件，比如 `clean` 用于清理编译生成的文件。为了避免目录下恰好有一个名为 `clean` 的文件导致冲突，我们需要将其声明为伪目标。

```makefile
.PHONY: clean

clean:
	rm -f *.o app
```

## 5. 总结：一个完整的基础 Makefile

```makefile
CC = gcc
CFLAGS = -Wall -g
TARGET = app
OBJS = main.o hello.o

# 默认目标（第一个目标）
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

main.o: main.c hello.h
	$(CC) $(CFLAGS) -c main.c

hello.o: hello.c hello.h
	$(CC) $(CFLAGS) -c hello.c

.PHONY: clean all

clean:
	rm -f $(OBJS) $(TARGET)
```

编写好 Makefile 后，在终端运行 `make` 即可构建项目，运行 `make clean` 即可清理项目。
