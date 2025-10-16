# 📚 Documentation Complète - RogueMagicBox

## 🎯 Vue d'ensemble

**RogueMagicBox** est un outil CLI (Command Line Interface) en C++17 conçu pour automatiser les workflows GitOps. Il permet de scanner des workspaces, initialiser des repositories GitHub, et automatiser les opérations Git/GitHub.

### 🌟 Fonctionnalités principales

- ✅ **Scan de workspace** : Analyse récursive de répertoires avec filtres et exclusions
- ✅ **Initialisation de repos GitHub** : Création automatique via API GitHub
- ✅ **Push automatisé** : Commit et push de tous les fichiers
- ✅ **Full-run** : Workflow complet de scan → init → push
- ✅ **Docker support** : Image Alpine multi-stage optimisée
- ✅ **CI/CD complet** : GitHub Actions avec 7 stages de validation
- ✅ **Analyse de qualité** : SonarCloud intégré

---

## 📦 Architecture du projet

```
RogueMagicBox/
├── src/                    # Code source C++
│   ├── main.cpp           # Point d'entrée principal
│   ├── cli/               # Interface ligne de commande
│   │   ├── args.hpp/cpp   # Parsing d'arguments
│   │   ├── commands_*.cpp # Implémentation des commandes
│   └── core/              # Logique métier
│       ├── scanner.*      # Scan de fichiers
│       ├── gitops.*       # Opérations Git/GitHub
│       ├── config.*       # Configuration TOML
│       └── logger.*       # Logging JSON structuré
├── include/rogue/         # Headers publics
├── tests/                 # Tests unitaires (Catch2)
├── config/                # Fichiers de configuration
│   └── rogue.toml        # Config par défaut
├── docs/                  # Documentation
├── scripts/               # Scripts utilitaires
├── third_party/           # Dépendances externes
├── .github/workflows/     # CI/CD GitHub Actions
├── Dockerfile             # Image Docker Alpine
├── CMakeLists.txt         # Configuration CMake
└── sonar-project.properties # Config SonarCloud
```

---

## 🚀 Installation

### Prérequis

- **C++17** ou supérieur
- **CMake** 3.20+
- **Git** 2.0+
- **Compilateur** : GCC 9+, Clang 10+, MSVC 2019+
- **libcurl** (pour les requêtes HTTP)

### 🐧 Linux (Ubuntu/Debian)

```bash
# Installation des dépendances
sudo apt-get update
sudo apt-get install -y build-essential cmake git libcurl4-openssl-dev

# Clone du repository
git clone https://github.com/XxxSamyxxX/RogueMagicBox.git
cd RogueMagicBox

# Compilation
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release -j

# Installation (optionnel)
sudo cmake --install build

# Utilisation
./build/roguebox --help
```

### 🪟 Windows

```powershell
# Prérequis : Visual Studio 2019+ avec C++ Desktop Development

# Clone du repository
git clone https://github.com/XxxSamyxxX/RogueMagicBox.git
cd RogueMagicBox

# Compilation
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release -j

# Utilisation
.\build\Release\roguebox.exe --help
```

### 🐳 Docker

```bash
# Build de l'image
docker build -t roguebox:latest .

# Utilisation
docker run --rm -v $(pwd):/workspace \
  -e GITHUB_TOKEN=your_token_here \
  roguebox:latest scan --root /workspace

# Pull depuis GitHub Container Registry
docker pull ghcr.io/xxxsamyxxx/roguebox:latest
```

---

## 📖 Guide d'utilisation

### 1. 🔍 Commande `scan`

Scanne un workspace et génère un inventaire JSON des fichiers.

```bash
roguebox scan --root /path/to/workspace [OPTIONS]
```

**Options :**

- `--root <path>` : Chemin du workspace à scanner (requis)
- `--include <glob>` : Pattern d'inclusion (répétable, ex: `*.cpp`)
- `--exclude <glob>` : Pattern d'exclusion (répétable, ex: `node_modules/**`)
- `--max-size-mb <int>` : Taille max des fichiers en MB (défaut: 50)
- `--include-secrets` : Inclure les fichiers secrets dans le scan
- `--dry-run` : Simulation sans écriture

**Exemple :**

