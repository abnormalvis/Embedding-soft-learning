#!/usr/bin/env bash
set -euo pipefail

if ! command -v apt >/dev/null 2>&1; then
    echo "This helper currently supports apt-based systems (Ubuntu/Debian)."
    echo "Please install Qt6 Quick3D packages manually for your distro."
    exit 1
fi

echo "Installing Qt6 Quick3D development/runtime packages..."
sudo apt update
sudo apt install -y qt6-quick3d-dev qml6-module-qtquick3d

echo "Done. Reconfigure and rebuild your project:"
echo "  cmake -S . -B build"
echo "  cmake --build build -j"

