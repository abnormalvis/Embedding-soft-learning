# 构建与调试工具

## 编译工具链
- GNU 工具集：`gcc`/`g++` 编译器，`ld` 链接器，`as` 汇编器。
- `make`：基于依赖的构建工具，配合 `Makefile`；`cmake` 可生成多种构建系统。
- `pkg-config`：查询已安装库的编译参数，`pkg-config --cflags --libs libname`。

## 调试器
- `gdb`：GNU 调试器，支持断点、单步、变量查看。
  ```bash
  gdb ./app
  (gdb) break main
  (gdb) run
  (gdb) bt     # 查看调用栈
  ```
- `lldb`：LLVM 提供的调试器，命令格式类似。
- 图形前端：`cgdb`、`ddd`、`nemiver` 提供更友好的界面。

## 内存与性能分析
- `valgrind`：检测内存泄漏和非法访问，`valgrind --leak-check=full ./app`。
- `massif`：Valgrind 工具之一，分析堆使用情况，`valgrind --tool=massif`。
- `perf`：性能剖析工具，可收集 CPU 周期、缓存、分支预测等统计。
  ```bash
  perf record ./app
  perf report
  ```
- `gprof`：基于编译器插桩的性能分析，需使用 `-pg` 编译和链接。

## 静态分析与代码质量
- `cppcheck`：静态分析常见错误，`cppcheck src/`。
- `clang-tidy`：基于 Clang 的分析工具，集成丰富规则。
- `clang-format`：统一代码风格，配合 `.clang-format` 配置文件。

## 构建日志与排查
- 使用 `make VERBOSE=1` 或 `cmake --build . --verbose` 查看详细命令。
- `strace -f -o log.txt make` 捕获构建期间的系统调用，定位文件缺失问题。
- 环境变量 `CFLAGS`、`LDFLAGS` 控制全局编译选项，`export CFLAGS="-O2 -Wall"`。

## 版本控制
- `git` 是主流分布式版本控制系统。
  - 克隆：`git clone repo.git`
  - 状态：`git status`
  - 提交：`git add file && git commit`
  - 查看差异：`git diff`
- 合理编写 `.gitignore` 避免无关文件纳入仓库。

## 软件包管理
- Debian/Ubuntu：`apt-get install package`；CentOS/RHEL：`yum` 或 `dnf`。
- 构建依赖往往需要开发版包，例如 `libssl-dev`、`zlib-devel`。
- 使用 `apt-cache search` 或 `dnf provides` 查询包信息。

## 容器化与虚拟化
- `docker`：隔离运行环境，确保构建/运行一致性。
- `podman`：无守护进程容器工具，兼容 docker 命令行。
- 虚拟化：`virt-manager`、`VirtualBox` 方便测试不同发行版或内核版本。

## 持续集成
- 常用 CI：GitHub Actions、GitLab CI、Jenkins。
- 编写脚本自动化编译、测试、打包流程，提高回归测试覆盖率。
- 在 CI 环境中合理划分构建、测试、部署阶段，输出日志与工件便于追踪。