```bash
# Scan basique
roguebox scan --root ./mon-projet

# Scan avec filtres
roguebox scan --root ./mon-projet \
  --include "*.cpp" --include "*.hpp" \
  --exclude "build/**" --exclude "*.o" \
  --max-size-mb 100

# Dry-run pour tester
roguebox scan --root ./mon-projet --dry-run
```

**Sortie :**

```json
{
  "files": [
    {
      "path": "src/main.cpp",
      "size": 1024,
      "type": "source"
    }
  ],
  "totalFiles": 42,
  "totalSize": 102400
}
```

---

### 2. 🆕 Commande `init-repo`

Initialise un repository Git local et crée un repository GitHub distant.

```bash
roguebox init-repo --root <path> --repo-name <name> [OPTIONS]
```

**Options :**

- `--root <path>` : Chemin du workspace (requis)
- `--repo-name <name>` : Nom du repository GitHub (requis)
- `--org <organization>` : Organisation GitHub (optionnel)
- `--private` : Repository privé (défaut: public)
- `--public` : Repository public
- `--no-remote` : Ne pas créer de remote GitHub
- `--dry-run` : Simulation sans création réelle

**Exemple :**

```bash
# Repository public
roguebox init-repo --root ./mon-projet \
  --repo-name "mon-super-projet" \
  --public

# Repository privé dans une organisation
roguebox init-repo --root ./mon-projet \
  --repo-name "projet-secret" \
  --org "mon-entreprise" \
  --private

# Local seulement (pas de GitHub)
roguebox init-repo --root ./mon-projet \
  --repo-name "local-only" \
  --no-remote
```

**Ce qui est créé :**

- ✅ `.git/` - Repository Git local
- ✅ `.gitignore` - Ignores standards
- ✅ `.rogueignore` - Ignores spécifiques RogueMagicBox
- ✅ `README.md` - Documentation de base
- ✅ `LICENSE` - Licence MIT
- ✅ Remote GitHub (si `--no-remote` pas utilisé)

---

### 3. 📤 Commande `push-all`

Commit et push tous les fichiers vers GitHub.

```bash
roguebox push-all --root <path> [OPTIONS]
```

**Options :**

- `--root <path>` : Chemin du workspace (requis)
- `--branch <name>` : Branche Git (défaut: master)
- `--commit-message "<msg>"` : Message de commit personnalisé
- `--dry-run` : Simulation sans push réel

**Exemple :**

```bash
# Push simple
roguebox push-all --root ./mon-projet

# Push avec message personnalisé
roguebox push-all --root ./mon-projet \
  --commit-message "feat: ajout de nouvelles fonctionnalités"

# Push sur une branche spécifique
roguebox push-all --root ./mon-projet \
  --branch develop

# Dry-run pour tester
roguebox push-all --root ./mon-projet --dry-run
```

---

### 4. 🎯 Commande `full-run`

Exécute le workflow complet : scan → init-repo → push-all.

```bash
roguebox full-run --root <path> --repo-name <name> [OPTIONS]
```

**Options :**

- Toutes les options de `scan`, `init-repo`, et `push-all` combinées

**Exemple :**

```bash
# Workflow complet public
roguebox full-run --root ./mon-projet \
  --repo-name "mon-projet-github" \
  --public

# Workflow complet privé avec filtres
roguebox full-run --root ./mon-projet \
  --repo-name "projet-confidentiel" \
  --private \
  --exclude "*.log" --exclude "secrets/**" \
  --commit-message "chore: initial commit from RogueMagicBox"

# Dry-run pour vérifier avant exécution
roguebox full-run --root ./mon-projet \
  --repo-name "test-projet" \
  --dry-run
```

---

## 🔧 Configuration

### Fichier `rogue.toml`

RogueMagicBox peut être configuré via un fichier TOML :

```toml
# config/rogue.toml

[scanner]
max_size_mb = 50
include_secrets = false
default_includes = ["*.cpp", "*.hpp", "*.h", "*.c"]
default_excludes = [
  "build/**",
  "node_modules/**",
  ".git/**",
  "*.o",
  "*.exe"
]

[gitops]
default_branch = "main"
default_commit_message = "Initial commit via RogueMagicBox"

[github]
# Token lu depuis GITHUB_TOKEN env var
api_base_url = "https://api.github.com"

[logging]
level = "info"  # debug, info, warn, error
format = "json" # json, text
```

