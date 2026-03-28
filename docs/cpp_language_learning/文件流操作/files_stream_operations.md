# 文件流操作

## 目录
1. [文件流基础](#文件流基础)
2. [打开模式](#打开模式)
3. [文本读写](#文本读写)
4. [二进制读写](#二进制读写)
5. [文件指针定位](#文件指针定位)
6. [错误处理与健壮性](#错误处理与健壮性)
7. [常见坑](#常见坑)

---

## 文件流基础

C++ 文件流核心类型：

- ifstream：读文件
- ofstream：写文件
- fstream：读写文件

基本流程：打开 -> 操作 -> 关闭。

RAII 下对象析构会自动 close，但显式 close 可提升语义清晰度。

---

## 打开模式

常用模式：

- ios::in
- ios::out
- ios::app
- ios::binary
- ios::trunc

```cpp
std::ofstream out("a.txt", std::ios::out | std::ios::trunc);
```

---

## 文本读写

### 写文件

```cpp
#include <fstream>

int main() {
    std::ofstream out("log.txt");
    if (!out.is_open()) return 1;
    out << "hello\n";
}
```

### 读文件（逐行）

```cpp
#include <fstream>
#include <string>

int main() {
    std::ifstream in("log.txt");
    if (!in.is_open()) return 1;

    std::string line;
    while (std::getline(in, line)) {
        // process line
    }
}
```

---

## 二进制读写

```cpp
#include <fstream>
#include <vector>

int main() {
    std::vector<char> data = {'A', 'B', 'C'};

    std::ofstream out("data.bin", std::ios::binary);
    out.write(data.data(), static_cast<std::streamsize>(data.size()));

    std::ifstream in("data.bin", std::ios::binary);
    std::vector<char> buf(3);
    in.read(buf.data(), static_cast<std::streamsize>(buf.size()));
}
```

---

## 文件指针定位

- seekg / tellg：输入流读指针
- seekp / tellp：输出流写指针

```cpp
in.seekg(0, std::ios::end);
auto size = in.tellg();
in.seekg(0, std::ios::beg);
```

常用于获取文件大小、随机读取块数据。

---

## 错误处理与健壮性

建议检查：

- is_open
- fail / bad / eof
- read/write 返回后状态

对于关键逻辑可结合异常：

```cpp
in.exceptions(std::ios::failbit | std::ios::badbit);
```

---

## 常见坑

1. 文本模式读二进制文件导致数据变形。
2. 未检查 gcount 就处理缓冲区。
3. 误用 eof 作为循环条件。
4. 相对路径基于运行目录，导致“找不到文件”。
5. 大文件读写未分块，内存压力过高。
