@echo off
echo =====================================
echo SuperOtto 3.1.0 编译脚本
echo =====================================
echo.

REM 创建必要的目录
if not exist "picturetest" mkdir picturetest
if not exist "music" mkdir music
if not exist "x64\Release" mkdir x64\Release

echo 已创建必要的目录结构。
echo.
echo 请使用 Visual Studio 2022 打开 SuperOtto.sln 进行编译。
echo.
echo 提示: 程序会自动创建所需的文件夹，无需手动配置。
echo.
pause
