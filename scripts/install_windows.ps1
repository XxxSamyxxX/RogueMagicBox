Param(
    [switch]$Coverage
)
$ErrorActionPreference = 'Stop'

function Require-Cmd($name) {
  if (-not (Get-Command $name -ErrorAction SilentlyContinue)) {
    Write-Error "Please install $name"
  }
}

Require-Cmd cmake
Require-Cmd git

# Optional
if (-not (Get-Command gh -ErrorAction SilentlyContinue)) {
  Write-Host "[info] gh CLI not found; API will require libcurl at build time or fallback limited"
}

$buildDir = Join-Path (Get-Location) 'build'
New-Item -ItemType Directory -Force -Path $buildDir | Out-Null
Set-Location $buildDir

$coverageFlag = $false
if ($Coverage) { $coverageFlag = $true }

cmake -DCMAKE_BUILD_TYPE=Release -DENABLE_COVERAGE=$coverageFlag ..
cmake --build . --config Release -j 2>$null
ctest -C Release --output-on-failure

# Package
cpack | Out-Null
