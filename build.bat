@echo off
echo === SuperOtto Build Script ===
echo.

REM Set Qt 5.15.2 paths
set QT_DIR=D:\Qt\5.15.2\mingw81_64
set MINGW_DIR=D:\Qt\Tools\mingw810_64
set CMAKE_DIR=D:\Qt\Tools\CMake_64

REM Add tools to PATH
set PATH=%MINGW_DIR%\bin;%CMAKE_DIR%\bin;%QT_DIR%\bin;%PATH%

REM Check for cmake
where cmake >nul 2>&1
if %errorlevel% neq 0 (
    echo [ERROR] CMake not found. Searched in: %CMAKE_DIR%\bin
    pause
    exit /b 1
)

echo Configuring project...
cmake -B build -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH="%QT_DIR%" -DCMAKE_BUILD_TYPE=Release
if %errorlevel% neq 0 (
    echo [ERROR] CMake configuration failed.
    pause
    exit /b 1
)

echo Building project...
cmake --build build --config Release -j%NUMBER_OF_PROCESSORS%
if %errorlevel% neq 0 (
    echo [ERROR] Build failed.
    pause
    exit /b 1
)

echo.
echo === Build successful! ===
echo Executable: build\SuperOtto.exe
echo.
echo To run, copy resources\music\ and resources\pictures\ next to the exe.
echo Or run: run.bat
pause
