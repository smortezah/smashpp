param(
  [string]$BuildDir = "",
  [string]$BuildType = "Release",
  [string]$Prefix = "",
  [int]$Parallel = [Environment]::ProcessorCount
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$rootDir = $PSScriptRoot
if ([string]::IsNullOrWhiteSpace($BuildDir)) {
  $BuildDir = Join-Path $rootDir "build"
}
if ([string]::IsNullOrWhiteSpace($Prefix)) {
  $Prefix = Join-Path $rootDir "dist"
}

cmake -S $rootDir -B $BuildDir -DCMAKE_BUILD_TYPE=$BuildType -DCMAKE_INSTALL_PREFIX=$Prefix
cmake --build $BuildDir --parallel $Parallel --config $BuildType
cmake --install $BuildDir --config $BuildType

Write-Host "Installed Smash++ binaries to $Prefix\bin"
