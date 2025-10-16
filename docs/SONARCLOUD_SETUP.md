# 📊 Guide de Configuration SonarCloud pour RogueMagicBox

## 🎯 Objectif

Configurer l'analyse de qualité de code automatique avec SonarCloud pour détecter :

- 🐛 **Bugs** : Erreurs logiques dans le code
- 🔒 **Vulnerabilities** : Failles de sécurité
- 🧹 **Code Smells** : Mauvaises pratiques
- 📊 **Coverage** : Couverture de tests
- 📋 **Duplications** : Code dupliqué

---

## ✅ Prérequis

- [x] Compte GitHub
- [x] Projet RogueMagicBox sur GitHub (public ou organisation avec SonarCloud)
- [ ] Compte SonarCloud (gratuit pour projets open-source)

---

## 📝 Étape 1 : Créer un Compte SonarCloud

### 1.1 S'inscrire sur SonarCloud

1. Va sur **https://sonarcloud.io**
2. Clique sur **"Log in"** en haut à droite
3. Choisis **"Log in with GitHub"**
4. Autorise SonarCloud à accéder à ton compte GitHub
5. Accepte les conditions d'utilisation

### 1.2 Créer une Organisation

1. Une fois connecté, clique sur **"+"** puis **"Analyze new project"**
2. Si c'est ta première fois, SonarCloud te demandera de créer une organisation :
   - **Name** : `xxxsamyxxx` (ou ton username GitHub)
   - **Key** : `xxxsamyxxx` (sera utilisé dans la config)
   - **Plan** : Choisis **"Free plan"** (pour projets publics)
3. Clique sur **"Create Organization"**

---

## 📦 Étape 2 : Importer le Projet RogueMagicBox

### 2.1 Ajouter le Projet

1. Dans SonarCloud, clique sur **"+"** → **"Analyze new project"**
2. Autorise SonarCloud à accéder à tes repos GitHub si demandé
3. Dans la liste des repos, trouve **"XxxSamyxxX/RogueMagicBox"**
4. Clique sur **"Set Up"** à côté du repo

### 2.2 Configurer le Projet

1. **Project Key** : `XxxSamyxxX_RogueMagicBox` (déjà configuré dans `sonar-project.properties`)
2. **Organization** : `xxxsamyxxx` (ton org)
3. Choisis **"With GitHub Actions"** comme méthode d'analyse
4. SonarCloud te donnera un **SONAR_TOKEN** → **COPIE-LE** (il ne sera plus visible après)

---

## 🔐 Étape 3 : Ajouter le Token à GitHub

### 3.1 Créer le Secret GitHub

1. Va sur **https://github.com/XxxSamyxxX/RogueMagicBox**
2. Clique sur **"Settings"** (onglet du repo)
3. Dans le menu de gauche, clique sur **"Secrets and variables"** → **"Actions"**
4. Clique sur **"New repository secret"**
5. Remplis :
   - **Name** : `SONAR_TOKEN`
   - **Secret** : Colle le token SonarCloud que tu as copié
6. Clique sur **"Add secret"**

### 3.2 Vérifier le Token

Le secret `SONAR_TOKEN` doit maintenant apparaître dans la liste des secrets.

---

## ⚙️ Étape 4 : Vérifier la Configuration du Projet

### 4.1 Fichier `sonar-project.properties`

Le fichier est déjà configuré dans le projet :

```properties
# Configuration SonarCloud pour RogueMagicBox
sonar.projectKey=XxxSamyxxX_RogueMagicBox
sonar.organization=xxxsamyxxx

# Métadonnées du projet
sonar.projectName=RogueMagicBox
sonar.projectVersion=1.0.0

# Chemins sources et tests
sonar.sources=src,include
sonar.tests=tests
sonar.exclusions=build/**,third_party/**,docs/**,scripts/**

# C++ Build Wrapper
sonar.cfamily.build-wrapper-output=bw-output
sonar.cfamily.cache.enabled=true
sonar.cfamily.threads=4
```

✅ **Pas de modification nécessaire** si ton organisation est bien `xxxsamyxxx`.

### 4.2 Workflow GitHub Actions

