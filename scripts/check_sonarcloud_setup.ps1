# Script de verification de la configuration SonarCloud
# Usage: .\scripts\check_sonarcloud_setup.ps1

$ErrorActionPreference = "Continue"

Write-Host "`n==================================================================" -ForegroundColor Cyan
Write-Host " Verification de la configuration SonarCloud pour RogueMagicBox" -ForegroundColor Cyan
Write-Host "==================================================================`n" -ForegroundColor Cyan

$script:Errors = 0
$script:Warnings = 0

# Fonctions utilitaires
function Test-Condition {
    param(
        [bool]$Condition,
        [string]$Message
    )
    
    if ($Condition) {
        Write-Host "[OK]   $Message" -ForegroundColor Green
    }
    else {
        Write-Host "[FAIL] $Message" -ForegroundColor Red
        $script:Errors++
    }
}

function Show-Warning {
    param([string]$Message)
    Write-Host "[WARN] $Message" -ForegroundColor Yellow
    $script:Warnings++
}

function Show-Info {
    param([string]$Message)
    Write-Host "[INFO] $Message" -ForegroundColor Cyan
}

# 1. Verifier que sonar-project.properties existe
Write-Host "1. Verification du fichier de configuration..." -ForegroundColor White
if (Test-Path "sonar-project.properties") {
    Test-Condition $true "sonar-project.properties existe"
    
    $content = Get-Content "sonar-project.properties" -Raw
    
    Test-Condition ($content -match "sonar.projectKey=XxxSamyxxX_RogueMagicBox") "sonar.projectKey est configure"
    Test-Condition ($content -match "sonar.organization=xxxsamyxxx") "sonar.organization est configure"
    Test-Condition ($content -match "sonar.sources=") "sonar.sources est configure"
}
else {
    Test-Condition $false "sonar-project.properties n'existe pas"
}

Write-Host ""

# 2. Verifier que les dossiers sources existent
Write-Host "2. Verification des dossiers sources..." -ForegroundColor White
Test-Condition (Test-Path "src") "Dossier src/ existe"

if (Test-Path "include") {
    Test-Condition $true "Dossier include/ existe"
}
else {
    Show-Warning "Dossier include/ n'existe pas (optionnel si headers dans src/)"
}

if (Test-Path "tests") {
    Test-Condition $true "Dossier tests/ existe"
}
else {
    Show-Warning "Dossier tests/ n'existe pas"
}

Write-Host ""

# 3. Verifier que le workflow GitHub Actions existe
Write-Host "3. Verification du workflow GitHub Actions..." -ForegroundColor White
if (Test-Path ".github\workflows\ci-cd-complete.yml") {
    Test-Condition $true "Workflow CI/CD existe"
    
    $workflow = Get-Content ".github\workflows\ci-cd-complete.yml" -Raw
    
    Test-Condition ($workflow -match "sonarcloud-analysis:") "Stage SonarCloud present dans le workflow"
    Test-Condition ($workflow -match "SONAR_TOKEN") "Reference a SONAR_TOKEN presente"
}
else {
    Test-Condition $false "Workflow CI/CD n'existe pas"
}

Write-Host ""

# 4. Verifier la connexion a SonarCloud (si token disponible)
Write-Host "4. Verification de la connexion SonarCloud..." -ForegroundColor White
$sonarToken = $env:SONAR_TOKEN

