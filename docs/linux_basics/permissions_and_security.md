# 权限与安全基础

## 用户与组
- 每个进程以某个用户和组身份运行；通过 UID/GID 控制访问权限。
- 查看当前用户：`id`；列出系统用户：`cat /etc/passwd`。
- 切换用户：`su - username`；临时以其他用户权限执行：`sudo -u user command`。

## 文件权限模型
- 访问控制基于 **所有者**、**所属组**、**其他人** 三个层级。
- 权限位：`r` 读、`w` 写、`x` 执行。目录的 `x` 表示是否可进入。
- 查看权限：`ls -l`；修改权限：`chmod`。

```bash
chmod u=rw,g=r,o= file.txt   # 精确赋值
chmod 755 script.sh          # 使用八进制
chown root:root /usr/local/bin/tool
```

## 特殊权限位
- `setuid`：用户ID位，执行文件以所有者身份运行(常见于 `passwd`)。
- `setgid`：组ID位，新建文件继承目录的组；可用于共享工作目录。
- `sticky bit`：目录下文件仅允许所有者删除(常用于 `/tmp`)。

## ACL 扩展访问控制
- `setfacl`/`getfacl` 提供更细粒度正/负权限。
- 示例：`setfacl -m u:alice:rw project.txt` 授予 alice 读写。

## sudo 配置
- `/etc/sudoers` 定义权限规则，应通过 `visudo` 编辑避免语法错误。
- 常用规则：`user ALL=(ALL) NOPASSWD: /usr/bin/systemctl restart service`
- 将用户加入 `sudo` 组可授予基本管理权限。

## SELinux 与 AppArmor
- SELinux：强制访问控制模型(MAC)，通过安全上下文限制进程与资源交互。
- 状态查看：`getenforce`；临时关闭：`setenforce 0`(不推荐长期关闭)。
- AppArmor：基于路径的 MAC 系统，Ubuntu 默认启用，配置位于 `/etc/apparmor.d/`。

## 文件完整性与审计
- `auditd`：记录安全事件，查看 `/var/log/audit/audit.log`。
- `tripwire`、`aide`：检测文件篡改。
- 常见安全实践：关闭不必要的服务、及时打补丁、限制 SSH Root 登录、使用长随机密码或密钥认证。

## 网络安全基础
- `iptables`/`nftables`：配置主机防火墙规则。
- `ufw`(Ubuntu) 或 `firewalld`(CentOS)：更友好的防火墙管理工具。
- 监控开放端口：`ss -tulpn` 或 `netstat -tulpn`。

## 日志与审计
- 系统日志由 `rsyslog` 或 `journald` 管理。
- `journalctl -u service` 查看特定服务日志。
- 配置 logrotate (`/etc/logrotate.d/`) 自动归档和清理日志。
