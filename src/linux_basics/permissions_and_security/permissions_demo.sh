#!/usr/bin/env bash
set -euo pipefail
sandbox=$(mktemp -d)
trap 'rm -rf "$sandbox"' EXIT
cd "$sandbox"
echo "creating files in $PWD"
touch config.txt script.sh
chmod 600 config.txt
chmod 755 script.sh
echo "resulting permissions:"
ls -l
