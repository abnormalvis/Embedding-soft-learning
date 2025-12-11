# 标准输入输出

## io 模型概述
- `stdio.h` 提供带缓冲的文件流接口，核心类型为 `FILE`。
- 每个进程默认打开 `stdin`、`stdout`、`stderr` 三个标准流。
- 缓冲策略取决于目标：终端通常使用行缓冲，文件使用全缓冲，`stderr` 通常不缓冲。

## 常用函数
| 类别 | 函数 | 用途 |
| ---- | ---- | ---- |
| 输出 | `printf` `fprintf` `sprintf` `snprintf` | 格式化输出到终端、文件或字符串 |
| 输入 | `scanf` `fscanf` `sscanf` | 按格式读取数据，需注意边界检查 |
| 行读取 | `fgets` `getline` | 获取一行文本，`getline` 会自动扩容缓冲区(GNU 扩展) |
| 二进制 | `fread` `fwrite` | 块读写，返回成功读写的元素个数 |
| 文件操作 | `fopen` `fclose` `fflush` | 打开、关闭和刷新文件流 |

```c
FILE *fp = fopen("data.bin", "rb");
if (!fp) {
    perror("fopen");
    return -1;
}

uint8_t buffer[256];
size_t nread = fread(buffer, 1, sizeof(buffer), fp);
if (nread < sizeof(buffer) && ferror(fp)) {
    perror("fread");
}

fclose(fp);
```

## 格式化输入输出注意事项
- `printf` 的格式串需与参数类型匹配，使用 `%zu/%zd` 打印 `size_t/ssize_t`。
- `scanf` 家族需要提供变量地址，建议在格式串中限制最大输入长度，例如 `scanf("%31s", buf);`。
- `scanf` 读取失败时会保留之前读取的字符，必要时使用 `fgets` + `sscanf` 组合提高可靠性。

## 缓冲与刷新
- `setvbuf(stream, buf, _IOLBF, size);` 可手动配置缓冲类型。
- `fflush(stdout);` 强制刷新输出缓冲区，常用于交互式程序。
- 当程序崩溃或 `exit` 正常返回时，库会自动刷新并关闭所有打开的流；`_Exit` 不会触发这一行为。

## 错误处理
- 大部分 `stdio` 函数通过返回值和 `ferror`/`feof` 提供错误状态。
- `perror` 根据 `errno` 打印错误描述，`clearerr` 可以重置流状态。
- 在循环读取时必须同时检查 `feof` 和 `ferror`，区分正常结束与读错。

## 高级主题
- **重定向**：使用 `freopen(NULL, "w", stdout);` 将输出重定向到不同文件。
- **线程安全**：`stdio` 函数通常是线程安全的，但过度使用全局锁可能造成性能瓶颈，必要时改用 `write` 等无缓冲系统调用。
- **自定义流**：POSIX 提供 `fopencookie`/`funopen` 可自定义 `FILE` 行为，例如在内存中模拟文件。
- **宽字符版本**：`wprintf/wscanf` 支持宽字符，在处理多字节编码时需注意本地化设置。
