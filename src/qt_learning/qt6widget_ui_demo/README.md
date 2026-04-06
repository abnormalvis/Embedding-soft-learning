# Qt6 Widgets UI Design Teaching Demo

## What this demo teaches
- Form layout with `QFormLayout`
- Data display/edit with `QTableWidget`
- Signal/slot interactions for add/update/delete
sh- MySQL-backed CRUD via `QSqlDatabase` (`QMYSQL`)
- Basic UI theme switching with runtime QSS

## Runtime dependencies (Ubuntu/Debian)
```bash
sudo apt update
sudo apt install -y libqt6sql6-mysql mysql-client
```

## Configure MySQL via environment variables
```bash
export DB_HOST=127.0.0.1
export DB_PORT=3306
export DB_NAME=qt_demo
export DB_USER=root
export DB_PASSWORD=your_password
```

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