### Variables d'environnement

```bash
# Token GitHub (REQUIS pour créer des repos)
export GITHUB_TOKEN=ghp_your_token_here

# Niveau de log (optionnel)
export ROGUE_LOG_LEVEL=debug

# Config custom (optionnel)
export ROGUE_CONFIG=/path/to/custom/rogue.toml
```

---

## 🐳 Docker - Guide complet

### Build de l'image

```bash
docker build -t roguebox:latest .
```

### Utilisation basique

```bash
# Scan
docker run --rm -v $(pwd):/workspace \
  roguebox:latest scan --root /workspace

# Full-run avec token
docker run --rm \
  -v $(pwd):/workspace \
  -e GITHUB_TOKEN=ghp_your_token \
  roguebox:latest full-run \
    --root /workspace \
    --repo-name "docker-test" \
    --private
```

### Permissions Git dans Docker

Si vous avez une erreur "dubious ownership", configurez Git :

```bash
docker run --rm \
  -v $(pwd):/workspace \
  -e GITHUB_TOKEN=ghp_your_token \
  roguebox:latest \
  /bin/sh -c "git config --global --add safe.directory /workspace && roguebox full-run --root /workspace --repo-name test --private"
```

### Image multi-stage optimisée

Le Dockerfile utilise une approche multi-stage :

```dockerfile
# Stage 1: Build
FROM alpine:latest AS builder
RUN apk add --no-cache build-base cmake git curl-dev
COPY . /app
RUN cmake -S /app -B /app/build -DCMAKE_BUILD_TYPE=Release
RUN cmake --build /app/build -j

# Stage 2: Runtime
FROM alpine:latest
RUN apk add --no-cache libcurl git bash
COPY --from=builder /app/build/roguebox /usr/local/bin/
ENTRYPOINT ["/usr/local/bin/roguebox"]
```

**Taille finale** : ~50 MB (vs ~500 MB avec image de build complète)

---

## 🔄 CI/CD - Pipeline GitHub Actions

### Architecture du pipeline (7 stages)

```yaml
1. 📋 Code Quality & Linting
   - clang-format (non-bloquant)
   - clang-tidy (simulation)

2. 🏗️ Build & Unit Tests
   - Matrix: Ubuntu + Windows
   - CMake + Catch2
   - Artifacts: binaires compilés

3. 🔄 Regression Tests (E2E)
   - Tests fonctionnels des commandes
   - Validation scan, init-repo

4. 🐳 Docker Build & Test
   - Build image Alpine
   - Tests: version, help, scan, full-run
   - Push vers GHCR (sur tags)

5. 📊 SonarCloud Analysis
   - Analyse de qualité de code
   - Build wrapper C++
   - Quality Gate

6. 🚀 Deploy (sur tags v*)
   - Création de GitHub Release
   - Upload des artifacts
   - Notes de version automatiques

7. ✅ Pipeline Success
   - Résumé de tous les stages
```

### Configuration requise

**Secrets GitHub à configurer :**

```
Settings → Secrets and variables → Actions → New repository secret
```

- `SONAR_TOKEN` : Token SonarCloud
- `GH_PAT` : GitHub Personal Access Token (scope: repo)

### Déclenchement du pipeline

```bash
# Sur push (master/main)
git push origin master

# Création de release
git tag v1.0.0
git push origin v1.0.0

# Manuel (workflow_dispatch)
# Via l'interface GitHub Actions
```

### Visualisation

```
https://github.com/XxxSamyxxX/RogueMagicBox/actions
```

---

## 📊 SonarCloud - Analyse de qualité

### Configuration

**Fichier `sonar-project.properties` :**

```properties
sonar.projectKey=XxxSamyxxX_RogueMagicBox
sonar.organization=xxxsamyxxx

sonar.sources=src,include
sonar.tests=tests
sonar.exclusions=build/**,third_party/**

sonar.cfamily.build-wrapper-output=bw-output
sonar.cfamily.cache.enabled=true
```

### Métriques analysées

