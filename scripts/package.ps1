param(
    [ValidateSet('Debug','Release','SuperOtto')]
    [string]$Configuration = 'Release',

    [ValidateSet('x64','Win32')]
    [string]$Platform = 'x64',

    [string]$OutputDir = ''
)

$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path -Parent $PSScriptRoot

if ([string]::IsNullOrWhiteSpace($OutputDir)) {
    $OutputDir = Join-Path $projectRoot 'dist'
}

if (-not (Test-Path $OutputDir)) {
    New-Item -ItemType Directory -Path $OutputDir | Out-Null
}

$binDir = Join-Path $projectRoot "$Platform\$Configuration"
$exePath = Join-Path $binDir 'SuperOtto.exe'

if (-not (Test-Path $exePath)) {
    throw "未找到可执行文件: $exePath。请先执行 scripts/build.ps1。"
}

$stamp = Get-Date -Format 'yyyyMMdd-HHmmss'
$staging = Join-Path $OutputDir "SuperOtto-$Configuration-$Platform-$stamp"
New-Item -ItemType Directory -Path $staging | Out-Null

Copy-Item $exePath -Destination $staging -Force

$mustCopy = @('picturetest', 'music', 'musicPath.txt')
foreach ($name in $mustCopy) {
    $from = Join-Path $binDir $name
    if (-not (Test-Path $from)) {
        $from = Join-Path $projectRoot $name
    }

    if (Test-Path $from) {
        Copy-Item $from -Destination $staging -Recurse -Force
    }
}

$zip = "$staging.zip"
if (Test-Path $zip) { Remove-Item $zip -Force }
Compress-Archive -Path (Join-Path $staging '*') -DestinationPath $zip -Force

Write-Host "打包完成: $zip" -ForegroundColor Green
