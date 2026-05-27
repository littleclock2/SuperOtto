# SuperOtto

跨平台多人网格寻路游戏 - 从 MFC 重构为 Qt 架构


## 功能特性

- 双人回合制网格寻路对战
- A* 自动寻路算法（曼哈顿距离启发式）
- 可自由调整分辨率（800x600 ~ 1920x1080 + 自定义）
- 默认分辨率 1960x1200
- TCP 联网对战（房主/客户端模式）
- 内置音乐播放器
- 支持图片和纯色两种渲染模式
- 深度中文界面
- 智能操作：单击选中格子，双击设置目标/清除障碍
- 回合切换提示弹窗（带淡出动画）
- 字体自适应缩放

## 一键构建

### Windows
```
双击 build.bat
```

### Linux / macOS
```bash
chmod +x build.sh
./build.sh
```

## 运行

### Windows
```
双击 run.bat
```

### Linux / macOS
```bash
cd build
./SuperOtto
```

## 环境要求

- CMake 3.16+
- Qt 5.15+ (Widgets, Network, Multimedia)
- MinGW 8.1+ 或 GCC 7+ 支持 C++17

### 安装 Qt

#### Windows
1. 下载 Qt 5.15.2: https://www.qt.io/download-qt-installer
2. 选择 MinGW 8.1.0 64-bit 组件
3. 安装完成后确保 Qt 和 MinGW 在 PATH 中

#### macOS
```bash
brew install qt@5
```

#### Linux (Ubuntu/Debian)
```bash
sudo apt install qt5-default qtbase5-dev qtmultimedia5-dev
```

## 项目结构

```
SuperOtto/
  CMakeLists.txt      # CMake 构建配置
  src/
    main.cpp          # 程序入口
    core/             # 游戏核心逻辑（纯 C++，无 Qt 依赖）
      Grid.cpp/h      # 网格管理
      Robot.cpp/h     # 机器人类
      AStar.cpp/h     # A*寻路 + BFS连通性检测
      GameManager.cpp/h # 游戏流程控制
    network/          # TCP 联网模块
      Protocol.cpp/h  # 网络协议解析
      NetworkManager.cpp/h # 网络连接管理
    audio/            # 音乐播放模块
      MusicPlayer.cpp/h # QMediaPlayer 封装
    ui/               # Qt 界面
      GameWidget.cpp/h    # 游戏画布（QPainter自定义绘制）
      MainWindow.cpp/h    # 主窗口
      SettingsWidget.cpp/h # 地图设置面板
      NetworkWidget.cpp/h # 网络设置面板
      MusicWidget.cpp/h   # 音乐播放面板
  resources/
    resources.qrc     # Qt资源文件
    icons/            # 程序图标
    music/            # 音乐文件（运行时从外部加载）
    picturetest/      # 图片资源（运行时从外部加载）
```

## 游戏操作

- **单击格子**：选中该格子（显示选中标记）
- **双击格子**：
  - 如果是障碍物 → 清除障碍
  - 如果是空地/奖杯 → 设置为目标并自动寻路
- **回合切换**：每次移动后自动切换玩家
- **范围高亮**：当前玩家周围显示可达范围

## 网络联机

1. 一端选择"主机"角色，点击"连接"启动服务器
2. 另一端选择"客户端"角色，输入IP和端口，点击"连接"
3. 连接成功后自动同步地图
4. 双方轮流操作，实时同步路径

## 音乐播放器

- 支持 MP3、WAV、WMA、OGG、FLAC 格式
- 自动扫描 music 目录
- 支持播放/暂停/停止/音量调节
- 可添加/移除本地音乐文件

## 从旧版迁移

如果是从 MFC 版本（SuperOtto3.1.0）迁移：
1. 将 `music/` 和 `picturetest/` 目录放在可执行文件的上级目录或同级目录
2. 程序启动时会自动搜索并加载这些资源

## 版本历史

### v3.2.0 (2026-05-08)

