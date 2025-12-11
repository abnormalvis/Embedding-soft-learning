# Makefile for C Language Advanced Examples

CC=gcc
CFLAGS=-Wall -Wextra -std=c99 -g
LDFLAGS=-pthread

# 目录定义
ARRAYS_DIR=arrays
STRUCTS_DIR=structs
POINTERS_DIR=pointers
MEMORY_DIR=memory
FUNCTION_PTR_DIR=function_pointers
MODULAR_DIR=modular_design
PREPROCESSOR_DIR=preprocessor
CONCURRENCY_DIR=concurrency

# 可执行文件
TARGETS=array_demo struct_demo pointer_demo memory_demo \
        callbacks_demo modular_demo preprocessor_demo pthread_demo

.PHONY: all clean

all: $(TARGETS)

# Arrays
array_demo: $(ARRAYS_DIR)/array_demo.c
	$(CC) $(CFLAGS) $< -o $@

# Structs
struct_demo: $(STRUCTS_DIR)/struct_demo.c
	$(CC) $(CFLAGS) $< -o $@

# Pointers
pointer_demo: $(POINTERS_DIR)/pointer_demo.c
	$(CC) $(CFLAGS) $< -o $@

# Memory
memory_demo: $(MEMORY_DIR)/memory_layout_demo.c
	$(CC) $(CFLAGS) $< -o $@

# Function pointers
callbacks_demo: $(FUNCTION_PTR_DIR)/callbacks.c
	$(CC) $(CFLAGS) $< -o $@

# Modular design
modular_demo: $(MODULAR_DIR)/main.c $(MODULAR_DIR)/counter.c
	$(CC) $(CFLAGS) $^ -o $@

# Preprocessor
preprocessor_demo: $(PREPROCESSOR_DIR)/demo.c
	$(CC) $(CFLAGS) $< -o $@

# Concurrency
pthread_demo: $(CONCURRENCY_DIR)/pthread_demo.c
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@

clean:
	rm -f $(TARGETS)

# 帮助信息
help:
	@echo "Available targets:"
	@echo "  all              - Build all examples"
	@echo "  array_demo       - Build array examples"
	@echo "  struct_demo      - Build struct examples"
	@echo "  pointer_demo     - Build pointer examples"
	@echo "  memory_demo      - Build memory layout demo"
	@echo "  callbacks_demo   - Build function pointer callbacks"
	@echo "  modular_demo     - Build modular design example"
	@echo "  preprocessor_demo- Build preprocessor example"
	@echo "  pthread_demo     - Build pthread example"
	@echo "  clean            - Remove all built files"
