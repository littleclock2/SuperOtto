# SuperOtto v3.2.0

跨平台多人网格寻路游戏 - 从 MFC 重构为 Qt 架构

> 程序图标：奶龙 (Nailong)

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

## 许可证

MIT License
