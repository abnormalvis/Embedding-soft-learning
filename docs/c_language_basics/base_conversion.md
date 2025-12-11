# 进制转换

## 基本概念
- C 语言中的整数常量可以使用不同前缀表示：`0b` 二进制(GNU 扩展)、`0` 八进制、`0x` 十六进制、无前缀为十进制。
- `printf` 系列通过格式控制符输出不同进制：`%d` 十进制、`%o` 八进制、`%x/%X` 十六进制、`%#x` 会在前面加上 `0x`。
- 字符串与整数之间的转换需要考虑溢出、非法字符以及符号。

## 标准库函数
- `strtol/strtoul/strtoll/strtoull`：可指定 `base` 参数(2~36 或 0)。当 `base` 为 0 时，函数自动根据前缀识别进制。
- `sscanf`：通过格式串直接解析，例如 `sscanf(text, "%x", &value);`。
- `sprintf`：将整数格式化写入字符串缓冲区。

```c
long to_decimal(const char *text, int base) {
    char *end = NULL;
    errno = 0;
    long value = strtol(text, &end, base);
    if (errno != 0 || end == text) {
        fprintf(stderr, "invalid number: %s\n", text);
        return 0;
    }
    return value;
}
```

## 手动转换
- **十进制转其他进制**：重复 `value % base` 取余并倒序排列。
- **其他进制转十进制**：从最高位开始累乘 `result = result * base + digit`。
- 为了高效处理二进制到十六进制的转换，可以每 4 位作为一组映射到单个十六进制字符。

```c
static const char alphabet[] = "0123456789ABCDEF";

size_t to_base_string(unsigned value, unsigned base, char *buffer, size_t capacity) {
    if (base < 2 || base > 16 || capacity < 2) {
        return 0;
    }
    size_t len = 0;
    do {
        buffer[len++] = alphabet[value % base];
        value /= base;
    } while (value && len + 1 < capacity);

    if (value != 0) {
        return 0; // 缓冲区不足
    }

    // 倒序
    for (size_t i = 0; i < len / 2; ++i) {
        char tmp = buffer[i];
        buffer[i] = buffer[len - 1 - i];
        buffer[len - 1 - i] = tmp;
    }
    buffer[len] = '\0';
    return len;
}
```

## 常见问题
- **溢出处理**：`strtol` 提供 `errno == ERANGE` 判断，可在读取前使用 `strtoll` 处理更大范围。
- **非法字符**：解析时要确保所有字符都在允许范围内；`strtol` 可通过 `end` 指针定位第一次非法字符。
- **大小写**：十六进制解析时应接受大小写字符，可统一调用 `tolower`。
- **前导符号**：手动解析时需要先处理 `+/-`，并防止 `--5` 等非法形式。

## 实战技巧
- 在网络协议或二进制文件中，往往需要结合位运算与移位来处理字节序。`ntohl/htonl` 等函数可以在不同端序之间转换。
- 多进制交互时建议编写单元测试覆盖边界情况，如最大值、最小值、零、包含前导零的表示等。
