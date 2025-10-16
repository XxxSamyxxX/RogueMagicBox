# 🚀 CI/CD Complete Pipeline - Documentation

## 📋 Vue d'ensemble

RogueMagicBox implémente une pipeline CI/CD complète répondant aux exigences du **Défi "CI/CD Express voie 93⁄4"** :

✅ **Tests unitaires** (Catch2)  
✅ **Tests de non-régression** (E2E)  
✅ **Vérification de norme de code** (clang-format, clang-tidy)  
✅ **Test d'image Docker**  
✅ **Compilation multi-plateforme** (Linux, Windows)  
✅ **Analyse SonarQube/SonarCloud**  
✅ **Déploiement automatique** (GitHub Releases, Docker Registry)

---

## 🏗️ Architecture de la Pipeline

```
┌─────────────────────────────────────────────────────────────┐
│                    TRIGGER (push/PR/tag)                     │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│  STAGE 1: Code Quality & Linting                            │
│  ✓ clang-format (norme Google C++ Style)                    │
│  ✓ clang-tidy (analyse statique)                            │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│  STAGE 2: Build & Unit Tests                                │
│  ✓ Compilation (Ubuntu, Windows)                            │
│  ✓ Tests unitaires (Catch2)                                 │
│  ✓ Génération coverage (gcov/llvm-cov)                      │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│  STAGE 3: Regression Tests (E2E)                            │
│  ✓ Tests fonctionnels scan/init/push                        │
│  ✓ Validation outputs et logs                               │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│  STAGE 4: Docker Build & Test                               │
│  ✓ Build multi-stage (Alpine)                               │
│  ✓ Tests de l'image (healthcheck, commands)                 │
│  ✓ Push vers GHCR (si tag)                                  │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│  STAGE 5: SonarCloud Analysis                               │
│  ✓ Analyse qualité du code                                  │
│  ✓ Détection bugs, vulnérabilités, code smells              │
│  ✓ Couverture de code                                       │
└─────────────────────────────────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────┐
│  STAGE 6: Deploy (si toutes étapes OK)                      │
│  ✓ GitHub Release (binaires + artifacts)                    │
│  ✓ Docker image tagged & pushed                             │
│  ✓ Release notes auto-générées                              │
└─────────────────────────────────────────────────────────────┘
```

---

## 📂 Fichiers de Configuration

### `.clang-format`

```yaml
BasedOnStyle: Google
Standard: c++17
IndentWidth: 4
ColumnLimit: 100
BreakBeforeBraces: Allman
```

**Vérification** :

```bash
clang-format --dry-run --Werror src/**/*.cpp include/**/*.hpp
```

### `.clang-tidy`

```yaml
Checks: >
  bugprone-*, cert-*, clang-analyzer-*,
  cppcoreguidelines-*, modernize-*, performance-*,
  readability-*
```

**Exécution** :

```bash
clang-tidy src/**/*.cpp -- -std=c++17 -Iinclude
```

### `Dockerfile`

Image multi-stage Alpine :

1. **Builder** : Compilation avec CMake
2. **Runtime** : Image légère avec binaire uniquement

**Test local** :

```bash
docker build -t roguebox:test .
docker run --rm roguebox:test --help
```

### `sonar-project.properties`

```properties
sonar.projectKey=XxxSamyxxX_RogueMagicBox
sonar.organization=xxxsamyxxx
sonar.sources=src,include
sonar.tests=tests
sonar.cfamily.build-wrapper-output=bw-output
```

---

## 🧪 Tests

### Tests Unitaires (Catch2)

**Fichiers** : `tests/test_*.cpp`

**Modules testés** :

- `test_scanner.cpp` : Scan de workspace, filtres, détection secrets
- `test_gitops.cpp` : Commandes Git, init, remote
- `test_config.cpp` : Parsing configuration

**Exécution locale** :

```bash
cd build
ctest -C Release --output-on-failure --verbose
```

### Tests de Non-Régression (E2E)

**Scripts** :

- `scripts/run_e2e_tests.sh` (Linux)
- `scripts/run_e2e_tests.ps1` (Windows)

**Scénarios** :

1. ✅ Scan basique et avec filtres
2. ✅ Init repo sans remote
3. ✅ Création fichiers standards (.gitignore, .rogueignore)
4. ✅ Commandes help et version
5. ✅ Sécurité dry-run (pas de changements)

**Exécution** :

```bash
# Linux
./scripts/run_e2e_tests.sh

# Windows
.\scripts\run_e2e_tests.ps1
```

---

## 🐳 Docker

### Build de l'image

```bash
docker build -t roguebox:latest .
```

### Tests de l'image

```bash
# Test 1: Version
docker run --rm roguebox:latest --version

# Test 2: Help
docker run --rm roguebox:latest --help

# Test 3: Scan dans container
docker run --rm -v $(pwd):/workspace roguebox:latest scan --root /workspace --dry-run
```

### Push vers GHCR

```bash
docker tag roguebox:latest ghcr.io/xxxsamyxxx/roguebox:latest
docker push ghcr.io/xxxsamyxxx/roguebox:latest
```

---

## 📊 SonarCloud

### Configuration

