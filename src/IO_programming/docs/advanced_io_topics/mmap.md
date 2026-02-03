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
### 参考实现

```c
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];

    // 打开文件用于读取
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    // 获取文件大小
    struct stat st;
    if (fstat(fd, &st) == -1) {
        perror("fstat");
        close(fd);
        return 1;
    }

    off_t filesize = st.st_size;
    printf("File size: %ld bytes\n", filesize);

    if (filesize == 0) {
        close(fd);
        return 0;
    }

    // 内存映射
    void *addr = mmap(NULL, filesize, PROT_READ, MAP_PRIVATE, fd, 0);
    if (addr == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return 1;
    }

    printf("mmap succeeded, address: %p\n", addr);

    // 通过 write 输出内容
    ssize_t written = 0;
    while (written < filesize) {
        ssize_t n = write(STDOUT_FILENO,
                          (char *)addr + written,
                          filesize - written);
        if (n == -1) {
            if (errno == EINTR) continue;
            perror("write");
            munmap(addr, filesize);
            close(fd);
            return 1;
        }
        written += n;
    }

    // 解除映射
    if (munmap(addr, filesize) == -1) {
        perror("munmap");
        close(fd);
        return 1;
    }

    close(fd);
    printf("\nmmap-cat completed\n");

    return 0;
}
```

编译运行：
```bash
gcc -o mmap_cat mmap_cat.c
./mmap_cat /etc/passwd
./mmap_cat /tmp/large_file > /tmp/output.txt
```

**关键点：**
- `mmap()` 将文件映射到内存，但映射区域仍通过 `write()` 输出
- 处理短写：`write()` 可能不会一次写完所有数据
- 处理 `EINTR`：被信号中断时重试
- `MAP_PRIVATE` 表示对映射区的修改不会写回文件（写时复制）
- `MAP_SHARED` 则修改会写回文件

**性能考虑：**
- 对于小文件，普通 IO 可能更简单
- 对于大文件，mmap 可以避免一次显式拷贝
- 但缺页异常可能导致隐式 IO，需要实际测试