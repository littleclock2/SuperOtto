# SuperOtto v3.1.0

基于 MFC 的双人对战网格路径规划游戏。玩家在网格地图上操控机器人，设置起点、终点与障碍物，通过 A* 算法自动寻路并沿路径动画移动，抢先到达奖杯得分。支持局域网 Socket 联机对战。

## 项目架构

```
SuperOtto3.1.0/
├── SuperOttoDlg.cpp/h    # 主对话框：UI绘制、用户交互、游戏逻辑、音量控制
├── AStar.cpp/h            # A*寻路算法 + BFS连通性检测
├── Robot.cpp/h            # 机器人类：位置管理与移动
├── MSocket.cpp/h          # 异步Socket封装（CAsyncSocket派生）
├── TCP.cpp                # 网络协议：地图收发、路径同步、客户端管理
├── SuperOtto.cpp/h        # MFC应用程序入口
├── resource.h             # 控件ID定义
├── SuperOtto.rc           # 资源文件（对话框布局、图标、版本信息）
├── framework.h            # MFC框架头文件
├── pch.cpp/h              # 预编译头
├── targetver.h            # 目标Windows版本
├── picturetest/           # 图片资源（背景、机器人、障碍物、奖杯等）
├── music/                 # 音乐资源（MP3/WMA/WAV）
├── musicPath.txt          # 音乐路径列表（兼容旧版）
└── scripts/               # 构建脚本（PowerShell）
```

### 核心模块

| 模块 | 职责 |
|------|------|
| **SuperOttoDlg** | 程序主干。负责网格绘制（双缓冲）、鼠标交互、游戏流程控制、音乐播放器（MCI）、音量滑块、冲刺动画（定时器）、窗口缩放（DeferWindowPos） |
| **AStar** | A* 寻路算法，使用曼哈顿距离启发式；`CanReach` 静态函数通过 BFS 检测两点连通性，用于障碍物放置验证 |
| **Robot** | 轻量机器人类，封装坐标位置与移动方法 |
| **MSocket / TCP** | 基于 `CAsyncSocket` 的网络通信。支持 Server/Client 模式，实现地图同步、路径广播、观众管理。Server 端可接受多个 Client 连接 |

### 游戏流程

```
启动 → 加载/生成网格 → 双方设置起点与终点
  → 随机生成障碍物（BFS连通性验证，动态上限30%）
  → 随机放置奖杯
  → 玩家轮流操作：设置冲刺目标 → A*寻路 → 沿路径动画移动
  → 到达奖杯得分 → 重新加载地图 → 循环
```

## 功能特性

- **A* 寻路**：四方向移动，自动绕过障碍物
- **冲刺系统**：鼠标悬停显示可达范围（BFS），点击冲刺目标后机器人沿路径平滑移动
- **双人对战**：本地轮流操作或局域网 Socket 联机
- **音乐播放器**：自动扫描 music 目录，支持播放/暂停/上一首/下一首，带垂直音量滑块
- **障碍物管理**：随机生成时自动验证连通性，防止玩家被困；支持手动清除
- **动态限制**：障碍物上限为网格总数的 30%，根据行×列自动计算
- **资源自适应**：自动创建 picturetest 和 music 文件夹；图片缺失时切换纯色背景模式
- **窗口缩放**：控件随窗口等比缩放（DeferWindowPos 批量重定位）

## 构建

使用 Visual Studio 2022 打开 `SuperOtto.sln` 编译，或使用命令行：

```powershell
# Release x64
powershell -ExecutionPolicy Bypass -File scripts/build.ps1 -Configuration Release -Platform x64
```

## 版本历史

### v3.1.0 (2026-05-01)

- 新增音量滑块控件（垂直滑动条 + 实时百分比显示）
- 障碍物生成加入 BFS 连通性验证，防止玩家被困
- 障碍物数目动态限制为网格总数的 30%
- 音乐播放器改为自动扫描 music 目录，不再依赖 musicPath.txt
- 程序启动时自动创建 picturetest 和 music 文件夹
- 修复冲刺过程中鼠标移动导致动画中断的问题
- 修复范围指示器跨越障碍物显示不准确的问题
- 优化非流畅模式下的绘制性能（GetUpdateRect 节流）
- 优化网格线绘制（单次 MoveTo/LineTo 替代逐格 Draw3dRect）
- UI 配色更新：浅蓝背景、金色奖杯、橙色/紫色机器人

### v3.0.1

- 初始版本