1. **Créer un projet** sur [SonarCloud](https://sonarcloud.io)

   - Organization: `xxxsamyxxx`
   - Project Key: `XxxSamyxxX_RogueMagicBox`

2. **Générer un token** :

   - Account → Security → Generate Tokens
   - Copier le token

3. **Ajouter secret GitHub** :
   - Repo Settings → Secrets → Actions
   - Nom: `SONAR_TOKEN`
   - Valeur: token copié

### Analyse locale

```bash
# Build avec wrapper
curl -sSLo build-wrapper.zip \
  https://sonarcloud.io/static/cpp/build-wrapper-linux-x86.zip
unzip build-wrapper.zip

# Configure et build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
./build-wrapper-linux-x86/build-wrapper-linux-x86-64 \
  --out-dir bw-output \
  cmake --build build -j

# Scanner
sonar-scanner \
  -Dsonar.projectKey=XxxSamyxxX_RogueMagicBox \
  -Dsonar.organization=xxxsamyxxx \
  -Dsonar.sources=src,include \
  -Dsonar.cfamily.build-wrapper-output=bw-output \
  -Dsonar.host.url=https://sonarcloud.io \
  -Dsonar.token=$SONAR_TOKEN
```

### Métriques surveillées

- **Bugs** : Erreurs logiques
- **Vulnérabilités** : Failles de sécurité
- **Code Smells** : Mauvaises pratiques
- **Coverage** : Couverture de tests
- **Duplications** : Code dupliqué
- **Technical Debt** : Dette technique

---

## 🚀 Déploiement

### Déclenchement

Le déploiement se déclenche automatiquement lors du push d'un **tag Git** :

```bash
git tag -a v1.0.0 -m "Release v1.0.0"
git push origin v1.0.0
```

### Conditions de validation

Le déploiement ne se fait **que si** :

- ✅ Code quality checks passés
- ✅ Build réussi sur toutes les plateformes
- ✅ Tests unitaires passés (100%)
- ✅ Tests E2E passés
- ✅ Image Docker construite et testée
- ✅ Analyse SonarCloud complétée

### Artifacts déployés

**GitHub Release** :

- `roguebox-ubuntu-latest.zip` (binaire Linux)
- `roguebox-windows-latest.zip` (binaire Windows)
- Release notes auto-générées

**Docker Registry** :

- `ghcr.io/xxxsamyxxx/roguebox:latest`
- `ghcr.io/xxxsamyxxx/roguebox:1.0.0` (version tagguée)

---

## 📈 Monitoring et Logs

### GitHub Actions

**Interface** : Repo → Actions → CI/CD Complete Pipeline

**Artifacts disponibles** :

- `roguebox-ubuntu-latest-Release` : Binaire Linux + logs
- `roguebox-windows-latest-Release` : Binaire Windows + logs
- `code-quality-reports` : Rapports clang-format/clang-tidy
- `e2e-test-results` : Logs des tests E2E

### SonarCloud Dashboard

**URL** : https://sonarcloud.io/dashboard?id=XxxSamyxxX_RogueMagicBox

**Vue d'ensemble** :

- Quality Gate status
- Coverage evolution
- New bugs/vulnerabilities
- Technical debt ratio

---

## 🔧 Maintenance

### Mise à jour des dépendances

**Ubuntu** :

```bash
sudo apt-get update
sudo apt-get upgrade build-essential cmake libcurl4-openssl-dev
```

**Docker base image** :

```dockerfile
FROM alpine:3.19  # Mettre à jour version
```

### Ajout de nouveaux tests

**Tests unitaires** :

1. Créer `tests/test_nouveau_module.cpp`
2. Ajouter dans `tests/CMakeLists.txt`
3. Relancer build + tests

**Tests E2E** :

1. Ajouter fonction `test_nouvelle_feature()` dans `scripts/run_e2e_tests.sh`
2. Appeler dans `main()`

### Configuration SonarCloud

**Modifier seuils qualité** :

- SonarCloud → Project Settings → Quality Gates
- Créer custom gate ou modifier "Sonar way"

---

## 📊 Résumé des Critères du Défi

| Critère                  | Implémentation                               | Validation |
| ------------------------ | -------------------------------------------- | ---------- |
| **Tests unitaires**      | Catch2 (`tests/test_*.cpp`)                  | ✅ Stage 2 |
| **Tests non-régression** | Scripts E2E (`scripts/run_e2e_tests.*`)      | ✅ Stage 3 |
| **Norme de code**        | clang-format + clang-tidy (Google Style)     | ✅ Stage 1 |
| **Test Docker**          | Build + exec tests dans CI                   | ✅ Stage 4 |
| **Compilation**          | CMake multi-platform (Linux/Windows)         | ✅ Stage 2 |
| **SonarQube**            | SonarCloud avec build-wrapper                | ✅ Stage 5 |
| **Déploiement**          | GitHub Releases + GHCR (si toutes étapes OK) | ✅ Stage 6 |

---

## 🎯 Prochaines Améliorations

- [ ] Ajout de tests de performance (benchmarks)
- [ ] Intégration Dependabot pour mises à jour auto
- [ ] Notifications Slack/Discord sur deploy
- [ ] Environnements de staging avant prod
- [ ] Rollback automatique si deploy échoue

---

**Documentation complète** : [README.md](../README.md) | [USER_GUIDE.md](USER_GUIDE.md)
