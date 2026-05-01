param(
    [ValidateSet('Debug','Release','SuperOtto')]
    [string]$Configuration = 'Release',

    [ValidateSet('x64','Win32')]
    [string]$Platform = 'x64'
)

$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path -Parent $PSScriptRoot

Write-Host '开始完整流程：构建 + 打包' -ForegroundColor Cyan

powershell -ExecutionPolicy Bypass -File (Join-Path $PSScriptRoot 'build.ps1') -Configuration $Configuration -Platform $Platform -KillRunning -Clean
powershell -ExecutionPolicy Bypass -File (Join-Path $PSScriptRoot 'package.ps1') -Configuration $Configuration -Platform $Platform

Write-Host '完成。' -ForegroundColor Green