**Qt5 重构版**

- 从 MFC 重构为 Qt5 跨平台架构
- 全界面深度中文本地化
- 默认分辨率 1960x1200
- 智能操作：单击选中、双击设置目标/清除障碍
- 回合切换弹窗提示（带淡出动画）
- 字体自适应缩放系统
- 奶龙程序图标
- A*寻路 + BFS范围高亮
- TCP联网对战
- 内置音乐播放器

### v3.1.0 (2026-05-01)

**音量控制与交互优化**

- 新增音量滑块控件（垂直滑动条 + 实时百分比显示）
- 修复冲刺过程中鼠标移动导致动画中断的问题
- 修复范围指示器跨越障碍物显示不准确的问题

**障碍物系统改进**

- 障碍物生成加入 BFS 连通性验证，防止玩家被困
- 障碍物数目动态限制为网格总数的 30%，根据行×列自动计算

**音乐播放器重构**

- 音乐播放器改为自动扫描 music 目录，不再依赖 musicPath.txt
- 程序启动时自动创建 picturetest 和 music 文件夹

**性能优化**

- 优化非流畅模式下的绘制性能（GetUpdateRect 节流）
- 优化网格线绘制（单次 MoveTo/LineTo 替代逐格 Draw3dRect）

**UI 改进**

- UI 配色更新：浅蓝背景、金色奖杯、橙色/紫色机器人

**工程化**

- 新增 .gitignore、README.md、构建脚本
- 构建目标从 Debug 切换为 Release

### v3.0.1 (2024-07)

**构建自动化**

- 新增 PowerShell 构建脚本集：`build.ps1`、`clean.ps1`、`diagnose-build.ps1`、`package.ps1`、`release.ps1`
- 新增 `.vscode/tasks.json` VS Code 任务配置
- 新增 `build.bat` 快捷构建入口

**资源管理改进**

- 图片资源目录统一为 `picturetest/`，移除旧 `picture/` 目录
- 图片路径改为相对路径，不再依赖绝对路径 `D:\picturetest\`

**音乐库更新**

- 替换全部背景音乐为中文歌曲（3首）

**其他**

- 新增 `路径规划相关.zip` 参考资料

### v2.8.0 (2024-06)

**图片资源大幅扩展**

- 新增 `picturetest/` 目录，包含 10 张图片资源
- 新增障碍物贴图：`Wall1.png` ~ `Wall4.png`（4种墙壁纹理）
- 新增奖杯图标：`Cup.png`
- 新增路径指示：`path.png`
- 新增范围指示：`range.png`
- 新增背景图：`bk.JPG`

**保留功能**

- 沿用 v2.3.0 的 TCP 网络模块和音乐播放功能
- 保留 5 首日文背景音乐

### v2.3.0 (2024-06)

**网络对战**

- 新增 `TCP.cpp` 网络协议模块
- 基于 `CAsyncSocket` 实现局域网 Socket 联机
- 支持 Server/Client 模式，地图同步与路径广播

**多媒体**

- 新增音乐播放功能，内置 5 首日文背景音乐
- 新增 `picture/` 目录，包含基础图片资源（`1.png`、`2.png`、`bk.png`）

**核心玩法**

- 实现 A* 寻路算法（四方向移动，曼哈顿距离启发式）
- 实现冲刺系统（鼠标悬停显示可达范围，点击后沿路径移动）
- 实现双人对战模式（本地轮流操作）

### v1.3.0 (2024-06)

**初始版本 - 基础框架**

- 基于 MFC 对话框的网格地图界面
- 基本网格渲染与鼠标交互
- 机器人位置管理与移动
- A* 寻路算法基础实现
- 项目工程文件（Visual Studio 解决方案）

**功能清单**

- 网格地图绘制
- 起点/终点设置
- 障碍物放置
- A* 自动寻路
- 机器人沿路径移动动画

## 许可证

MIT License

---

---

Maintained by contributors.
