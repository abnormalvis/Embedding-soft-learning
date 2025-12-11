# Shell 与文件系统

## Shell 基础
- 常见 Shell：`bash`、`zsh`、`fish`。Linux 默认多为 `bash`。
- 命令结构：`command [options] [arguments]`，选项区分短格式(`-l`)和长格式(`--long`)。
- 管道 `|` 将上一个命令的标准输出作为下一个命令的标准输入。
- 重定向：`>` 覆盖写入文件，`>>` 追加，`2>` 重定向标准错误，`&>` 同时重定向 stdout/stderr。

```bash
ls -al /etc | grep "conf" > conf_files.txt
```

## 文件系统布局
- `/` 根目录，所有路径从此开始。
- `/bin`、`/usr/bin`：用户可执行程序；`/sbin`、`/usr/sbin`：系统管理程序。
- `/etc`：系统配置文件；`/var`：日志、缓存、队列等易变数据；`/tmp`：临时文件。
- `/home`：普通用户家目录；`/root`：超级用户家目录。

## 常用路径命令
- `pwd`：打印当前工作目录。
- `cd path`：切换目录，`cd -` 返回上一目录，`cd ~user` 进入指定用户家目录。
- `ls`：列出目录内容，常用参数 `-a` 显示隐藏文件，`-l` 显示详细信息，`-h` 人性化文件大小。
- `find`：按条件查找文件，例如：`find /var/log -type f -mtime -7`。

## 软硬链接
- **硬链接**：多个目录项指向同一 inode，只能在同一文件系统内创建。
- **软链接**：类似快捷方式，存储目标路径，可跨越文件系统。

```bash
ln original.txt hard_copy.txt
ln -s /usr/local/bin/tool /usr/bin/tool
```

## 通配符与扩展
- `*` 匹配任意长度，`?` 匹配单个字符，`[abc]` 匹配字符集合。
- 花括号扩展：`touch log_{info,warning,error}.txt`
- 命令替换：``echo "Today is $(date)"`` ；也可使用反引号 `` `command` ``。

## 环境变量
- 查看：`env` 或 `printenv`。
- 设置：`export PATH="/opt/tool/bin:$PATH"`；只对当前 Shell 有效。
- 持久化：写入 `~/.bashrc` 或 `~/.profile`。

## 文件系统监控
- `df -h`：查看磁盘使用情况。
- `du -sh *`：统计当前目录各项大小。
- `inotifywait`(需安装)：实时监控文件变更。

## 备份与归档
- `tar`：打包与解包。
  - 创建：`tar -czf backup.tar.gz project/`
  - 解压：`tar -xzf backup.tar.gz`
- `rsync`：增量同步，`rsync -av --delete src/ dest/`
