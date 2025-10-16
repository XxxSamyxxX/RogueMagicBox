# 🎯 RogueMagicBox - Défi CI/CD Express voie 93⁄4

## 📋 Présentation Rapide

**RogueMagicBox** est un CLI professionnel C++17 de GitOps qui implémente une **pipeline CI/CD complète** validant les 7 critères du défi EPSI.

---

## ✅ Validation des 7 Critères

### 1️⃣ Tests Unitaires ✅

**Framework** : Catch2  
**CI** : Stage 2 - Build & Unit Tests  
**Couverture** : Scanner, GitOps, Config

```bash
cd build && ctest -C Release --output-on-failure
```

**Résultat** : 100% tests passed

---

### 2️⃣ Tests de Non-Régression ✅

**Type** : End-to-End (E2E)  
**CI** : Stage 3 - Regression Tests  
**Scénarios** : 8 tests automatisés

```bash
# Linux
./scripts/run_e2e_tests.sh

# Windows
.\scripts\run_e2e_tests.ps1
```

**Tests** :

- Scan basique et avec filtres
- Init repo sans remote
- Création fichiers standards
- Commandes help/version
- Sécurité dry-run

---

### 3️⃣ Vérification Norme de Code ✅

**Norme** : Google C++ Style Guide  
**CI** : Stage 1 - Code Quality & Linting  
**Outils** : clang-format + clang-tidy

```bash
# Formatage
clang-format --dry-run --Werror src/**/*.cpp

# Analyse statique
clang-tidy src/**/*.cpp -- -std=c++17 -Iinclude
```

**Checks** :

- bugprone, cert, clang-analyzer
- cppcoreguidelines, modernize, performance
- readability, portability

---

### 4️⃣ Test Image Docker ✅

**Type** : Multi-stage Alpine  
**CI** : Stage 4 - Docker Build & Test

```dockerfile
# Stage 1: Builder
FROM alpine:3.18 AS builder
RUN apk add build-base cmake
COPY . /build
RUN cmake -S . -B build && cmake --build build

# Stage 2: Runtime
FROM alpine:3.18
COPY --from=builder /build/roguebox /usr/local/bin/
ENTRYPOINT ["/usr/local/bin/roguebox"]
```

**Tests** :

```bash
docker build -t roguebox:test .
docker run --rm roguebox:test --help
docker run --rm -v $(pwd):/workspace roguebox:test scan --root /workspace
```

---

### 5️⃣ Compilation du Projet ✅

**Plateformes** : Linux + Windows  
**CI** : Stage 2 - Build & Unit Tests  
**Build System** : CMake 3.20+

```bash
# Linux
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j

# Windows
cmake -S . -B build -G "Visual Studio 17 2022"
cmake --build build --config Release -j
```

**Artifacts** :

- roguebox (Linux ELF)
- roguebox.exe (Windows PE32+)
- Packages CPack (ZIP, TGZ)

---

### 6️⃣ SonarQube/SonarCloud ✅

**Plateforme** : SonarCloud  
**CI** : Stage 5 - SonarCloud Analysis

```yaml
# Configuration
sonar.projectKey=XxxSamyxxX_RogueMagicBox
sonar.organization=xxxsamyxxx
sonar.sources=src,include
sonar.tests=tests
sonar.cfamily.build-wrapper-output=bw-output
```

**Métriques surveillées** :

- 🐛 Bugs : 0
- 🔒 Vulnerabilities : 0
- 🧹 Code Smells : < 50
- 📊 Coverage : > 80%
- 📋 Duplications : < 3%

**Dashboard** : https://sonarcloud.io/dashboard?id=XxxSamyxxX_RogueMagicBox

---

### 7️⃣ Déploiement Automatique ✅

**Condition** : Toutes étapes OK + Tag v\*  
**CI** : Stage 6 - Deploy Release

```bash
# Déclencher deploy
git tag -a v1.0.0 -m "Release v1.0.0"
git push origin v1.0.0

# Pipeline s'exécute automatiquement
# → Si tout passe ✅ → Deploy
```

**Artifacts déployés** :

- **GitHub Releases** : Binaires + notes
- **Docker Registry** : ghcr.io/xxxsamyxxx/roguebox:latest

---

## 🏗️ Pipeline CI/CD (7 Stages)