if ($sonarToken) {
    Show-Info "SONAR_TOKEN detecte dans l'environnement"
    
    try {
        $base64Auth = [Convert]::ToBase64String([Text.Encoding]::ASCII.GetBytes("${sonarToken}:"))
        $headers = @{
            Authorization = "Basic $base64Auth"
        }
        
        $response = Invoke-WebRequest -Uri "https://sonarcloud.io/api/components/show?component=XxxSamyxxX_RogueMagicBox" `
            -Headers $headers -Method Get -ErrorAction Stop
        
        if ($response.StatusCode -eq 200) {
            Test-Condition $true "Projet accessible sur SonarCloud"
        }
    }
    catch {
        $statusCode = $_.Exception.Response.StatusCode.value__
        if ($statusCode -eq 401) {
            Test-Condition $false "Token invalide ou expire"
        }
        elseif ($statusCode -eq 404) {
            Test-Condition $false "Projet non trouve sur SonarCloud (verifier projectKey)"
        }
        else {
            Test-Condition $false "Erreur HTTP $statusCode lors de la connexion"
        }
    }
}
else {
    Show-Warning "SONAR_TOKEN non defini dans l'environnement (normal en local)"
    Show-Info "Le token est requis uniquement dans GitHub Actions"
    Show-Info "Pour tester: `$env:SONAR_TOKEN='ton_token'"
}

Write-Host ""

# 5. Verifier si build-wrapper est telecharge
Write-Host "5. Verification des outils SonarCloud..." -ForegroundColor White
if (Test-Path "build-wrapper-linux-x86\build-wrapper-linux-x86-64") {
    Test-Condition $true "Build wrapper C++ telecharge"
}
else {
    Show-Warning "Build wrapper non telecharge (sera telecharge par la CI)"
    Show-Info "Pour telecharger manuellement:"
    Show-Info "  Invoke-WebRequest -Uri 'https://sonarcloud.io/static/cpp/build-wrapper-linux-x86.zip' -OutFile 'build-wrapper.zip'"
    Show-Info "  Expand-Archive -Path 'build-wrapper.zip' -DestinationPath '.'"
}

Write-Host ""

# 6. Verifier si le scanner SonarCloud est installe (optionnel)
Write-Host "6. Verification du scanner SonarCloud (optionnel)..." -ForegroundColor White
try {
    $sonarScannerPath = Get-Command sonar-scanner -ErrorAction Stop
    Test-Condition $true "sonar-scanner installe"
    Show-Info "Chemin: $($sonarScannerPath.Source)"
}
catch {
    Show-Warning "sonar-scanner non installe (pas requis, GitHub Actions l'utilise)"
    Show-Info "Pour installer: https://docs.sonarcloud.io/advanced-setup/ci-based-analysis/sonarscanner-cli/"
}

Write-Host ""

# 7. Resume
Write-Host "==================================================================" -ForegroundColor Cyan
Write-Host " RESUME DE LA VERIFICATION" -ForegroundColor Cyan
Write-Host "==================================================================`n" -ForegroundColor Cyan

if ($script:Errors -eq 0 -and $script:Warnings -eq 0) {
    Write-Host "Configuration SonarCloud complete et correcte !`n" -ForegroundColor Green
    Write-Host "Prochaines etapes :"
    Write-Host "   1. Creer un compte sur https://sonarcloud.io"
    Write-Host "   2. Importer le projet RogueMagicBox"
    Write-Host "   3. Copier le SONAR_TOKEN"
    Write-Host "   4. Ajouter le secret SONAR_TOKEN dans GitHub (Settings -> Secrets -> Actions)"
    Write-Host "   5. Pusher un commit pour declencher l'analyse`n"
    Write-Host "Guide complet: docs\SONARCLOUD_SETUP.md`n" -ForegroundColor Cyan
    exit 0
}
elseif ($script:Errors -eq 0) {
    Write-Host "Configuration SonarCloud OK avec $($script:Warnings) avertissement(s)`n" -ForegroundColor Yellow
    Write-Host "Les avertissements sont generalement non bloquants."
    Write-Host "Consulte le guide pour plus d'infos: docs\SONARCLOUD_SETUP.md`n"
    exit 0
}
else {
    Write-Host "$($script:Errors) erreur(s) detectee(s) dans la configuration" -ForegroundColor Red
    Write-Host "$($script:Warnings) avertissement(s)`n" -ForegroundColor Yellow
    Write-Host "Corrige les erreurs avant de continuer."
    Write-Host "Consulte le guide: docs\SONARCLOUD_SETUP.md`n"
    exit 1
}
