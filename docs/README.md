# RogueMagicBox

Logiciel cross-platform (Linux/Windows) pour scanner un dossier de Workshop, initialiser un dépôt Git/GitHub et pousser les sources, avec logs, dry-run et preuve de travail.

Avertissement: Ne stockez pas de données personnelles ni de secrets en clair. Les fichiers sensibles sont ignorés par défaut (.env, _.key, _.pem, *.pfx, *token\*). Utilisez --include-secrets à vos risques pour la démo.

## Installation rapide (60s)

Linux:

```
./scripts/install_linux.sh
```

Windows (PowerShell):

```
./scripts/install_windows.ps1
```

## Usage rapide

- Scan (inventaire JSON):

```
build/roguebox scan --root ../MonWorkshop --dry-run
```

- Run complet (scan → init → push):

```
build/roguebox full-run --root ../MonWorkshop --repo-name epsi-roguebox-demo --private
```

## Options clés

- --root <path>
- --include / --exclude (glob simple)
- --max-size-mb (par défaut 50)
- --dry-run
- --repo-name, --org, --private|--public, --no-remote
- --branch, --commit-message
- --config <file> (format TOML simplifié key=value)
- --include-secrets (affiche bannière d’avertissement)

## Limitations

- Hash non-cryptographique pour la démo (std::hash) — remplaçable par SHA-256.
- API GitHub dépend de libcurl sinon fallback gh CLI.
- Glob simplifié (\* et ?).

## Packaging

CPack génère .zip (Windows) et .tar.gz (Linux) lors de l’exécution de cpack.

## Sécurité

- Jamais de token affiché dans les logs.
- .rogueignore inclut des patterns sensibles par défaut.
