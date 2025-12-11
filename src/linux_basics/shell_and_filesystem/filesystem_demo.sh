#!/usr/bin/env bash
set -euo pipefail
workdir=$(mktemp -d)
trap 'rm -rf "$workdir"' EXIT
cd "$workdir"
echo "working directory: $PWD"
echo "creating sample tree"
mkdir -p projects/logs
printf "alpha\nbeta\n" > projects/list.txt
ls -al
echo "using find to locate .txt files"
find . -type f -name '*.txt'
echo "showing disk usage"
du -sh .
