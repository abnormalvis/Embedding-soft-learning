#!/usr/bin/env bash
set -euo pipefail
sleep_cmd() {
    sleep 2 &
    pid=$!
    echo "spawned sleep with pid $pid"
    ps -o pid,ppid,cmd -p "$pid"
    wait "$pid"
    echo "background sleep finished"
}
case "${1:-}" in
    run)
        sleep_cmd
        ;;
    *)
        echo "usage: $0 run"
        ;;
esac
