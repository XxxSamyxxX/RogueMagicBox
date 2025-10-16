# 📘 Guide Utilisateur RogueMagicBox

Guide complet pour utiliser **roguebox.exe** dans tous les scénarios.

---

## Table des matières

1. [Installation et Configuration](#installation-et-configuration)
2. [Commande `scan`](#commande-scan)
3. [Commande `init-repo`](#commande-init-repo)
4. [Commande `push-all`](#commande-push-all)
5. [Commande `full-run`](#commande-full-run)
6. [Workflows Recommandés](#workflows-recommandés)
7. [Dépannage](#dépannage)

---

## Installation et Configuration

### 1. Vérifier les prérequis

```powershell
# Git
git --version  # ≥ 2.30

# CMake (si build from source)
cmake --version  # ≥ 3.20

# GitHub CLI (optionnel mais recommandé)
gh --version  # ≥ 2.0
```

### 2. Configurer l'authentification GitHub

**Option A : GitHub CLI (Recommandé)**

```powershell
gh auth login
```

Suivez le flow interactif :

- Protocol: **HTTPS**
- Authenticate Git: **Yes**
- Copiez le code one-time
- Ouvrez le navigateur et collez le code

**Option B : Token d'accès personnel**

1. Aller sur GitHub → Settings → Developer settings → Personal access tokens
2. Generate new token (classic)
3. Permissions : `repo`, `read:org`
4. Copier le token

```powershell
# Windows PowerShell
$env:GITHUB_TOKEN="ghp_votre_token_ici"

# Pour persister (déconseillé en prod)
[System.Environment]::SetEnvironmentVariable('GITHUB_TOKEN', 'ghp_...', 'User')
```

### 3. Premier test

```powershell
roguebox.exe scan --root "." --dry-run
```

Si aucune erreur → Installation OK ✅

---

## Commande `scan`

### 📖 Description

Analyse un dossier et génère un inventaire JSON des fichiers avec métadonnées.

### 🎯 Syntaxe

```bash
roguebox scan --root <path> [OPTIONS]
```

### ⚙️ Options

| Option              | Type     | Défaut     | Description                      |
| ------------------- | -------- | ---------- | -------------------------------- |
| `--root`            | `<path>` | **Requis** | Chemin du dossier à scanner      |
| `--include`         | `<glob>` | `*`        | Pattern d'inclusion (répétable)  |
| `--exclude`         | `<glob>` | -          | Pattern d'exclusion (répétable)  |
| `--max-size-mb`     | `<int>`  | `50`       | Taille max par fichier (MB)      |
| `--include-secrets` | flag     | `false`    | Forcer inclusion de secrets (⚠️) |
| `--dry-run`         | flag     | `false`    | Simulation sans sortie           |

### 📝 Exemples

#### Scan basique

```powershell
roguebox scan --root "C:\Workshop\PRA"
```

**Sortie** :

```json
{
  "files": [
    {
      "path": "README.md",
      "size": 1234,
      "hash": "abc123...",
      "mtime": "2025-10-15T10:00:00"
    },
    {
      "path": "src/main.cpp",
      "size": 5678,
      "hash": "def456...",
      "mtime": "2025-10-15T09:30:00"
    }
  ],
  "total_size": 6912,
  "generated_at": "2025-10-15T11:00:00",
  "root": "C:\\Workshop\\PRA"
}
```

#### Scan avec filtres

```powershell
# Exclure logs et node_modules
roguebox scan --root ".\projet" --exclude "*.log" --exclude "node_modules"

# Scanner seulement les fichiers Python
roguebox scan --root ".\src" --include "*.py"

# Augmenter la limite de taille
roguebox scan --root ".\data" --max-size-mb 100
```

#### Dry-run (simulation)

```powershell
roguebox scan --root ".\Workshop" --dry-run
```

**Affiche** :

- Nombre de fichiers trouvés
- Total size estimé
- Warnings si secrets détectés
- **Aucun fichier créé**

### 🔍 Détection de secrets

Le scanner détecte automatiquement :

**Patterns de fichiers** :

- `*.key`, `*.pem`, `*.pfx`
- `.env`, `.env.local`, `.env.production`
- `id_rsa`, `id_dsa`
- `*token*`, `*secret*`, `*password*`

**Patterns de contenu** :

- `password=`, `api_key=`
- `Bearer`, `Authorization:`
- `aws_access_key_id`

**Avertissement affiché** :

```
[warn] scanner: Sensitive file detected: config/.env (pattern: .env)
```

### 📂 Fichiers de sortie

**Emplacement** : `./scan_result.json` (racine d'exécution)

**Format** : Voir exemple ci-dessus

---

## Commande `init-repo`

### 📖 Description

Initialise un dépôt Git local et crée optionnellement un repo GitHub distant.

### 🎯 Syntaxe

```bash
roguebox init-repo --root <path> --repo-name <name> [OPTIONS]
```

### ⚙️ Options

| Option        | Type     | Défaut     | Description             |
| ------------- | -------- | ---------- | ----------------------- |
| `--root`      | `<path>` | **Requis** | Dossier du projet       |
| `--repo-name` | `<name>` | **Requis** | Nom du repo GitHub      |
| `--org`       | `<org>`  | (user)     | Organisation GitHub     |
| `--private`   | flag     | `true`     | Créer repo privé        |
| `--public`    | flag     | `false`    | Créer repo public       |
| `--no-remote` | flag     | `false`    | Skip création GitHub    |
| `--dry-run`   | flag     | `false`    | Afficher plan seulement |

### 📝 Exemples

#### Repo privé d'organisation

```powershell
roguebox init-repo `
  --root "C:\Workshop\PRA" `
  --repo-name "epsi-pra-2025" `
  --private `
  --org "EPSI-Cyber"
```

**Actions** :

1. ✅ `git init` dans `C:\Workshop\PRA`
2. ✅ Création de `.gitignore`, `.rogueignore`, `LICENSE`, `README.md`
3. ✅ Création repo `EPSI-Cyber/epsi-pra-2025` (privé) sur GitHub
4. ✅ Configuration remote `origin` → `https://github.com/EPSI-Cyber/epsi-pra-2025.git`

#### Repo public personnel

```powershell
roguebox init-repo `
  --root ".\mon-projet" `
  --repo-name "awesome-project" `
  --public
```

**Résultat** :

- Repo `VotreUser/awesome-project` (public)
- Visible dans vos repos GitHub

#### Local seulement (pas de GitHub)

```powershell
roguebox init-repo `
  --root ".\test-local" `
  --repo-name "local-test" `
  --no-remote
```

**Actions** :

- ✅ Init Git local
- ✅ Fichiers standards créés
- ❌ Aucune interaction GitHub

#### Dry-run

```powershell
roguebox init-repo `
  --root ".\PRA" `
  --repo-name "test-pra" `
  --dry-run
```

**Sortie** :

```
[info] init-repo: [dry-run] Would initialize git repo and create standard files (.gitignore, .rogueignore, LICENSE, README)
[info] init-repo: [dry-run] Would create and set GitHub remote
       url: https://github.com/VotreUser/test-pra.git
       private: true
```

### 📂 Fichiers créés automatiquement

#### `.gitignore`

```
build/
*.log
logs/
*.exe
*.obj
*.pdb
node_modules/
__pycache__/
```

#### `.rogueignore`

```
*.key
*.pem
*.pfx
*.kdbx
.env
.env.*
id_*
*token*
*secret*
*.iso
*.zip
*.tar.gz
*.7z
*.mp4
*.mov
```

#### `LICENSE`

MIT License avec année courante et placeholder copyright.

#### `README.md`

```markdown
# Workshop Import

Ce dépôt a été initialisé automatiquement par RogueMagicBox.

⚠️ Avertissement: ne commitez pas de données personnelles ni de secrets en clair.
Fichiers sensibles ignorés par défaut via .rogueignore.

Usage rapide: voir docs/USAGE.md dans RogueMagicBox.
```

### 🔐 Création GitHub

**Mécanisme** :

1. Tentative via **GitHub API** (si libcurl disponible et `GITHUB_TOKEN` défini)
2. Fallback vers **gh CLI** (si authentifié)
3. Erreur si aucune méthode disponible

**Idempotence** :

- Si le repo existe déjà → skip création, configure juste le remote

---

## Commande `push-all`

### 📖 Description

Stage, commit et push tous les fichiers vers le remote GitHub.

### 🎯 Syntaxe

```bash
roguebox push-all --root <path> [OPTIONS]
```

### ⚙️ Options

| Option             | Type      | Défaut     | Description                    |
| ------------------ | --------- | ---------- | ------------------------------ |
| `--root`           | `<path>`  | **Requis** | Dossier du repo                |
| `--branch`         | `<name>`  | `main`     | Branche cible                  |
| `--commit-message` | `"<msg>"` | (auto)     | Message de commit personnalisé |
| `--dry-run`        | flag      | `false`    | Afficher plan sans push        |

### 📝 Exemples

#### Push simple

```powershell
roguebox push-all --root "C:\Workshop\PRA"
```

**Actions** :

1. `git add -A`
2. `git commit -m "chore(import): add Workshop sources & docs (2025-10-15)"`
3. `git push -u origin main`

#### Push avec message personnalisé

```powershell
roguebox push-all `
  --root ".\projet" `
  --commit-message "feat: add authentication module"
```

#### Push sur branche dev

```powershell
roguebox push-all --root ".\app" --branch "dev"
```

#### Dry-run

```powershell
roguebox push-all --root ".\PRA" --dry-run
```

**Sortie** :

```
[info] push-all: [dry-run] Would commit and push
       message: chore(import): add Workshop sources & docs (2025-10-15)
       branch: main
       mode: single
```

### 🧩 Chunking automatique

**Problématique** : GitHub a une limite de 100 MB par push recommandée.

**Solution RogueMagicBox** :

- Si `total_size > 100 MB` → mode **chunked**
- Commits par batches de ~50 MB
- Message suffixé `[chunk]`

**Exemple** :

```powershell
# Projet de 250 MB
roguebox push-all --root ".\gros-projet"
```

**Commits créés** :

```
1. chore(import): add Workshop sources & docs (2025-10-15) [chunk]  # ~50 MB
2. chore(import): add Workshop sources & docs (2025-10-15) [chunk]  # ~50 MB
3. chore(import): add Workshop sources & docs (2025-10-15) [chunk]  # ~50 MB
4. chore(import): add Workshop sources & docs (2025-10-15) [chunk]  # ~50 MB
5. chore(import): add Workshop sources & docs (2025-10-15) [chunk]  # ~50 MB
```

**Fichiers >50 MB** :

- Skippés automatiquement
- Warning affiché :
  ```
  [warn] push-all: skip >50MB: data/large-dataset.zip
  ```

### ⚠️ Gestion "nothing to commit"

**Comportement** :

- Si `git commit` échoue (working tree clean)
- **Continue vers push** (peut pousser commit existant vers nouveau remote)
- Warning affiché :
  ```
  [warn] push-all: Commit failed (possibly nothing to commit); will attempt push anyway
  ```

---

## Commande `full-run`

### 📖 Description

Pipeline complet : **scan → init-repo → push-all → proof of work**.

### 🎯 Syntaxe

```bash
roguebox full-run --root <path> --repo-name <name> [OPTIONS]
```

### ⚙️ Options

Combine toutes les options de `scan`, `init-repo`, et `push-all`.

**Principales** :

- `--root <path>` (requis)
- `--repo-name <name>` (requis)
- `--org <org>` (optionnel)
- `--private` / `--public`
- `--branch <name>`
- `--max-size-mb <int>`
- `--dry-run`

### 📝 Exemple complet

```powershell
roguebox full-run `
  --root "C:\Users\Student\Workshop\PRA" `
  --repo-name "epsi-i2-cyber-pra-2025" `
  --private `
  --org "EPSI-Cyber" `
  --branch "main" `
  --max-size-mb 50
```

### 🔄 Pipeline détaillé

#### Étape 1 : Scan (automatique)

```
[info] scanner: Scanning C:\Users\Student\Workshop\PRA
[info] scanner: Found 24 files, total: 66.4 KB
```

#### Étape 2 : Init Repo

```
[info] git: Repo already initialized (ou git init si nouveau)
[info] github: Repo created via gh CLI
       https://github.com/EPSI-Cyber/epsi-i2-cyber-pra-2025
[info] git: Remote 'origin' configured
```

#### Étape 3 : Push

```
[info] git: exec: git -C "C:\...\PRA" add -A
[info] git: exec: git -C "C:\...\PRA" commit -m "..."
[info] git: exec: git -C "C:\...\PRA" push -u origin main
Enumerating objects: 8, done.
Writing objects: 100% (8/8), 7.88 KiB | 3.94 MiB/s, done.
[info] push-all: Pushed successfully
```

#### Étape 4 : Proof of Work

**Fichier créé** : `docs/PROOF_OF_WORK.md`

**Contenu** :

````markdown
# PROOF OF WORK

Generated at: 2025-10-15T11:03:00

## Inventory

```json
{
  "files": [
    {"path": "README.md", "size": 1234, "hash": "abc...", "mtime": "2025-10-15T10:00:00"},
    ...
  ],
  "total_size": 66446,
  "generated_at": "2025-10-15T11:03:00",
  "root": "C:\\Users\\Student\\Workshop\\PRA"
}
```
````

### ✅ Résultat final

- ✅ Dossier PRA scanné et inventorié
- ✅ Repo Git initialisé avec standards
- ✅ Repo GitHub `EPSI-Cyber/epsi-i2-cyber-pra-2025` créé (privé)
- ✅ Tous les fichiers commités et pushés
- ✅ Preuve de travail horodatée disponible
- ✅ Logs JSON complets dans `logs/rogue.log`

---

## Workflows Recommandés

### 🎓 Workshop EPSI - PRA

```powershell
# 1. Naviguer vers le dossier Workshop
cd "C:\Users\Student\Desktop\EPSI\Workshop"

# 2. Full-run en un coup
roguebox full-run `
  --root ".\PRA" `
  --repo-name "epsi-i2-pra-$(Get-Date -Format 'yyyy-MM-dd')" `
  --private `
  --org "EPSI-I2-Cyber"

# 3. Vérifier le repo en ligne
gh repo view EPSI-I2-Cyber/epsi-i2-pra-2025-10-15 --web
```

### 🔍 Audit de projet avant push

```powershell
# 1. Scanner en dry-run
roguebox scan --root ".\MonProjet" --dry-run

# 2. Vérifier les warnings de sécurité
# Si OK → continuer

# 3. Init + Push
roguebox full-run --root ".\MonProjet" --repo-name "audit-2025" --private
```

### 🚀 Mise à jour d'un repo existant

```powershell
# 1. Modifier des fichiers dans le projet
# ...

# 2. Push uniquement (repo déjà initialisé)
roguebox push-all `
  --root ".\MonProjet" `
  --commit-message "docs: update README and architecture diagrams"
```

### 🧪 Test avant production

```powershell
# 1. Dry-run complet
roguebox full-run `
  --root ".\ProjetSensible" `
  --repo-name "test-projet" `
  --dry-run

# 2. Analyser le plan affiché
# 3. Si OK → relancer sans --dry-run
```

---

## Dépannage

### ❌ Erreur : "gh repo create failed"

**Cause** : GitHub CLI non authentifié ou token manquant

**Solution** :

```powershell
# Vérifier auth
gh auth status

# Si pas connecté
gh auth login
```

### ❌ Erreur : "fatal: not a git repository"

**Cause** : Commande exécutée hors du bon dossier

**Solution** :

```powershell
# Toujours utiliser --root absolu
roguebox push-all --root "C:\chemin\complet\vers\projet"
```

### ❌ Erreur : "Failed to push" (remote ahead)

**Cause** : Le remote a des commits que vous n'avez pas en local

**Solution** :

```powershell
cd "C:\chemin\vers\projet"
git pull origin main --rebase
roguebox push-all --root "."
```

### ⚠️ Warning : "Sensitive file detected"

**Cause** : Fichier potentiellement sensible trouvé (`.env`, `*.key`, etc.)

**Options** :

1. **Recommandé** : Ajouter au `.rogueignore`

   ```
   echo "mon-fichier-sensible.key" >> .rogueignore
   ```

2. **Déconseillé** : Forcer inclusion
   ```powershell
   roguebox scan --root "." --include-secrets
   ```

### 🐌 Push très lent

**Cause** : Fichiers lourds (>50 MB) ou nombreux

**Solutions** :

- Vérifier `.rogueignore` pour exclure médias/archives
- Utiliser `--max-size-mb` plus petit
- Scanner en dry-run pour voir le total size

### 📋 Logs pour debug

**Consulter** :

```powershell
# Logs JSON complets
cat logs\rogue.log | ConvertFrom-Json | Format-List

# Filtrer par niveau
cat logs\rogue.log | Select-String "error"
```

---

## 🔗 Ressources

- [README principal](../README.md)
- [Architecture technique](ARCHITECTURE.md)
- [Guide configuration](CONFIGURATION.md)
- [Exemples avancés](USAGE.md)

---

**Bon scan et bon push ! 🚀**
