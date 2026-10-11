# Builds the Windows installer: Release build -> windeployqt -> Inno Setup.
# Usage (from any directory):  powershell -ExecutionPolicy Bypass -File installer\build-installer.ps1
# Output: build\installer\WeirdMath-<version>-setup.exe

$ErrorActionPreference = 'Stop'

$QtDir    = 'C:\Qt\6.12.0\mingw_64'
$QtTools  = 'C:\Qt\Tools'
$Root     = Split-Path -Parent $PSScriptRoot
$BuildDir = Join-Path $Root 'build\Desktop_Qt_6_12_0_MinGW_64_bit-Release'
$Deploy   = Join-Path $Root 'build\deploy\WeirdMath'

$env:PATH = "$QtTools\mingw1310_64\bin;$QtTools\Ninja;$QtTools\CMake_64\bin;$QtDir\bin;" + $env:PATH

# Find the Inno Setup compiler (per-user or machine-wide install).
$Iscc = @(
    "$env:LOCALAPPDATA\Programs\Inno Setup 6\ISCC.exe",
    "${env:ProgramFiles(x86)}\Inno Setup 6\ISCC.exe",
    "$env:ProgramFiles\Inno Setup 6\ISCC.exe"
) | Where-Object { Test-Path $_ } | Select-Object -First 1
if (-not $Iscc) { throw 'Inno Setup 6 not found. Install it: winget install JRSoftware.InnoSetup' }

# Version comes from project() in CMakeLists.txt.
$Version = (Select-String -Path (Join-Path $Root 'CMakeLists.txt') -Pattern 'project\(\S+\s+VERSION\s+([\d.]+)').Matches[0].Groups[1].Value

cmake -S $Root -B $BuildDir -G Ninja "-DCMAKE_PREFIX_PATH=$($QtDir -replace '\\','/')" -DCMAKE_BUILD_TYPE=Release
if ($LASTEXITCODE) { throw 'CMake configure failed' }
cmake --build $BuildDir
if ($LASTEXITCODE) { throw 'Build failed' }

if (Test-Path $Deploy) { Remove-Item -Recurse -Force $Deploy }
New-Item -ItemType Directory -Force $Deploy | Out-Null
Copy-Item (Join-Path $BuildDir 'WeirdMath.exe') $Deploy
windeployqt --release --compiler-runtime --no-translations --no-system-d3d-compiler --no-opengl-sw (Join-Path $Deploy 'WeirdMath.exe')
if ($LASTEXITCODE) { throw 'windeployqt failed' }

& $Iscc "/DAppVersion=$Version" (Join-Path $PSScriptRoot 'WeirdMath.iss')
if ($LASTEXITCODE) { throw 'Inno Setup failed' }
