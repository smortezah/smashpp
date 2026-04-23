Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

param(
  [string]$BuildDir = (Join-Path $PSScriptRoot "build"),
  [string]$BuildType = "Release",
  [string]$Prefix = (Join-Path $PSScriptRoot "dist"),
  [int]$Parallel = [Environment]::ProcessorCount
)

$rootDir = $PSScriptRoot

cmake -S $rootDir -B $BuildDir -DCMAKE_BUILD_TYPE=$BuildType -DCMAKE_INSTALL_PREFIX=$Prefix
cmake --build $BuildDir --parallel $Parallel --config $BuildType
cmake --install $BuildDir --config $BuildType

Write-Host "Installed Smash++ binaries to $Prefix\bin"
