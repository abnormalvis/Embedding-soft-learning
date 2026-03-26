# HTTP Server Demo 说明与问题分析

## 1. 当前实现概览

核心文件：
- `server.c`：epoll + 非阻塞 socket 的 HTTP Demo
- `files/html/default.html`：首页 HTML 模板
- `files/image`、`files/video`：静态资源目录

当前演示路由：
- `/` -> `./files/html/default.html`
- `/hello` -> 文本响应
- `/image` -> 图片文件
- `/video` -> 视频文件

## 2. 本轮问题答疑总结

### 2.1 send_all 里为什么是 `p + sent` 和 `len - sent`

`send` 在非阻塞模式下可能只发送一部分数据。
- `p + sent`：从“尚未发送”的位置继续发
- `len - sent`：本次剩余待发送长度

### 2.2 send flags 为什么是 0

`flags = 0` 表示默认发送语义，不附加额外行为。
在 Demo 场景最直接，便于理解主流程。

### 2.3 EINTR / EAGAIN / EWOULDBLOCK 的含义

- `EINTR`：系统调用被信号中断，可重试
- `EAGAIN` / `EWOULDBLOCK`：非阻塞 socket 当前不可写，需要稍后重试

### 2.4 open 成功后为什么还要 fstat

`open` 只说明拿到了 fd，不代表后续元信息读取一定成功。
`fstat` 用于读取文件大小并写入 `Content-Length`，失败时应返回 500。

### 2.5 send 和 send_all 的区别

- `send`：一次系统调用，可能部分发送
- `send_all`：封装循环重试，目标是发送完整缓冲区

### 2.6 为什么要判断 `strstr(uri, "..")`

这是目录穿越防护的基础手段，阻止 `../` 访问站点根目录外文件。

## 3. 本轮新增安全补强（P1 + P2）

### 3.1 P1：请求解析安全

1. 请求行解析改为有边界版本：
- `sscanf(request, "%15s %255s %15s", ...)`
- 解析失败返回 `400 Bad Request`

2. 增加基础完整性检查：
- 未出现 `\r\n\r\n` 的请求视为不完整，返回 400

### 3.2 P2：路径与 URI 安全

1. URI 百分号解码（处理 `%2e` 等）
2. 屏蔽 `..` 路径片段
3. 通过 `realpath` 约束访问范围在 `./files` 下
4. 检查父目录和目标文件的规范化路径，阻止符号链接穿越

## 4. 本轮新增协议能力（P3 已落地）

1. 增加 HEAD 支持
- `GET` 和 `HEAD` 均可访问 `/`、`/hello`、`/image`、`/video`
- `HEAD` 仅返回响应头，不返回 body

2. 增加视频 Range 206 支持
- `/video` 支持 `Range: bytes=...` 单段范围请求
- 合法范围返回 `206 Partial Content` + `Content-Range`
- 非法范围返回 `416 Range Not Satisfiable`

3. MIME 改为表驱动映射
- 使用扩展名映射表统一管理 `Content-Type`
- 后续扩展新类型仅需增表项

## 5. 本轮新增并发能力（线程池）

1. 主线程职责
- 负责 `accept` 新连接
- 负责 `epoll_wait` 事件分发
- 将就绪连接投递到线程池任务队列

2. 工作线程职责
- 从任务队列取连接
- 执行 `recv -> handle_request -> send -> close`
- 通过固定数量线程提升并发请求处理能力

3. 当前配置
- 工作线程数：`THREAD_POOL_SIZE = 4`
- 任务队列容量：`TASK_QUEUE_SIZE = 1024`

## 6. 已知风险与限制

1. 仅支持 GET/HEAD
- 不支持 POST/PUT/DELETE

2. 仍是 Connection: close 模型
- 每次请求后关闭连接，不支持 keep-alive

3. 大文件发送仍在主事件循环内串行推进
- 高并发下，大文件会影响其他连接时延

4. 仅基础 HTTP 解析
- 目前只解析请求首行与最小校验，不是完整 HTTP 解析器

5. Range 当前仅在 `/video` 路由启用
- 其他静态文件仍按整文件返回

## 7. 建议改进路线

### P3（短期）
1. 将固定资源路径改为配置化
2. 为图片/普通静态文件按需启用 Range
3. 支持 If-Modified-Since / ETag 等缓存头

### P4（中期）
1. 增加 Range 请求支持（206）
2. 引入连接状态机并支持 keep-alive
3. 将大文件发送改为 EPOLLOUT 驱动的异步续传

## 8. 编译与验证

编译：
```bash
gcc -Wall -Wextra -O2 server.c -o http_server -pthread
```

基础验证：
```bash
curl -i http://127.0.0.1:8080/
curl -i http://127.0.0.1:8080/hello
curl -i http://127.0.0.1:8080/image
curl -i http://127.0.0.1:8080/video
curl -I http://127.0.0.1:8080/video
curl -i -H "Range: bytes=0-99" http://127.0.0.1:8080/video
```

安全验证样例：
```bash
curl -i "http://127.0.0.1:8080/%2e%2e/%2e%2e/etc/passwd"
curl -i "http://127.0.0.1:8080/../../etc/passwd"
```

预期：上述穿越请求应返回 `403` 或 `400`，不应读取系统文件。
