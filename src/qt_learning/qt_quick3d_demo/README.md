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

## Run
```bash
./build/qt_quick3d_demo
```

## Teaching points
1. `PerspectiveCamera` controls the viewpoint.
2. `DirectionalLight` affects shading and depth.
3. `Model` with `#Cube` source is ideal for quick experiments.
4. `NumberAnimation` can animate 3D properties directly.
