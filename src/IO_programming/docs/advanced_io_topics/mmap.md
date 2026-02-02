# mmap：内存映射文件

`mmap(2)` 允许把文件（或匿名内存）映射到进程虚拟地址空间。之后你可以像访问内存一样访问文件内容。

## 1. 头文件与函数原型

```c
#include <sys/mman.h>
#include <sys/types.h>

void *mmap(void *addr, size_t length, int prot, int flags, int fd, off_t offset);
int munmap(void *addr, size_t length);
```

## 2. 常用参数

- `prot`：权限
  - `PROT_READ` / `PROT_WRITE` / `PROT_EXEC`
- `flags`：映射方式
  - `MAP_PRIVATE`：写时复制（不会写回文件）
  - `MAP_SHARED`：共享映射（修改可写回文件，需配合 `msync` 或依赖回写时机）

`offset` 必须按页对齐（通常 4096）。

## 3. 典型用途

- 大文件读取：减少一次用户态拷贝，随机访问方便
- 共享内存：`shm_open + mmap` 或 `memfd_create + mmap`
- 实现零拷贝风格的解析（注意仍可能发生缺页与磁盘 IO）

## 4. 注意事项与坑

- 访问越界会触发 `SIGSEGV`。
- 映射不是“读完就没事”：缺页会导致隐式 IO，性能分析要考虑 page fault。
- 写回时机不确定：需要强一致时用 `msync()` 或明确的写入策略。

## 5. 练习

实现 `mmap-cat`：用 `mmap` 读取文件内容并写到 stdout（注意仍要用 `write` 输出）。