Le workflow `.github/workflows/ci-cd-complete.yml` contient déjà le stage SonarCloud :

```yaml
sonarcloud-analysis:
  name: 📊 SonarCloud Analysis
  needs: [code-quality, build-and-test]
  runs-on: ubuntu-latest

  steps:
    - name: 📥 Checkout
      uses: actions/checkout@v4
      with:
        fetch-depth: 0

    - name: 🛠️ Configure and build with wrapper
      run: |
        curl -sSLo build-wrapper-linux-x86.zip \
          https://sonarcloud.io/static/cpp/build-wrapper-linux-x86.zip
        unzip -q build-wrapper-linux-x86.zip
        cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
        ./build-wrapper-linux-x86/build-wrapper-linux-x86-64 \
          --out-dir bw-output \
          cmake --build build -j

    - name: 🔍 SonarCloud Scan
      uses: SonarSource/sonarcloud-github-action@master
      env:
        GITHUB_TOKEN: ${{ secrets.GITHUB_TOKEN }}
        SONAR_TOKEN: ${{ secrets.SONAR_TOKEN }}
```

✅ **Déjà configuré et prêt à l'emploi !**

---

## 🚀 Étape 5 : Lancer la Première Analyse

### 5.1 Option A : Push un Commit

La méthode la plus simple :

```bash
# Fais un petit changement (ou vide)
git commit --allow-empty -m "chore: trigger SonarCloud analysis"
git push origin master
```

### 5.2 Option B : Re-run un Workflow Existant

1. Va sur **https://github.com/XxxSamyxxX/RogueMagicBox/actions**
2. Clique sur le dernier workflow **"CI/CD Complete Pipeline"**
3. Clique sur **"Re-run all jobs"**

### 5.3 Vérifier l'Exécution

1. Va sur l'onglet **"Actions"** du repo GitHub
2. Ouvre le workflow en cours d'exécution
3. Vérifie que le stage **"📊 SonarCloud Analysis"** s'exécute
4. Attends que le job se termine (environ 2-3 minutes)

---

## 📊 Étape 6 : Consulter les Résultats

### 6.1 Dashboard SonarCloud

1. Va sur **https://sonarcloud.io/dashboard?id=XxxSamyxxX_RogueMagicBox**
2. Tu verras le dashboard avec :
   - **Bugs** : Nombre de bugs détectés
   - **Vulnerabilities** : Failles de sécurité
   - **Code Smells** : Mauvaises pratiques
   - **Coverage** : Couverture de tests (si configuré)
   - **Duplications** : Code dupliqué
   - **Security Hotspots** : Points sensibles à vérifier

### 6.2 Quality Gate

SonarCloud applique un **Quality Gate** par défaut :

- ✅ **Passed** : Le code respecte les critères de qualité
- ❌ **Failed** : Des problèmes critiques ont été détectés

Critères par défaut :

- 🐛 Nouveaux bugs : 0
- 🔒 Nouvelles vulnérabilités : 0
- 🧹 Nouveaux code smells : acceptés selon ratio
- 📊 Coverage sur nouveau code : > 80% (si configuré)

### 6.3 Badge SonarCloud

Tu peux ajouter un badge dans ton README :

```markdown
[![Quality Gate Status](https://sonarcloud.io/api/project_badges/measure?project=XxxSamyxxX_RogueMagicBox&metric=alert_status)](https://sonarcloud.io/dashboard?id=XxxSamyxxX_RogueMagicBox)

[![Bugs](https://sonarcloud.io/api/project_badges/measure?project=XxxSamyxxX_RogueMagicBox&metric=bugs)](https://sonarcloud.io/dashboard?id=XxxSamyxxX_RogueMagicBox)
[![Code Smells](https://sonarcloud.io/api/project_badges/measure?project=XxxSamyxxX_RogueMagicBox&metric=code_smells)](https://sonarcloud.io/dashboard?id=XxxSamyxxX_RogueMagicBox)
[![Security Rating](https://sonarcloud.io/api/project_badges/measure?project=XxxSamyxxX_RogueMagicBox&metric=security_rating)](https://sonarcloud.io/dashboard?id=XxxSamyxxX_RogueMagicBox)
```

---

## 🔧 Configuration Avancée (Optionnel)

