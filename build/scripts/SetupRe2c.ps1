# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT License.

[CmdletBinding()]
param([string]$Destination = (Join-Path $PSScriptRoot '..\..\Tools\re2c\4.6'))
$ErrorActionPreference = 'Stop'
$Destination = [IO.Path]::GetFullPath($Destination)
$exe = Join-Path $Destination 'bin\re2c.exe'
if (Test-Path $exe) {
    $version = & $exe --version
    if ($LASTEXITCODE -ne 0 -or $version -ne 're2c 4.6') { throw "Unexpected re2c version at $exe" }
    Write-Output $exe
    return
}
$cmake = Get-Command cmake -ErrorAction SilentlyContinue
if (-not $cmake) {
    $vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
    $vs = & $vswhere -latest -products '*' -property installationPath
    $cmakePath = Join-Path $vs 'Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe'
} else { $cmakePath = $cmake.Source }
if (-not (Test-Path $cmakePath)) { throw 'Install the Visual Studio C++ CMake tools before setting up re2c.' }
New-Item -ItemType Directory -Force -Path $Destination | Out-Null
$archive = Join-Path $Destination 're2c-4.6.tar.xz'
Invoke-WebRequest 'https://github.com/skvadrik/re2c/releases/download/4.6/re2c-4.6.tar.xz' -OutFile $archive
if ((Get-FileHash $archive -Algorithm SHA256).Hash -ne '75BF2696445E831D0D44E0D9F2909EEFFC18C09757F222B2FB025F7E59FE130B') {
    throw 're2c source archive checksum mismatch.'
}
& tar -xf $archive -C $Destination
if ($LASTEXITCODE -ne 0) { throw 'Cannot extract re2c source.' }
$source = Join-Path $Destination 're2c-4.6'
$build = Join-Path $Destination 'build'
& $cmakePath -S $source -B $build -G 'Visual Studio 18 2026' -A x64 -DCMAKE_INSTALL_PREFIX="$Destination" -DRE2C_BUILD_RE2GO=OFF -DRE2C_BUILD_RE2RUST=OFF
if ($LASTEXITCODE -ne 0) { throw 're2c configuration failed.' }
& $cmakePath --build $build --config Release --target re2c --parallel
if ($LASTEXITCODE -ne 0) { throw 're2c build failed.' }
New-Item -ItemType Directory -Force -Path (Split-Path $exe) | Out-Null
Copy-Item (Join-Path $build 're2c.exe') $exe
if ((& $exe --version) -ne 're2c 4.6') { throw 'Built re2c version mismatch.' }
Write-Output $exe