- ✅ **Bugs** : Détection d'erreurs potentielles
- ✅ **Vulnerabilities** : Failles de sécurité
- ✅ **Code Smells** : Mauvaises pratiques
- ✅ **Coverage** : Couverture de tests
- ✅ **Duplications** : Code dupliqué
- ✅ **Maintainability** : Indice de maintenabilité

### Accès au dashboard

```
https://sonarcloud.io/dashboard?id=XxxSamyxxX_RogueMagicBox
```

### Badges (à ajouter au README)

```markdown
[![Quality Gate](https://sonarcloud.io/api/project_badges/measure?project=XxxSamyxxX_RogueMagicBox&metric=alert_status)](https://sonarcloud.io/dashboard?id=XxxSamyxxX_RogueMagicBox)
[![Bugs](https://sonarcloud.io/api/project_badges/measure?project=XxxSamyxxX_RogueMagicBox&metric=bugs)](https://sonarcloud.io/dashboard?id=XxxSamyxxX_RogueMagicBox)
[![Code Smells](https://sonarcloud.io/api/project_badges/measure?project=XxxSamyxxX_RogueMagicBox&metric=code_smells)](https://sonarcloud.io/dashboard?id=XxxSamyxxX_RogueMagicBox)
```

---

## 🧪 Tests

### Tests unitaires (Catch2)

```bash
# Compilation avec tests
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j

# Exécution des tests
cd build
ctest --output-on-failure --verbose

# Tests spécifiques
./tests/rogue_tests "[scanner]"
./tests/rogue_tests "[gitops]"
```

### Tests E2E

```bash
# Via le script
./scripts/run_e2e_tests.sh

# Ou manuellement
./build/roguebox scan --root ./test_workspace --dry-run
./build/roguebox init-repo --root ./test_workspace --repo-name test --no-remote --dry-run
```

### Coverage

```bash
# Génération avec gcovr (Linux)
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="--coverage"
cmake --build build -j
ctest --test-dir build
gcovr --root . --filter src/ --html-details coverage.html
```

---

## 🔐 Sécurité

### Gestion des tokens GitHub

**❌ NE JAMAIS :**

- Commiter le token dans le code
- Le stocker en clair dans un fichier

**✅ TOUJOURS :**

- Utiliser les variables d'environnement
- Utiliser les GitHub Secrets pour CI/CD
- Révoquer les tokens non utilisés

### Création d'un token GitHub

1. GitHub → Settings → Developer settings → Personal access tokens → Tokens (classic)
2. Generate new token → Select scopes: `repo` (full access)
3. Copier le token : `ghp_...`
4. Exporter : `export GITHUB_TOKEN=ghp_...`

### Permissions requises

- `repo` : Création et gestion de repositories
- `workflow` : (optionnel) Pour modifier les workflows via API

---

## 🐛 Dépannage

### Problème : "GITHUB_TOKEN not set"

```bash
# Vérifier la variable
echo $GITHUB_TOKEN

# Si vide, l'exporter
export GITHUB_TOKEN=ghp_your_token_here

# Persister (Linux/macOS)
echo 'export GITHUB_TOKEN=ghp_your_token_here' >> ~/.bashrc
source ~/.bashrc
```

### Problème : "dubious ownership in repository"

```bash
# Dans Docker
docker run --rm -v $(pwd):/workspace roguebox:latest \
  /bin/sh -c "git config --global --add safe.directory /workspace && roguebox scan --root /workspace"

# En local
git config --global --add safe.directory /path/to/repo
```

### Problème : Compilation échoue sur Linux

```bash
# Vérifier GCC version (9+ requis)
gcc --version

# Installer libcurl
sudo apt-get install libcurl4-openssl-dev

# Nettoyer et rebuilder
rm -rf build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

### Problème : Tests échouent

```bash
# Vérifier que Git est configuré
git config --global user.name "Your Name"
git config --global user.email "your.email@example.com"