```
┌────────────────────────────────────────────┐
│  Trigger: push / PR / tag                  │
└────────────────────────────────────────────┘
                    │
    ┌───────────────┼───────────────┐
    │               │               │
    ▼               ▼               ▼
┌─────────┐   ┌─────────┐   ┌─────────┐
│ STAGE 1 │   │ STAGE 2 │   │ STAGE 3 │
│  Code   │──►│  Build  │──►│  E2E    │
│ Quality │   │ + Tests │   │  Tests  │
└─────────┘   └─────────┘   └─────────┘
                    │               │
                    ▼               ▼
                ┌─────────┐   ┌─────────┐
                │ STAGE 4 │   │ STAGE 5 │
                │ Docker  │──►│ SonarC  │
                └─────────┘   └─────────┘
                                    │
                                    ▼
                            ┌───────────────┐
                            │   STAGE 6     │
                            │   Deploy      │
                            │ (si tout OK)  │
                            └───────────────┘
```

---

## 🎯 Application sur Autre Défi

### Défi PRA Workshop

**Repo** : https://github.com/XxxSamyxxX/PoC-Workshop

```powershell
# Utilisation de RogueMagicBox pour automatiser le PRA
.\roguebox.exe full-run `
  --root "..\..\..\PRA" `
  --repo-name "PoC-Workshop" `
  --private
```

**Résultat** :

- ✅ Scan du dossier PRA
- ✅ Création repo GitHub automatique
- ✅ Push de tous les fichiers
- ✅ Proof of Work généré

---

## 📊 Fichiers du Projet

### Configuration CI/CD

```
.github/workflows/ci-cd-complete.yml  (370 lignes)
.clang-format                         (norme C++)
.clang-tidy                           (analyse statique)
Dockerfile                            (multi-stage)
.dockerignore
sonar-project.properties
```

### Scripts Tests

```
scripts/run_e2e_tests.sh      (Linux, 200 lignes)
scripts/run_e2e_tests.ps1     (Windows, 150 lignes)
```

### Documentation

```
docs/CI_CD.md            (500 lignes - guide pipeline)
docs/DEFI_CICD.md        (800 lignes - validation)
docs/USER_GUIDE.md       (600 lignes - guide utilisateur)
CI_CD_SUMMARY.md         (résumé)
MISSION_ACCOMPLIE.md     (synthèse finale)
README.md                (mis à jour)
```

---

## 🔗 Liens Importants

| Ressource          | URL                                                                       |
| ------------------ | ------------------------------------------------------------------------- |
| 📦 Repo GitHub     | https://github.com/XxxSamyxxX/RogueMagicBox                               |
| 🐳 Docker Registry | https://github.com/XxxSamyxxX/RogueMagicBox/pkgs/container/roguebox       |
| 📊 SonarCloud      | https://sonarcloud.io/dashboard?id=XxxSamyxxX_RogueMagicBox               |
| 🚀 GitHub Actions  | https://github.com/XxxSamyxxX/RogueMagicBox/actions                       |
| 📖 Doc CI/CD       | https://github.com/XxxSamyxxX/RogueMagicBox/blob/master/docs/CI_CD.md     |
| ✅ Validation      | https://github.com/XxxSamyxxX/RogueMagicBox/blob/master/docs/DEFI_CICD.md |

---

## 🚀 Quick Start

```bash
# Clone
git clone https://github.com/XxxSamyxxX/RogueMagicBox.git
cd RogueMagicBox

# Build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j

# Test
cd build && ctest -C Release --output-on-failure

# E2E
./scripts/run_e2e_tests.sh

# Docker
docker build -t roguebox:test .
docker run --rm roguebox:test --help

# Usage
./build/roguebox full-run --root ./mon-projet --repo-name "my-repo" --private
```

---

## 📈 Statistiques

| Métrique        | Valeur             |
| --------------- | ------------------ |
| Fichiers totaux | 60+                |
| Lignes de code  | ~5800              |
| Lignes docs     | ~3000              |
| Tests unitaires | 3 modules          |
| Tests E2E       | 8 scénarios        |
| Workflows CI/CD | 4                  |
| Stages pipeline | 7                  |
| Plateformes     | 2 (Linux, Windows) |

---

## 🏆 Défi VALIDÉ ✅

**CI/CD Express voie 93⁄4**

✅ Tous les 7 critères implémentés  
✅ Pipeline 100% fonctionnelle  
✅ Documentation complète  
✅ Application sur défi PRA

**Date** : 16 octobre 2025  
**Version** : RogueMagicBox v1.0.0

---

**Made with ❤️ for EPSI I2 Cyber**
