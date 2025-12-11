# 网络基础与常用工具

## 网络接口
- 查看网络接口与状态：`ip addr` 或 `ip link`。
- 启用/禁用接口：`ip link set eth0 up/down`。
- 配置 IP：`sudo ip addr add 192.168.1.10/24 dev eth0`。
- 查看路由：`ip route`。

## 网络诊断工具
| 工具 | 功能 |
| ---- | ---- |
| `ping` | 检测连通性与时延，`ping -c 4 example.com` |
| `traceroute` / `tracepath` | 路由跟踪，观察数据包路径 |
| `mtr` | 综合 ping 与 traceroute，实时统计 |
| `dig` / `nslookup` | DNS 查询 |
| `ss` | 显示套接字连接状态 |

```bash
ss -tulpen   # TCP/UDP 监听端口及进程
mtr -rw example.com
```

## 抓包分析
- `tcpdump`：命令行抓包工具。
  - 常用：`sudo tcpdump -i eth0 port 80` 捕获指定端口。
  - 输出保存：`sudo tcpdump -w capture.pcap` 可用 Wireshark 分析。
- `wireshark`：图形界面强大抓包分析工具，适合深入分析协议。

## 网络服务管理
- `systemctl start|stop|status service`，管理如 `sshd`、`nginx` 等守护进程。
- 查看监听端口：`sudo lsof -iTCP -sTCP:LISTEN`
- `netcat(nc)`：可创建临时 TCP/UDP 服务或测试端口连通性，`nc -l 12345`。

## 防火墙基础
- `iptables` 示例：
  ```bash
  sudo iptables -A INPUT -p tcp --dport 22 -j ACCEPT
  sudo iptables -A INPUT -j DROP
  ```
- `nft`：新一代工具，与 `iptables` 规则共存，但建议只使用其一。
- `ufw` 示例：`sudo ufw allow 22/tcp`，默认策略 `sudo ufw default deny inbound`。

## 网络配置与管理
- `NetworkManager`：桌面环境常用网络管理服务，命令行工具 `nmcli`。
  - 列出连接：`nmcli connection show`
  - 启用 Wi-Fi：`nmcli device wifi connect SSID password PASS`
- 服务器场景常用 `systemd-networkd` 或手工配置 `/etc/network/interfaces`。

## 流量监控
- `iftop`：实时显示网络流量（需 root）。
- `nethogs`：按进程统计带宽使用情况。
- `vnstat`：长期流量统计，需守护进程收集数据。

## 资源与日志
- `journalctl -u NetworkManager` 查看网络守护进程日志。
- 系统级网络日志通常位于 `/var/log/syslog` 或 `/var/log/messages`。
- 当遭遇网络问题时，结合 `ping`、`ss`、`tcpdump`、`journalctl` 多维度排查。
