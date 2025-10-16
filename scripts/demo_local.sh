#!/usr/bin/env bash
set -euo pipefail

# Demo scenario (local)
ROOT=${1:-../MonWorkshop}
REPO=${2:-epsi-roguebox-demo}

./scripts/install_linux.sh
./build/roguebox scan --root "$ROOT" --dry-run
./build/roguebox full-run --root "$ROOT" --repo-name "$REPO" --private --dry-run
./build/roguebox full-run --root "$ROOT" --repo-name "$REPO" --private