### Coverage (Couverture de Tests)

Pour activer la couverture de code avec `gcov` :

1. **Modifier CMakeLists.txt** pour activer les flags de coverage :

```cmake
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
endif()
```

2. **Générer le rapport de couverture** :

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j
cd build && ctest
gcov -r -p **/*.gcda
```

3. **Mettre à jour `sonar-project.properties`** :

```properties
sonar.cfamily.gcov.reportsPath=build
```

4. **Mettre à jour le workflow GitHub Actions** :

```yaml
- name: 📊 Generate coverage
  run: |
    cd build
    ctest
    gcov -r -p **/*.gcda
```

### Quality Gate Personnalisé

1. Va sur SonarCloud → Project Settings → Quality Gate
2. Crée un nouveau Quality Gate avec tes propres critères :
   - Bugs : 0 (strict)
   - Vulnerabilities : 0 (strict)
   - Code Smells : < 50
   - Coverage : > 80%
   - Duplications : < 3%

---

## 🐛 Dépannage

### Erreur : "SONAR_TOKEN not found"

**Cause** : Le secret n'est pas configuré dans GitHub.

**Solution** :

1. Vérifie que le secret `SONAR_TOKEN` existe dans Settings → Secrets → Actions
2. Vérifie l'orthographe exacte (sensible à la casse)

### Erreur : "Project not found in SonarCloud"

**Cause** : Le projet n'existe pas ou la clé est incorrecte.

**Solution** :

1. Vérifie que le projet existe sur SonarCloud
2. Vérifie `sonar.projectKey` dans `sonar-project.properties`
3. Vérifie `sonar.organization` correspond à ton org SonarCloud

### Erreur : "Build wrapper output not found"

**Cause** : Le build-wrapper n'a pas été exécuté correctement.

**Solution** :

1. Vérifie que le build-wrapper est téléchargé et exécuté avant le scan
2. Vérifie que le chemin `bw-output` est correct

### L'analyse ne détecte aucun fichier C++

**Cause** : Les chemins sources sont incorrects.

**Solution** :

1. Vérifie `sonar.sources=src,include` dans `sonar-project.properties`
2. Vérifie que ces dossiers existent et contiennent du code C++

---

## 📚 Ressources

- **Documentation SonarCloud** : https://docs.sonarcloud.io/
- **Build Wrapper C++** : https://docs.sonarcloud.io/advanced-setup/languages/c-c-objective-c/
- **Quality Gates** : https://docs.sonarcloud.io/improving/quality-gates/
- **Coverage C++** : https://docs.sonarcloud.io/enriching/test-coverage/overview/

---

## ✅ Checklist Finale

Avant de considérer SonarCloud comme configuré :

- [ ] Compte SonarCloud créé
- [ ] Organisation créée sur SonarCloud
- [ ] Projet RogueMagicBox importé
- [ ] Token SonarCloud copié
- [ ] Secret `SONAR_TOKEN` ajouté dans GitHub
- [ ] `sonar-project.properties` vérifié
- [ ] Workflow GitHub Actions vérifié
- [ ] Premier scan lancé et réussi
- [ ] Dashboard SonarCloud accessible
- [ ] Quality Gate configuré (optionnel)
- [ ] Badge SonarCloud ajouté au README (optionnel)

---

## 🎯 Résumé en 5 Minutes

1. **Créer compte SonarCloud** → https://sonarcloud.io (login avec GitHub)
2. **Importer projet** → Analyze new project → XxxSamyxxX/RogueMagicBox
3. **Copier token** → SonarCloud te donne un `SONAR_TOKEN`
4. **Ajouter secret GitHub** → Settings → Secrets → Actions → New secret : `SONAR_TOKEN`
5. **Push un commit** → `git commit --allow-empty -m "chore: enable SonarCloud" && git push`
6. **Vérifier résultats** → https://sonarcloud.io/dashboard?id=XxxSamyxxX_RogueMagicBox

✅ **C'est tout ! L'analyse SonarCloud sera maintenant exécutée automatiquement à chaque push.**

---

**Date** : 16 octobre 2025  
**Auteur** : GitHub Copilot  
**Version** : 1.0.0
