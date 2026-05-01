$ErrorActionPreference = 'Continue'

Write-Host '=== SuperOtto 构建环境诊断 ==='

$msbuild = Get-Command msbuild -ErrorAction SilentlyContinue
if ($msbuild) {
    Write-Host "MSBuild: $($msbuild.Source)"
} else {
    Write-Host 'MSBuild: 未在 PATH 中找到'
}

$devenv = Get-Command devenv -ErrorAction SilentlyContinue
if ($devenv) {
    Write-Host "devenv: $($devenv.Source)"
} else {
    Write-Host 'devenv: 未在 PATH 中找到'
}

$vcvars = Get-ChildItem -Path "$Env:ProgramFiles\Microsoft Visual Studio" -Recurse -Filter vcvars64.bat -ErrorAction SilentlyContinue | Select-Object -First 1
if ($vcvars) {
    Write-Host "vcvars64.bat: $($vcvars.FullName)"
} else {
    Write-Host 'vcvars64.bat: 未找到（可能未安装 VC++ 工具链）'
}

$required = @(
    'SuperOtto.sln',
    'SuperOtto.vcxproj',
    'SuperOtto.rc',
    'scripts/build.ps1',
    'scripts/package.ps1'
)

$root = Split-Path -Parent $PSScriptRoot
foreach ($r in $required) {
    $p = Join-Path $root $r
    if (Test-Path $p) {
        Write-Host "OK: $r"
    } else {
        Write-Host "MISS: $r"
    }
}

Write-Host '=== 诊断结束 ==='
