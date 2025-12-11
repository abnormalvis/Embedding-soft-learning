#!/usr/bin/env bash
set -euo pipefail
if command -v ip >/dev/null 2>&1; then
    echo "interfaces:" 
    ip -brief addr
else
    echo "ip command not available"
fi
if command -v ping >/dev/null 2>&1; then
    echo "pinging localhost"
    ping -c 2 127.0.0.1 >/dev/null && echo "ping succeeded" || echo "ping failed"
fi
if command -v ss >/dev/null 2>&1; then
    echo "listening sockets"
    ss -tulpn | head -n 5
fi
