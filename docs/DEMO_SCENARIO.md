# Scénario de démo (3–5 min)

1. Installer et construire:

```
./scripts/install_linux.sh
```

2. Scan d’un dossier Workshop:

```
build/roguebox scan --root ../MonWorkshop --dry-run
```

3. Création d’un dépôt privé et push:

```
build/roguebox full-run --root ../MonWorkshop --repo-name epsi-roguebox-demo --private
```

4. Preuve: afficher logs et inventaire:

```
cat logs/rogue.log
cat docs/PROOF_OF_WORK.md
```

Soutenance – points à montrer:

- Matrice build (CI), cross-platform
- Logs JSON + console lisible
- Dry-run
- Sécurité (.rogueignore, masquage tokens)
