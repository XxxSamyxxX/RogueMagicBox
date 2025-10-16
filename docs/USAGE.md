# Guide d’utilisation

## Commandes

- scan --root <path> [--include <glob> …] [--exclude <glob> …] [--max-size-mb <int>] [--dry-run]
- init-repo --root <path> --repo-name <name> [--org <org>] [--private|--public] [--no-remote]
- push-all --root <path> [--branch <name>] [--commit-message "<msg>"] [--dry-run]
- full-run --root <path> --repo-name <name> [options…]

## Exemples (Linux)

```
./scripts/install_linux.sh
build/roguebox full-run \
  --root ../MonWorkshop \
  --repo-name epsi-roguebox-demo \
  --private --dry-run
build/roguebox full-run --root ../MonWorkshop --repo-name epsi-roguebox-demo --private
```

## Exemples (Windows PowerShell)

```
./scripts/install_windows.ps1
./build/roguebox.exe full-run --root ../MonWorkshop --repo-name epsi-roguebox-demo --private
```

## TUI (ASCII mock)

```
+---------------------------+
| Rogue Magic Box           |
+---------------------------+
| [*] Scanning files...     |
| [*] Initializing repo...  |
| [*] Committing changes... |
| [*] Pushing to remote...  |
+---------------------------+
| Status: OK                |
+---------------------------+
```

## Codes de sortie

- 0: OK
- 1: Erreur d’arguments
- 2: Scan échoué
- 3: Init repo échoué
- 4: Création GitHub échouée
- 5: Remote échoué
- 6: Stage échoué
- 7: Commit échoué
- 8: Push échoué
