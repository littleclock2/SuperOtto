param(
    [ValidateSet('Debug','Release','SuperOtto')]
    [string]$Configuration = 'Release',

    [ValidateSet('x64','Win32')]
    [string]$Platform = 'x64',

    [switch]$Clean,
    [switch]$KillRunning,
    [switch]$VerboseLog
)

$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path -Parent $PSScriptRoot
$solution = Join-Path $projectRoot 'SuperOtto.sln'

if (-not (Test-Path $solution)) {
    throw "找不到解决方案: $solution"
}

if ($KillRunning) {
    Get-Process -Name 'SuperOtto' -ErrorAction SilentlyContinue | Stop-Process -Force
}

function Resolve-MsBuild {
    $candidates = @(
        "$Env:ProgramFiles\Microsoft Visual Studio\2022\Community\MSBuild\Current\Bin\MSBuild.exe",
        "$Env:ProgramFiles\Microsoft Visual Studio\2022\Professional\MSBuild\Current\Bin\MSBuild.exe",
        "$Env:ProgramFiles\Microsoft Visual Studio\2022\Enterprise\MSBuild\Current\Bin\MSBuild.exe",
        "$Env:ProgramFiles(x86)\Microsoft Visual Studio\2019\BuildTools\MSBuild\Current\Bin\MSBuild.exe",
        "$Env:ProgramFiles(x86)\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin\MSBuild.exe"
    )

    foreach ($p in $candidates) {
        if (Test-Path $p) { return $p }
    }

    $cmd = Get-Command msbuild -ErrorAction SilentlyContinue
    if ($cmd) { return $cmd.Source }

    return $null
}

$msbuild = Resolve-MsBuild
if (-not $msbuild) {
    throw "未找到 MSBuild。请安装 Visual Studio 2022 或 Build Tools。"
}

$target = if ($Clean) { 'Clean;Build' } else { 'Build' }
$logFile = Join-Path $projectRoot "build-$Configuration-$Platform.log"

$args = @(
    $solution,
    '/m',
    "/t:$target",
    "/p:Configuration=$Configuration",
    "/p:Platform=$Platform",
    '/p:PreferredToolArchitecture=x64',
    '/nologo',
    '/restore'
)

if ($VerboseLog) {
    $args += '/v:detailed'
} else {
    $args += '/v:minimal'
}

Write-Host "使用 MSBuild: $msbuild"
Write-Host "构建参数: Configuration=$Configuration, Platform=$Platform"

& $msbuild @args *>&1 | Tee-Object -FilePath $logFile

if ($LASTEXITCODE -ne 0) {
    Write-Host "构建失败，日志: $logFile" -ForegroundColor Red
    Write-Host "若报 LNK1104 无法写入 exe，请先关闭正在运行的 SuperOtto，或使用 -KillRunning。" -ForegroundColor Yellow
    exit $LASTEXITCODE
}

Write-Host "构建成功。日志: $logFile" -ForegroundColor Green
