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

$configureArgs = @(
  "-S", $rootDir,
  "-B", $BuildDir,
  "-DCMAKE_BUILD_TYPE=$BuildType",
  "-DCMAKE_INSTALL_PREFIX=$Prefix"
)
$buildArgs = @(
  "--build", $BuildDir,
  "--parallel", $Parallel,
  "--config", $BuildType
)
$installArgs = @(
  "--install", $BuildDir,
  "--config", $BuildType,
  "--prefix", $Prefix
)

& cmake @configureArgs
& cmake @buildArgs
& cmake @installArgs

Write-Host "Installed Smash++ binaries to $(Join-Path $Prefix 'bin')"
