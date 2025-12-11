# 进程与作业管理

## 进程概念
- 进程是程序的运行实例，拥有独立的地址空间、文件描述符和进程号(PID)。
- 父子关系：新进程通过 `fork()` 创建，调用 `exec*` 系列加载新程序。
- `init` 或 `systemd` 是 PID 1，负责接管孤儿进程并处理僵尸进程。

## 常用命令
| 命令 | 功能 |
| ---- | ---- |
| `ps aux` | 查看所有进程，`ps -ef` 显示父子关系 |
| `top` / `htop` | 实时监控 CPU、内存占用，`htop` 支持交互操作 |
| `pstree` | 树状展示进程继承关系 |
| `pgrep NAME` | 按名称查找进程 PID |
| `pkill -TERM NAME` | 向匹配进程发送信号 |

```bash
ps -o pid,ppid,cmd -p 1234
```

## 信号
- 信号用来通知进程发生事件，常见如 `SIGINT`(Ctrl+C)、`SIGTERM`、`SIGHUP`、`SIGKILL`。
- 发送信号：`kill -TERM <pid>` 或 `killall -HUP nginx`。
- 进程可通过 `signal`/`sigaction` 捕获并自定义处理逻辑，`SIGKILL`、`SIGSTOP` 无法捕获。

## 作业控制
- 背景：Shell 允许一个终端中同时管理多个作业。
- `Ctrl+Z`：挂起当前前台进程，转入后台停止状态。
- `jobs`：列出当前 Shell 的作业及状态。
- `bg %1`：让指定作业在后台继续运行；`fg %1`：调回前台。
- 后台运行命令：`command &`。

## 守护进程
- 以后台长期运行的服务程序，如 `sshd`、`cron`。
- 创建方式：脱离控制终端、创建新会话、重定向标准流。
- 管理工具：`systemctl` 启动、停止和查看服务状态。

```bash
sudo systemctl status sshd
sudo systemctl restart nginx
```

## 监控与调试
- `strace -p PID`：跟踪系统调用与信号。
- `lsof -p PID`：列出进程打开的文件/套接字。
- `gdb -p PID`：附加调试某个进程。
- `perf top`：分析性能热点，需要 root 权限。

## 僵尸与孤儿进程
- **僵尸**：子进程结束但父进程未调用 `wait` 回收，会占用 PID 表项。
- **孤儿**：父进程提前结束，子进程由 `init/systemd` 接管，避免成为僵尸。
- 在编写守护进程或服务程序时务必正确处理子进程的回收逻辑。

## 资源限制
- `ulimit` 配置软/硬资源限制，如最大文件数、核心转储大小。
- `/etc/security/limits.conf` 可针对特定用户设置长期限制。
- `systemd-run --scope -p MemoryMax=512M command` 运行时动态施加 cgroup 限制。
