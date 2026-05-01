# SuperOtto 构建与打包脚本

## 1) 清理（可选）
在项目根目录执行：

```powershell
powershell -ExecutionPolicy Bypass -File .\scripts\clean.ps1
```

## 0) 先做环境诊断（建议）

```powershell
powershell -ExecutionPolicy Bypass -File .\scripts\diagnose-build.ps1
```

如果提示找不到 `MSBuild` / `vcvars64.bat`，请在 Visual Studio Installer 中安装：
- 使用 C++ 的桌面开发
- MFC（x86 和 x64）

## 2) 构建
推荐 Release|x64：

```powershell
powershell -ExecutionPolicy Bypass -File .\scripts\build.ps1 -Configuration Release -Platform x64 -KillRunning -Clean

## 5) 一键发布（构建+打包）

```powershell
powershell -ExecutionPolicy Bypass -File .\scripts\release.ps1 -Configuration Release -Platform x64
```
```

- `-VerboseLog`：输出详细日志

日志文件会输出到项目根目录，例如：`build-Release-x64.log`。

## 3) 打包

```powershell
powershell -ExecutionPolicy Bypass -File .\scripts\package.ps1 -Configuration Release -Platform x64
```

打包结果位于：`dist\SuperOtto-<Configuration>-<Platform>-<时间戳>.zip`

## 4) 资源说明
构建后会自动复制以下资源到输出目录（已在工程中配置 Post-Build）：
- `picturetest/`
- `music/`
- `musicPath.txt`

如果你是从 VS IDE 直接 Build，也会自动复制这些资源。
