$ErrorActionPreference = 'Stop'
$projectRoot = Split-Path -Parent $PSScriptRoot

Get-Process -Name 'SuperOtto' -ErrorAction SilentlyContinue | Stop-Process -Force

$paths = @(
    (Join-Path $projectRoot 'x64'),
    (Join-Path $projectRoot 'SuperOtto\x64'),
    (Join-Path $projectRoot '.vs'),
    (Join-Path $projectRoot 'dist')
)

foreach ($p in $paths) {
    if (Test-Path $p) {
        Remove-Item $p -Recurse -Force -ErrorAction SilentlyContinue
        Write-Host "已清理: $p"
    }
}

Get-ChildItem -Path $projectRoot -Filter '*.log' -File -ErrorAction SilentlyContinue | Remove-Item -Force -ErrorAction SilentlyContinue
Write-Host '清理完成。' -ForegroundColor Green
