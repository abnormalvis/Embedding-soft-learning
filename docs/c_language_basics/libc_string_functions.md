# libc 字符串函数

## 总体原则
- C 字符串以 `\0` 终止，操作前需保证源字符串可读且以 `\0` 结尾。
- 目标缓冲区大小必须提前知晓或可控，避免缓冲区溢出。
- 操作字符串前应清楚字符编码，本指南默认单字节 ASCII 或 UTF-8。

## strlen
- 原型：`size_t strlen(const char *s);`
- 功能：返回首个 `\0` 之前的字符数量。
- 复杂度：O(n)。在循环中频繁调用时应缓存结果。
- 风险：未终止字符串会越界读取；应与 `strnlen` 结合使用。

```c
size_t my_strlen(const char *s) {
    const char *p = s;
    while (*p) {
        ++p;
    }
    return (size_t)(p - s);
}
```

## strcpy / strncpy
- 原型：`char *strcpy(char *dest, const char *src);`
- 功能：复制源字符串到目标缓冲区（包含终止符）。
- 风险：若目标缓冲区不足会导致溢出。
- `strncpy` 会在不足长度时填充 `\0`，但若源字符串过长可能不追加终止符，使用后应手动补齐。

```c
char *my_strcpy(char *dest, const char *src) {
    char *p = dest;
    while ((*p++ = *src++) != '\0') {
        ;
    }
    return dest;
}
```

## strcat / strncat
- 原型：`char *strcat(char *dest, const char *src);`
- 功能：将源字符串追加到目标字符串末尾。
- 风险：需保证 `dest` 原有长度与追加内容总和不超过缓冲区。
- `strncat` 会追加最多 `n` 个字符，但仍会在末尾写入 `\0`。

## strcmp / strncmp
- 原型：`int strcmp(const char *lhs, const char *rhs);`
- 功能：按字典序比较两字符串。
- 返回值：负数、零、正数分别表示 lhs 小于、等于、大于 rhs。
- `strncmp` 至多比较前 `n` 个字符，适合前缀比较。

## strstr
- 原型：`char *strstr(const char *haystack, const char *needle);`
- 功能：查找子串首次出现位置。
- 返回值：指向 haystack 中的匹配位置，未找到返回 `NULL`。
- 优化：大数据场景可使用 KMP/Boyer-Moore；glibc 内部在长串上使用 Two-Way 探测算法。

## strchr / strrchr
- 原型：`char *strchr(const char *s, int c);`
- 功能：查找字符首次出现位置，搜索包含终止符。
- `strrchr` 寻找最后一次出现的位置，可用来定位文件路径的分隔符。

## strtok / strtok_r
- 原型：`char *strtok(char *str, const char *delim);`
- 功能：使用分隔符集合切分字符串，内部维护静态状态。
- 限制：线程不安全；原串会被修改。
- `strtok_r` 接受额外的状态指针参数，可用于多线程或并发解析。

```c
char *save = NULL;
for (char *tok = strtok_r(text, ",", &save); tok; tok = strtok_r(NULL, ",", &save)) {
    printf("%s\n", tok);
}
```

## 安全替代方案
- POSIX 提供 `strlcpy/strlcat`（在 glibc 需自行实现），返回写入的总长度，可判断是否被截断。
- Microsoft C 运行库提供 `_s` 后缀函数，如 `strcpy_s`，调用者必须传入目标缓冲区大小。
- C11 Annex K 定义 `strcpy_s/strcat_s` 等安全函数，但支持度有限。

## 调试建议
- 在开发阶段可使用地址消毒器(ASan)或 Valgrind 检测越界和未初始化读写。
- 对字符串操作编写单元测试，覆盖空串、极短/极长字符串、特殊字符、缓冲区边界。
- 在接口文档中明确约定：输入是否允许 `NULL`、是否包含终止符、是否可重复使用等信息。
