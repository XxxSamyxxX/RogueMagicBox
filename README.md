# 🎯 RogueMagicBox

**CLI professionnel de GitOps pour l'automatisation Workshop → GitHub**

[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.cppreference.com/w/cpp/17)
[![CMake](https://img.shields.io/badge/CMake-3.20+-green.svg)](https://cmake.org/)
[![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20Linux-lightgrey.svg)]()
[![License](https://img.shields.io/badge/license-MIT-orange.svg)](LICENSE)
[![CI/CD](https://img.shields.io/badge/CI%2FCD-Complete-success.svg)](docs/CI_CD.md)
[![Docker](https://img.shields.io/badge/Docker-Available-2496ED?logo=docker&logoColor=white)](https://github.com/XxxSamyxxX/RogueMagicBox/pkgs/container/roguebox)
[![SonarCloud](https://img.shields.io/badge/SonarCloud-Integrated-4E9BCD?logo=sonarcloud&logoColor=white)](https://sonarcloud.io/dashboard?id=XxxSamyxxX_RogueMagicBox)

## 📖 Vue d'ensemble

**RogueMagicBox** est un outil CLI complet en C++17 conçu pour automatiser la gestion de projets Workshop :

- 🔍 **Scan** et inventaire intelligent de dossiers
- 🔒 **Détection automatique** de fichiers sensibles (tokens, clés, secrets)
- 🌐 **Création de repos GitHub** (API + gh CLI fallback)
- 🚀 **Push automatisé** avec chunking pour gros projets
- 📋 **Proof of Work** JSON structuré avec logs détaillés

### 🎯 Cas d'usage principaux

- **Workshops EPSI/Écoles** : Initialisation rapide de PRA/projets et push GitHub sécurisé
- **DevOps/CI** : Automatisation de synchronisation de workspaces vers repos distants
- **Audit & Compliance** : Inventaires JSON horodatés avec hashes pour traçabilité

---

## ✨ Fonctionnalités

### 🔍 Scan & Analyse

- ✅ Inventaire récursif avec métadonnées (size, hash, mtime)
- ✅ Filtrage via `.rogueignore` (patterns glob)
- ✅ Détection de secrets (patterns : `token`, `password`, `api_key`, `.env`, etc.)
- ✅ Limite de taille configurable (défaut: 50 MB par fichier)
- ✅ Sortie JSON structurée

### 🌐 GitHub Integration

- ✅ Création de repos via **GitHub API** (libcurl) ou **gh CLI** (fallback)
- ✅ Support org/user, repos privés/publics
- ✅ Configuration automatique des remotes
- ✅ Authentification via `GITHUB_TOKEN` ou `gh auth`

### 🚀 Git Automation

- ✅ Initialisation repo avec fichiers standards (`.gitignore`, `LICENSE`, `README.md`)
- ✅ Commit sémantique automatique avec timestamps
- ✅ **Chunking intelligent** pour repos >100 MB (batches ~50 MB)
- ✅ Skip automatique des fichiers >50 MB avec warnings
- ✅ Gestion idempotente ("nothing to commit" → continue push)

### 🔒 Sécurité

- ✅ `.rogueignore` par défaut (tokens, clés, secrets, médias lourds)
- ✅ Masquage des secrets dans les logs
- ✅ Warnings sur inclusion de fichiers sensibles
- ✅ Option `--include-secrets` explicite pour forcer (déconseillé)

### 📊 Observabilité

- ✅ Logs JSON structurés (`logs/rogue.log`)
- ✅ Proof of Work Markdown + JSON (`docs/PROOF_OF_WORK.md`)
- ✅ Mode `--dry-run` pour simulation sans effets de bord
- ✅ Codes de sortie standardisés pour CI/CD

---

## 🚀 Installation

### Prérequis

**Obligatoires** :

- **C++17** compatible compiler (MSVC 17+, GCC 9+, Clang 10+)
- **CMake** 3.20+
- **Git** 2.30+

**Optionnels** :

- **GitHub CLI** (`gh`) : Pour création de repos sans token
- **libcurl** : Pour GitHub API (sinon fallback vers gh)

### 🪟 Windows (PowerShell)

```powershell
# 1. Cloner le projet
git clone https://github.com/votre-org/RogueMagicBox.git
cd RogueMagicBox

# 2. Configurer et builder
cmake -S . -B build -G "Visual Studio 17 2022"
cmake --build build --config Release -j 8

# 3. Exécutable disponible
.\build\Release\roguebox.exe --help
```

### 🐧 Linux / macOS

```bash
# 1. Cloner
git clone https://github.com/votre-org/RogueMagicBox.git
cd RogueMagicBox

# 2. Installer dépendances (Ubuntu/Debian)
sudo apt update
sudo apt install -y build-essential cmake git libcurl4-openssl-dev

# 3. Builder
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j $(nproc)

# 4. Exécutable
./build/roguebox --help
```

### 📦 Installation système (optionnel)

```bash
# Linux/macOS
sudo cmake --install build

# Windows (Admin PowerShell)
cmake --install build --prefix "C:\Program Files\RogueMagicBox"
```

---

## 🎯 Utilisation Rapide

### 1️⃣ Scan d'un dossier Workshop

```powershell
roguebox.exe scan --root "C:\Workshop\PRA"
```

**Sortie** :

```json
{
  "files": [
    {
      "path": "README.md",
      "size": 1234,
      "hash": "abc...",
      "mtime": "2025-10-15T10:00:00"
    },
    {
      "path": "src/main.cpp",
      "size": 5678,
      "hash": "def...",
      "mtime": "2025-10-15T09:30:00"
    }
  ],
  "total_size": 45678,
  "generated_at": "2025-10-15T11:00:00",
  "root": "C:\\Workshop\\PRA"
}
```

### 2️⃣ Initialiser Git + GitHub

```powershell
roguebox.exe init-repo `
  --root "C:\Workshop\PRA" `
  --repo-name "epsi-pra-2025" `
  --private `
  --org "MonEcole"
```

**Résultat** :

- ✅ Repo Git local créé
- ✅ Fichiers `.gitignore`, `.rogueignore`, `LICENSE`, `README.md` générés
- ✅ Repo GitHub `MonEcole/epsi-pra-2025` (privé) créé
- ✅ Remote `origin` configuré

### 3️⃣ Commit & Push

```powershell
roguebox.exe push-all --root "C:\Workshop\PRA"
```

**Actions** :

- `git add -A`
- `git commit -m "chore(import): add Workshop sources & docs (2025-10-15)"`
- `git push -u origin main`

### 4️⃣ Pipeline complet (Recommandé ⭐)

```powershell
roguebox.exe full-run `
  --root "C:\Workshop\PRA" `
  --repo-name "epsi-pra-final" `
  --private `
  --org "MonEcole"
```

**Étapes automatisées** :

1. 🔍 Scan + inventaire JSON
2. 📁 Init repo local + standards
3. 🌐 Création repo GitHub
4. 🔗 Configuration remote
5. 📦 Commit de tout
6. 🚀 Push vers GitHub
7. 📝 Génération `docs/PROOF_OF_WORK.md`

---

## 📚 Documentation Complète

- 📖 **[Guide Utilisateur](docs/USER_GUIDE.md)** : Toutes les commandes détaillées
- 🏗️ **[Architecture](docs/ARCHITECTURE.md)** : Design interne, modules, API
- ⚙️ **[Configuration](docs/CONFIGURATION.md)** : Options avancées, fichiers config
- 💡 **[Exemples](docs/USAGE.md)** : Cas d'usage réels avec screenshots
- 🔧 **[Développement](docs/DEVELOPMENT.md)** : Build, tests, contribution

---

## 🔧 Commandes CLI

### `scan` - Analyse de dossier

```bash
roguebox scan --root <path> [OPTIONS]
```

**Options** :

- `--include <glob>` : Pattern d'inclusion (répétable)
- `--exclude <glob>` : Pattern d'exclusion (répétable)
- `--max-size-mb <int>` : Taille max fichier (défaut: 50)
- `--include-secrets` : Force inclusion de secrets (⚠️)
- `--dry-run` : Simulation sans sortie fichier

**Exemple** :

```powershell
roguebox scan --root ".\src" --exclude "*.log" --exclude "node_modules" --max-size-mb 100
```

---

### `init-repo` - Initialisation Git/GitHub

```bash
roguebox init-repo --root <path> --repo-name <name> [OPTIONS]
```

**Options** :

- `--org <org>` : Organisation GitHub (défaut: user)
- `--private` : Repo privé (défaut)
- `--public` : Repo public
- `--no-remote` : Skip création GitHub (local seulement)
- `--dry-run` : Affiche plan sans exécution

**Exemples** :

```powershell
# Repo privé d'org
roguebox init-repo --root ".\PRA" --repo-name "pra-2025" --private --org "EPSI"

# Repo public personnel
roguebox init-repo --root ".\projet" --repo-name "mon-projet" --public

# Local seulement
roguebox init-repo --root ".\test" --repo-name "local-test" --no-remote
```

---

### `push-all` - Commit & Push

```bash
roguebox push-all --root <path> [OPTIONS]
```

**Options** :

- `--branch <name>` : Branche cible (défaut: main)
- `--commit-message "<msg>"` : Message personnalisé
- `--dry-run` : Affiche plan (single/chunked)

**Exemples** :

```powershell
# Push simple
roguebox push-all --root ".\PRA"

# Push avec message custom
roguebox push-all --root ".\projet" --commit-message "feat: add authentication module"

# Push sur branche dev
roguebox push-all --root ".\projet" --branch "dev"
```

---

### `full-run` - Pipeline complet

```bash
roguebox full-run --root <path> --repo-name <name> [OPTIONS]
```

**Options** : Combine toutes les options de `init-repo` et `push-all`

**Exemple complet** :

```powershell
roguebox full-run `
  --root "C:\Users\User\Workshop\PRA" `
  --repo-name "epsi-pra-i2-2025" `
  --private `
  --org "EPSI-Cyber" `
  --branch "main" `
  --max-size-mb 50
```

---

## 🔒 Sécurité & Best Practices

### 🛡️ Fichiers ignorés par défaut (`.rogueignore`)

```
# Clés et secrets
*.key
*.pem
*.pfx
*.p12
*.kdbx
.env
.env.*
id_*
*token*
*secret*
*password*

# Archives lourdes
*.iso
*.zip
*.tar.gz
*.7z
*.rar

# Médias
*.mp4
*.mov
*.avi
*.mkv
```

### 📋 Checklist avant push

- [ ] Aucun fichier `.env` ou token dans le code
- [ ] Vérifier `scan --dry-run` pour détection secrets
- [ ] `.rogueignore` personnalisé si besoins spécifiques
- [ ] Tester avec `--dry-run` avant exécution réelle
- [ ] Repos sensibles → toujours `--private`

### 🔐 Authentification GitHub

**Option 1 : GitHub CLI (Recommandé)**

```powershell
gh auth login
# Suivre le flow web
```

**Option 2 : Token d'accès personnel**

```powershell
# Windows
$env:GITHUB_TOKEN="ghp_votretoken..."

# Linux/macOS
export GITHUB_TOKEN="ghp_votretoken..."
```

**Permissions requises** :

- `repo` (création repos)
- `read:org` (si utilisation `--org`)

---

## 📊 Logs & Observabilité

### Logs JSON structurés

**Emplacement** : `logs/rogue.log`

**Format** :

```json
{"ctx":"init-repo","level":"info","msg":"Repo created via gh CLI","ts":"2025-10-15T11:03:00"}
{"ctx":"push-all","level":"warn","msg":"Commit failed (possibly nothing to commit); will attempt push anyway","ts":"2025-10-15T11:03:01"}
{"ctx":"git","level":"error","msg":"non-zero exit","code":"128","ts":"2025-10-15T11:03:02"}
```

### Proof of Work

**Emplacement** : `docs/PROOF_OF_WORK.md`

**Contenu** :

- Timestamp de génération
- Inventaire JSON complet des fichiers
- Hash de chaque fichier (démonstration via `std::hash`)
- Total size du workspace

**Exemple** :

````markdown
# PROOF OF WORK

Generated at: 2025-10-15T11:03:00

## Inventory

```json
{
  "files": [...],
  "total_size": 66446,
  "generated_at": "2025-10-15T11:03:00",
  "root": "C:\\Workshop\\PRA"
}
```
````

---

## 🧪 Tests

### Exécuter les tests

```bash
# Configuration et build
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j 8

# Exécuter les tests
cd build
ctest -C Release --output-on-failure
```

**Tests inclus** :

- ✅ Scanner (patterns, ignore, tailles)
- ✅ GitOps (init, remote, commit)
- ✅ Config parser (TOML basique)

---

## 🏗️ Architecture

```
RogueMagicBox/
├── src/
│   ├── main.cpp              # CLI entry point
│   ├── cli/                  # Command handlers
│   │   ├── args.hpp          # CLI argument parsing
│   │   ├── commands_scan.cpp
│   │   ├── commands_init.cpp
│   │   └── commands_push.cpp
│   └── core/                 # Business logic
│       ├── scanner.cpp       # Workspace scanning
│       ├── gitops.cpp        # Git operations
│       ├── github_api.cpp    # GitHub API/CLI
│       ├── logger.cpp        # JSON logging
│       ├── config.cpp        # Config parser
│       └── utils.cpp         # Helpers
├── include/rogue/            # Public headers
├── tests/                    # Unit tests
├── docs/                     # Documentation
├── scripts/                  # Install/demo scripts
└── third_party/              # Vendored deps (JSON, Catch2 shim)
```

**Modules clés** :

- **Scanner** : Parcours récursif, filtres, métadonnées
- **GitOps** : Abstraction Git (`git -C` pour portabilité)
- **GitHubApi** : Création repos (libcurl → gh fallback)
- **Logger** : Logs JSON structurés avec masquage secrets

---

## 🛠️ Build depuis les sources

### Options CMake

```bash
cmake -S . -B build \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_TESTS=ON \
  -DCMAKE_INSTALL_PREFIX=/usr/local
```

**Variables** :

- `CMAKE_BUILD_TYPE` : `Debug`, `Release`, `RelWithDebInfo`
- `BUILD_TESTS` : `ON`/`OFF` (défaut: `ON`)
- `HAVE_LIBCURL` : Auto-détecté (si présent → GitHub API)

### Packaging

```bash
# Générer archive distributable
cmake --build build --target package

# Résultat: RogueMagicBox-1.0.0-win64.zip (ou .tar.gz)
```

---

## 🚀 CI/CD Pipeline

RogueMagicBox implémente une **pipeline CI/CD complète** conforme au défi **"CI/CD Express voie 93⁄4"** :

✅ **Tests unitaires** (Catch2)  
✅ **Tests de non-régression** (E2E automatisés)  
✅ **Vérification norme de code** (clang-format, clang-tidy)  
✅ **Build & test image Docker** (multi-stage Alpine)  
✅ **Analyse SonarCloud** (qualité, sécurité, dette technique)  
✅ **Déploiement automatique** (GitHub Releases + Docker Registry)

**Documentation complète** : [docs/CI_CD.md](docs/CI_CD.md)

### 🐳 Utilisation Docker

```bash
# Pull depuis GitHub Container Registry
docker pull ghcr.io/xxxsamyxxx/roguebox:latest

# Scan d'un workspace
docker run --rm -v $(pwd):/workspace \
  ghcr.io/xxxsamyxxx/roguebox:latest \
  scan --root /workspace --dry-run

# Full-run avec GitHub auth
docker run --rm -v $(pwd):/workspace \
  -e GITHUB_TOKEN=$GITHUB_TOKEN \
  ghcr.io/xxxsamyxxx/roguebox:latest \
  full-run --root /workspace --repo-name "my-project" --private
```

---

## 🤝 Contribution

Les contributions sont bienvenues ! Veuillez :

1. Fork le projet
2. Créer une branche feature (`git checkout -b feature/amazing`)
3. Commit avec messages sémantiques (`git commit -m 'feat: add X'`)
4. Push vers la branche (`git push origin feature/amazing`)
5. Ouvrir une Pull Request

**Guidelines** :

- Suivre C++17 best practices
- Ajouter tests pour nouvelles features
- Mettre à jour la documentation
- Respecter le style existant (clang-format)

---

## 📄 Licence

MIT License - voir [LICENSE](LICENSE)

Copyright (c) 2025 RogueMagicBox Contributors

---

## 🙏 Remerciements

- **nlohmann/json** : JSON library (vendored minimal shim)
- **Catch2** : Test framework (shim inclus)
- **GitHub CLI** : Fallback pour création repos
- Communauté C++ pour les best practices

---

## 📞 Support

- 🐛 **Issues** : [GitHub Issues](https://github.com/votre-org/RogueMagicBox/issues)
- 💬 **Discussions** : [GitHub Discussions](https://github.com/votre-org/RogueMagicBox/discussions)
- 📧 **Email** : support@roguebox.dev

---

**Made with ❤️ for Workshop automation**