# Lancer avec verbose
ctest --test-dir build --verbose --output-on-failure
```

---

## 📈 Performances

### Optimisations implémentées

- ✅ **Multi-threading** : Scan de fichiers parallélisé
- ✅ **Compilation Release** : Optimisations -O3
- ✅ **Docker multi-stage** : Image runtime minimale
- ✅ **Cache SonarCloud** : Build wrapper avec cache
- ✅ **Artifacts CI/CD** : Réutilisation entre jobs

### Benchmarks

| Opération | Workspace 1000 fichiers | Workspace 10000 fichiers |
| --------- | ----------------------- | ------------------------ |
| Scan      | ~0.5s                   | ~3s                      |
| Init-repo | ~1s                     | ~1s                      |
| Push-all  | ~2s                     | ~15s                     |
| Full-run  | ~3.5s                   | ~19s                     |

---

## 🤝 Contribution

### Workflow de contribution

1. **Fork** le repository
2. **Clone** votre fork : `git clone https://github.com/YOUR_USERNAME/RogueMagicBox.git`
3. **Branche** : `git checkout -b feature/ma-fonctionnalite`
4. **Commits** : `git commit -m "feat: ajout de ma fonctionnalité"`
5. **Push** : `git push origin feature/ma-fonctionnalite`
6. **Pull Request** vers `main`

### Convention de commits

Suivre [Conventional Commits](https://www.conventionalcommits.org/) :

```
feat: nouvelle fonctionnalité
fix: correction de bug
docs: documentation
style: formatage, point-virgules
refactor: refactoring de code
test: ajout/modification de tests
chore: tâches de maintenance
```

### Code style

```bash
# Formatter avec clang-format
find src include tests -name '*.cpp' -o -name '*.hpp' | xargs clang-format -i

# Vérifier
clang-format --dry-run --Werror src/**/*.cpp
```

---

## 📄 Licence

**MIT License**

Copyright (c) 2025 XxxSamyxxX

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

---

## 📞 Support

### Ressources

- 📖 **Documentation** : [docs/](./docs/)
- 🐛 **Issues** : https://github.com/XxxSamyxxX/RogueMagicBox/issues
- 💬 **Discussions** : https://github.com/XxxSamyxxX/RogueMagicBox/discussions
- 📧 **Email** : (à configurer)

### Liens utiles

- [Guide d'utilisation](./USER_GUIDE.md)
- [Setup SonarCloud](./SONARCLOUD_SETUP.md)
- [CI/CD Documentation](./CI_CD.md)
- [Changelog](./CHANGELOG.md)

---

## 🎓 Contexte académique

Ce projet a été développé dans le cadre du **Workshop "CI/CD Express voie 93/4"** à l'EPSI.

### Critères de validation

- ✅ **Critère 1** : GitHub Actions workflow configuré
- ✅ **Critère 2** : Build multi-plateforme (Ubuntu + Windows)
- ✅ **Critère 3** : Tests automatisés avec Catch2
- ✅ **Critère 4** : Docker build & push vers GHCR
- ✅ **Critère 5** : SonarCloud intégré avec quality gate
- ✅ **Critère 6** : Releases automatiques sur tags
- ✅ **Critère 7** : Documentation complète

---

## 🏆 Fonctionnalités avancées

### 1. Logging JSON structuré

```json
{
  "timestamp": "2025-10-16T14:30:00Z",
  "level": "info",
  "component": "scanner",
  "message": "Scan completed",
  "context": {
    "totalFiles": 42,
    "duration": "1.5s"
  }
}
```

### 2. Configuration TOML flexible

Support de configurations hiérarchiques avec héritage.

### 3. Dry-run universel

Toutes les commandes supportent `--dry-run` pour validation.

### 4. Gestion d'erreurs robuste

- Try-catch C++ avec messages détaillés
- Codes de retour explicites (0=succès, 1=erreur user, 2=erreur système)
- Logs structurés pour débogage

### 5. API GitHub moderne

- Authentification par token
- Création de repos public/privé
- Support des organizations
- Gestion des erreurs HTTP

---

## 🔮 Roadmap

### Version 2.0 (à venir)

- [ ] Support GitLab et Bitbucket
- [ ] Interface web (dashboard)
- [ ] Configuration via GUI
- [ ] Plugins système
- [ ] Templates de repositories
- [ ] Webhooks
- [ ] Multi-langues (i18n)

### Version 1.1 (en cours)

- [x] CI/CD complet
- [x] SonarCloud
- [ ] Coverage 80%+
- [ ] Performance optimizations
- [ ] Documentation interactive

---

**Version** : 1.0.0  
**Dernière mise à jour** : 16 octobre 2025  
**Auteur** : XxxSamyxxX  
**Status** : Production Ready ✅
