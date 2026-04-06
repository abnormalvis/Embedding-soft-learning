# Qt Quick 3D Teaching Demo

## What this demo teaches
- Basic 3D scene setup with `View3D`
- Camera and light configuration
- 3D model transform (rotation/scale)
- Real-time interaction through sliders and checkbox

## Build
```bash
cmake -S . -B build
cmake --build build -j
```

## Install Quick3D dependencies (Ubuntu/Debian)
```bash
sudo apt update
sudo apt install -y qt6-quick3d-dev qml6-module-qtquick3d
```

Then reconfigure and rebuild so CMake can detect `Qt6::Quick3D`:
```bash
cmake -S . -B build
cmake --build build -j
```

If you want configure-time hard failure when Quick3D is missing:
```bash
cmake -S . -B build -DREQUIRE_QUICK3D=ON
```

You can also use the helper script:
```bash
chmod +x ./install_quick3d_deps.sh
./install_quick3d_deps.sh
```

## Run
```bash
./build/qt_quick3d_demo
```

## Teaching points
1. `PerspectiveCamera` controls the viewpoint.
2. `DirectionalLight` affects shading and depth.
3. `Model` with `#Cube` source is ideal for quick experiments.
4. `NumberAnimation` can animate 3D properties directly.
