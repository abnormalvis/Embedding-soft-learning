# Makefile 函数使用指南

Makefile 提供了丰富的内置函数，用于处理文件名、字符串、逻辑控制等。函数的调用格式为 `$(function arguments)`，参数之间用逗号分隔。

## 1. 字符串处理函数

### `subst` (字符串替换)
将字符串 `text` 中的 `from` 替换为 `to`。
- **格式**: `$(subst from,to,text)`
- **示例**:
  ```makefile
  STR = feet on the street
  RES = $(subst ee,EE,$(STR))
  # 结果: fEEt on the strEEt
  ```

### `patsubst` (模式替换)
查找 `text` 中符合模式 `pattern` 的单词，替换为 `replacement`。`%` 代表通配符。
- **格式**: `$(patsubst pattern,replacement,text)`
- **示例**:
  ```makefile
  SRCS = main.c foo.c
  OBJS = $(patsubst %.c,%.o,$(SRCS))
  # 结果: main.o foo.o
  ```
  *注：这等同于变量替换引用 `$(SRCS:.c=.o)`。*

### `strip` (去空格)
去掉字符串开头和结尾的空格，并将中间多个空格合并为一个。
- **格式**: `$(strip string)`

### `findstring` (查找字符串)
在 `in` 中查找 `find`。如果找到返回 `find`，否则返回空。
- **格式**: `$(findstring find,in)`

### `filter` (过滤)
保留 `text` 中符合模式 `pattern` 的单词。可以有多个模式。
- **格式**: `$(filter pattern...,text)`
- **示例**:
  ```makefile
  FILES = main.c main.h foo.c bar.o
  SRCS = $(filter %.c %.s,$(FILES))
  # 结果: main.c foo.c
  ```

### `filter-out` (反过滤)
去除 `text` 中符合模式 `pattern` 的单词。
- **格式**: `$(filter-out pattern...,text)`
- **示例**:
  ```makefile
  OBJS = main.o foo.o bar.o
  # 去除 main.o
  OTHERS = $(filter-out main.o,$(OBJS))
  # 结果: foo.o bar.o
  ```

## 2. 文件名处理函数

### `dir` (取目录)
取出文件名序列中每个文件名的目录部分。
- **格式**: `$(dir names...)`
- **示例**: `$(dir src/foo.c hacks)` -> `src/ ./`

### `notdir` (取文件名)
取出文件名序列中每个文件名的非目录部分。
- **格式**: `$(notdir names...)`
- **示例**: `$(notdir src/foo.c)` -> `foo.c`

### `suffix` (取后缀)
- **格式**: `$(suffix names...)`
- **示例**: `$(suffix src/foo.c src-1.0/bar.c hacks)` -> `.c .c`

### `basename` (取前缀)
取出包含目录但不包含后缀的部分。
- **格式**: `$(basename names...)`
- **示例**: `$(basename src/foo.c)` -> `src/foo`

### `addprefix` (加前缀)
- **格式**: `$(addprefix prefix,names...)`
- **示例**: `$(addprefix src/,foo bar)` -> `src/foo src/bar`

### `addsuffix` (加后缀)
- **格式**: `$(addsuffix suffix,names...)`
- **示例**: `$(addsuffix .c,foo bar)` -> `foo.c bar.c`

### `wildcard` (通配符扩展)
获取匹配模式的文件名列表（检查文件系统）。
- **格式**: `$(wildcard pattern)`
- **示例**:
  ```makefile
  SRCS = $(wildcard src/*.c)
  # 结果: src/main.c src/utils.c ... (实际存在的文件)
  ```

## 3. 逻辑控制函数

### `if`
- **格式**: `$(if condition,then-part[,else-part])`
- **说明**: 如果 `condition` 非空，展开 `then-part`，否则展开 `else-part`。

### `error`
产生一个致命错误，提示信息为 `text`，并停止 make 执行。
- **格式**: `$(error text...)`
- **示例**:
  ```makefile
  ifdef ERROR_FLAG
      $(error Stopping because ERROR_FLAG is set)
  endif
  ```

### `warning`
产生一个警告信息，但不会停止执行。
- **格式**: `$(warning text...)`

## 4. 其他常用函数

### `shell`
执行 Shell 命令并返回输出。
- **格式**: `$(shell command)`
- **示例**:
  ```makefile
  DATE := $(shell date +%Y-%m-%d)
  FILES := $(shell find . -name "*.c")
  ```
  *注意：频繁调用 shell 函数会影响 make 的性能。*

### `foreach`
循环处理列表。
- **格式**: `$(foreach var,list,text)`
- **说明**: 把 `list` 中的单词逐个取出赋给 `var`，然后展开 `text`。
- **示例**:
  ```makefile
  DIRS = a b c
  FILES = $(foreach dir,$(DIRS),$(wildcard $(dir)/*.c))
  # 遍历 a, b, c 目录下的所有 .c 文件
  ```

## 5. 调试函数

### `info`
打印信息到标准输出。
- **格式**: `$(info text...)`
- **用途**: 调试变量值。
  ```makefile
  $(info The value of SRCS is $(SRCS))
  ```
