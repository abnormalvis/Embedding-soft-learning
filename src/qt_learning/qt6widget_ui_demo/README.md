# Qt6 Widgets UI Design Teaching Demo

## What this demo teaches
- Form layout with `QFormLayout`
- Data display/edit with `QTableWidget`
- Signal/slot interactions for add/update/delete
- Basic UI theme switching with runtime QSS

## Build
```bash
cmake -S . -B build
cmake --build build -j
```

## Run
```bash
./build/qt6widget_ui_demo
```

## Suggested class activities
1. Add input validation rules for phone format.
2. Replace `QTableWidget` with `QTableView + QAbstractTableModel`.
3. Move stylesheets to external `.qss` files.
