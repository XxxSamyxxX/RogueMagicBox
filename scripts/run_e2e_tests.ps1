# Script PowerShell de tests E2E pour Windows
# Tests de non-régression automatisés

param(
    [string]$RogueBoxPath = ".\build\Release\roguebox.exe",
    [string]$TestDir = ".\e2e_tests_workspace"
)

$ErrorActionPreference = "Continue"
$Global:FailedTests = 0
$LogFile = "e2e_tests.log"

function Log-Info {
    param([string]$Message)
    Write-Host "[INFO] $Message" -ForegroundColor Green
    "$Message" | Out-File -FilePath $LogFile -Append
}

function Log-Error {
    param([string]$Message)
    Write-Host "[ERROR] $Message" -ForegroundColor Red
    "$Message" | Out-File -FilePath $LogFile -Append
}

function Log-Warn {
    param([string]$Message)
    Write-Host "[WARN] $Message" -ForegroundColor Yellow
    "$Message" | Out-File -FilePath $LogFile -Append
}

function Setup-TestEnvironment {
    Log-Info "Setting up E2E test environment..."
    
    if (Test-Path $TestDir) {
        Remove-Item -Recurse -Force $TestDir
    }
    
    New-Item -ItemType Directory -Path $TestDir | Out-Null
    
    # Create sample files
    "# Test README" | Out-File "$TestDir\README.md"
    New-Item -ItemType Directory -Path "$TestDir\src" -Force | Out-Null
    "int main() { return 0; }" | Out-File "$TestDir\src\main.cpp"
    New-Item -ItemType Directory -Path "$TestDir\docs" -Force | Out-Null
    "# Documentation" | Out-File "$TestDir\docs\guide.md"
}

function Test-ScanBasic {
    Log-Info "TEST 1: Basic scan command"
    
    & $RogueBoxPath scan --root $TestDir --dry-run
    
    if ($LASTEXITCODE -eq 0) {
        Log-Info "✅ TEST 1 PASSED"
        return $true
    }
    else {
        Log-Error "❌ TEST 1 FAILED"
        $Global:FailedTests++
        return $false
    }
}

function Test-ScanFilters {
    Log-Info "TEST 2: Scan with filters"
    
    & $RogueBoxPath scan --root $TestDir --exclude "*.log" --dry-run
    
    if ($LASTEXITCODE -eq 0) {
        Log-Info "✅ TEST 2 PASSED"
        return $true
    }
    else {
        Log-Error "❌ TEST 2 FAILED"
        $Global:FailedTests++
        return $false
    }
}

function Test-InitNoRemote {
    Log-Info "TEST 3: Init repo without remote"
    
    & $RogueBoxPath init-repo --root $TestDir --repo-name "e2e-test" --no-remote --dry-run
    
    if ($LASTEXITCODE -eq 0) {
        Log-Info "✅ TEST 3 PASSED"
        return $true
    }
    else {
        Log-Error "❌ TEST 3 FAILED"
        $Global:FailedTests++
        return $false
    }
}

function Test-StandardFilesCreation {
    Log-Info "TEST 4: Standard files creation"
    
    & $RogueBoxPath init-repo --root $TestDir --repo-name "e2e-test" --no-remote | Out-Null
    
    $gitignoreExists = Test-Path "$TestDir\.gitignore"
    $rogueignoreExists = Test-Path "$TestDir\.rogueignore"
    
    if ($gitignoreExists -and $rogueignoreExists) {
        Log-Info "✅ TEST 4 PASSED"
        return $true
    }
    else {
        Log-Error "❌ TEST 4 FAILED"
        $Global:FailedTests++
        return $false
    }
}

function Test-HelpCommand {
    Log-Info "TEST 5: Help command"
    
    $output = & $RogueBoxPath --help 2>&1 | Out-String
    
    if ($output -match "scan|init-repo|push-all|full-run") {
        Log-Info "✅ TEST 5 PASSED"
        return $true
    }
    else {
        Log-Error "❌ TEST 5 FAILED"
        $Global:FailedTests++
        return $false
    }
}

function Cleanup {
    Log-Info "Cleaning up test workspace..."
    if (Test-Path $TestDir) {
        Remove-Item -Recurse -Force $TestDir -ErrorAction SilentlyContinue
    }
}

# Main execution
Log-Info "========================================="
Log-Info "RogueMagicBox E2E Regression Tests (Windows)"
Log-Info "========================================="

if (-not (Test-Path $RogueBoxPath)) {
    Log-Error "RogueMagicBox binary not found at: $RogueBoxPath"
    exit 1
}

Setup-TestEnvironment

# Run tests
Test-ScanBasic
Test-ScanFilters
Test-InitNoRemote
Test-StandardFilesCreation
Test-HelpCommand

Cleanup

Log-Info "========================================="
if ($Global:FailedTests -eq 0) {
    Log-Info "✅ ALL TESTS PASSED"
    Log-Info "========================================="
    exit 0
}
else {
    Log-Error "❌ $Global:FailedTests TEST(S) FAILED"
    Log-Info "========================================="
    exit 1
}
