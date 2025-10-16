#!/bin/bash
# Script de vérification de la configuration SonarCloud
# Usage: ./scripts/check_sonarcloud_setup.sh

set -e

echo "🔍 Vérification de la configuration SonarCloud pour RogueMagicBox"
echo "=================================================================="
echo ""

# Couleurs
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

ERRORS=0
WARNINGS=0

# Fonction de vérification
check() {
    if [ $1 -eq 0 ]; then
        echo -e "${GREEN}✅ $2${NC}"
    else
        echo -e "${RED}❌ $2${NC}"
        ((ERRORS++))
    fi
}

warn() {
    echo -e "${YELLOW}⚠️  $1${NC}"
    ((WARNINGS++))
}

info() {
    echo -e "ℹ️  $1"
}

# 1. Vérifier que sonar-project.properties existe
echo "1️⃣  Vérification du fichier de configuration..."
if [ -f "sonar-project.properties" ]; then
    check 0 "sonar-project.properties existe"
    
    # Vérifier les propriétés obligatoires
    if grep -q "sonar.projectKey=XxxSamyxxX_RogueMagicBox" sonar-project.properties; then
        check 0 "sonar.projectKey est configuré"
    else
        check 1 "sonar.projectKey est manquant ou incorrect"
    fi
    
    if grep -q "sonar.organization=xxxsamyxxx" sonar-project.properties; then
        check 0 "sonar.organization est configuré"
    else
        check 1 "sonar.organization est manquant ou incorrect"
    fi
    
    if grep -q "sonar.sources=" sonar-project.properties; then
        check 0 "sonar.sources est configuré"
    else
        check 1 "sonar.sources est manquant"
    fi
else
    check 1 "sonar-project.properties n'existe pas"
fi

echo ""

# 2. Vérifier que les dossiers sources existent
echo "2️⃣  Vérification des dossiers sources..."
if [ -d "src" ]; then
    check 0 "Dossier src/ existe"
else
    check 1 "Dossier src/ n'existe pas"
fi

if [ -d "include" ]; then
    check 0 "Dossier include/ existe"
else
    warn "Dossier include/ n'existe pas (optionnel si headers dans src/)"
fi

if [ -d "tests" ]; then
    check 0 "Dossier tests/ existe"
else
    warn "Dossier tests/ n'existe pas"
fi

echo ""

# 3. Vérifier que le workflow GitHub Actions existe
echo "3️⃣  Vérification du workflow GitHub Actions..."
if [ -f ".github/workflows/ci-cd-complete.yml" ]; then
    check 0 "Workflow CI/CD existe"
    
    if grep -q "sonarcloud-analysis:" ".github/workflows/ci-cd-complete.yml"; then
        check 0 "Stage SonarCloud présent dans le workflow"
    else
        check 1 "Stage SonarCloud manquant dans le workflow"
    fi
    
    if grep -q "SONAR_TOKEN" ".github/workflows/ci-cd-complete.yml"; then
        check 0 "Référence à SONAR_TOKEN présente"
    else
        check 1 "Référence à SONAR_TOKEN manquante"
    fi
else
    check 1 "Workflow CI/CD n'existe pas"
fi

echo ""

# 4. Vérifier la connexion à SonarCloud (si token disponible)
echo "4️⃣  Vérification de la connexion SonarCloud..."
if [ ! -z "$SONAR_TOKEN" ]; then
    info "SONAR_TOKEN détecté dans l'environnement"
    
    # Tester la connexion avec l'API SonarCloud
    HTTP_CODE=$(curl -s -o /dev/null -w "%{http_code}" \
        -u "$SONAR_TOKEN:" \
        "https://sonarcloud.io/api/components/show?component=XxxSamyxxX_RogueMagicBox")
    
    if [ "$HTTP_CODE" -eq 200 ]; then
        check 0 "Projet accessible sur SonarCloud"
    elif [ "$HTTP_CODE" -eq 401 ]; then
        check 1 "Token invalide ou expiré"
    elif [ "$HTTP_CODE" -eq 404 ]; then
        check 1 "Projet non trouvé sur SonarCloud (vérifier projectKey)"
    else
        check 1 "Erreur HTTP $HTTP_CODE lors de la connexion"
    fi
else
    warn "SONAR_TOKEN non défini dans l'environnement (normal en local)"
    info "Le token est requis uniquement dans GitHub Actions"
    info "Pour tester: export SONAR_TOKEN=ton_token"
fi

echo ""

# 5. Vérifier si build-wrapper est téléchargé
echo "5️⃣  Vérification des outils SonarCloud..."
if [ -f "build-wrapper-linux-x86/build-wrapper-linux-x86-64" ]; then
    check 0 "Build wrapper C++ téléchargé"
else
    warn "Build wrapper non téléchargé (sera téléchargé par la CI)"
    info "Pour télécharger manuellement:"
    info "  curl -sSLo build-wrapper-linux-x86.zip https://sonarcloud.io/static/cpp/build-wrapper-linux-x86.zip"
    info "  unzip build-wrapper-linux-x86.zip"
fi

echo ""

# 6. Vérifier si le scanner SonarCloud est installé (optionnel)
echo "6️⃣  Vérification du scanner SonarCloud (optionnel)..."
if command -v sonar-scanner &> /dev/null; then
    check 0 "sonar-scanner installé"
    info "Version: $(sonar-scanner --version | head -n 1)"
else
    warn "sonar-scanner non installé (pas requis, GitHub Actions l'utilise)"
    info "Pour installer: https://docs.sonarcloud.io/advanced-setup/ci-based-analysis/sonarscanner-cli/"
fi

echo ""

# 7. Résumé
echo "=================================================================="
echo "📊 RÉSUMÉ DE LA VÉRIFICATION"
echo "=================================================================="

if [ $ERRORS -eq 0 ] && [ $WARNINGS -eq 0 ]; then
    echo -e "${GREEN}✅ Configuration SonarCloud complète et correcte !${NC}"
    echo ""
    echo "🚀 Prochaines étapes :"
    echo "   1. Créer un compte sur https://sonarcloud.io"
    echo "   2. Importer le projet RogueMagicBox"
    echo "   3. Copier le SONAR_TOKEN"
    echo "   4. Ajouter le secret SONAR_TOKEN dans GitHub (Settings → Secrets → Actions)"
    echo "   5. Pusher un commit pour déclencher l'analyse"
    echo ""
    echo "📚 Guide complet: docs/SONARCLOUD_SETUP.md"
    exit 0
elif [ $ERRORS -eq 0 ]; then
    echo -e "${YELLOW}⚠️  Configuration SonarCloud OK avec $WARNINGS avertissement(s)${NC}"
    echo ""
    echo "Les avertissements sont généralement non bloquants."
    echo "Consulte le guide pour plus d'infos: docs/SONARCLOUD_SETUP.md"
    exit 0
else
    echo -e "${RED}❌ $ERRORS erreur(s) détectée(s) dans la configuration${NC}"
    echo -e "${YELLOW}⚠️  $WARNINGS avertissement(s)${NC}"
    echo ""
    echo "Corrige les erreurs avant de continuer."
    echo "Consulte le guide: docs/SONARCLOUD_SETUP.md"
    exit 1
fi
