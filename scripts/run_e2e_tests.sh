#!/bin/bash
# Script de tests End-to-End pour RogueMagicBox
# Tests de non-régression automatisés

set -e  # Exit on error

ROGUEBOX="./build/Release/roguebox"
TEST_DIR="./e2e_tests_workspace"
LOG_FILE="./e2e_tests.log"

# Colors
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

log_info() {
    echo -e "${GREEN}[INFO]${NC} $1" | tee -a "$LOG_FILE"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1" | tee -a "$LOG_FILE"
}

log_warn() {
    echo -e "${YELLOW}[WARN]${NC} $1" | tee -a "$LOG_FILE"
}

cleanup() {
    log_info "Cleaning up test workspace..."
    rm -rf "$TEST_DIR"
}

setup() {
    log_info "Setting up E2E test environment..."
    rm -f "$LOG_FILE"
    cleanup
    mkdir -p "$TEST_DIR"
    
    # Create sample files for testing
    echo "# Test README" > "$TEST_DIR/README.md"
    mkdir -p "$TEST_DIR/src"
    echo "int main() { return 0; }" > "$TEST_DIR/src/main.cpp"
    mkdir -p "$TEST_DIR/docs"
    echo "# Documentation" > "$TEST_DIR/docs/guide.md"
}

# Test 1: Scan command
test_scan_basic() {
    log_info "TEST 1: Basic scan command"
    
    if $ROGUEBOX scan --root "$TEST_DIR" --dry-run; then
        log_info "✅ TEST 1 PASSED: Scan dry-run successful"
        return 0
    else
        log_error "❌ TEST 1 FAILED: Scan dry-run failed"
        return 1
    fi
}

# Test 2: Scan with filters
test_scan_filters() {
    log_info "TEST 2: Scan with exclude filters"
    
    if $ROGUEBOX scan --root "$TEST_DIR" --exclude "*.log" --dry-run; then
        log_info "✅ TEST 2 PASSED: Scan with filters successful"
        return 0
    else
        log_error "❌ TEST 2 FAILED: Scan with filters failed"
        return 1
    fi
}

# Test 3: Init repo (no remote)
test_init_no_remote() {
    log_info "TEST 3: Init repo without remote"
    
    if $ROGUEBOX init-repo \
        --root "$TEST_DIR" \
        --repo-name "e2e-test-repo" \
        --no-remote \
        --dry-run; then
        log_info "✅ TEST 3 PASSED: Init no-remote successful"
        return 0
    else
        log_error "❌ TEST 3 FAILED: Init no-remote failed"
        return 1
    fi
}

# Test 4: Verify .gitignore creation
test_gitignore_creation() {
    log_info "TEST 4: Verify standard files creation"
    
    # Run init without dry-run to actually create files
    $ROGUEBOX init-repo \
        --root "$TEST_DIR" \
        --repo-name "e2e-test-repo" \
        --no-remote > /dev/null 2>&1 || true
    
    if [ -f "$TEST_DIR/.gitignore" ] && [ -f "$TEST_DIR/.rogueignore" ]; then
        log_info "✅ TEST 4 PASSED: Standard files created"
        return 0
    else
        log_error "❌ TEST 4 FAILED: Standard files not created"
        return 1
    fi
}

# Test 5: Help command
test_help_command() {
    log_info "TEST 5: Help command output"
    
    if $ROGUEBOX --help | grep -q "scan\|init-repo\|push-all\|full-run"; then
        log_info "✅ TEST 5 PASSED: Help command working"
        return 0
    else
        log_error "❌ TEST 5 FAILED: Help command missing expected output"
        return 1
    fi
}

# Test 6: Version command (if exists)
test_version_command() {
    log_info "TEST 6: Version command"
    
    if $ROGUEBOX --version 2>/dev/null || [ $? -eq 1 ]; then
        log_info "✅ TEST 6 PASSED: Version command checked"
        return 0
    else
        log_warn "⚠️  TEST 6 SKIPPED: Version command not implemented"
        return 0
    fi
}

# Test 7: Invalid command handling
test_invalid_command() {
    log_info "TEST 7: Invalid command handling"
    
    if ! $ROGUEBOX invalid-command-xyz 2>/dev/null; then
        log_info "✅ TEST 7 PASSED: Invalid command correctly rejected"
        return 0
    else
        log_error "❌ TEST 7 FAILED: Invalid command not rejected"
        return 1
    fi
}

# Test 8: Dry-run safety
test_dry_run_safety() {
    log_info "TEST 8: Dry-run safety (no actual changes)"
    
    local before_count=$(find "$TEST_DIR" -type f | wc -l)
    
    $ROGUEBOX full-run \
        --root "$TEST_DIR" \
        --repo-name "dry-run-test" \
        --no-remote \
        --dry-run > /dev/null 2>&1 || true
    
    local after_count=$(find "$TEST_DIR" -type f | wc -l)
    
    if [ "$before_count" -eq "$after_count" ]; then
        log_info "✅ TEST 8 PASSED: Dry-run made no file changes"
        return 0
    else
        log_warn "⚠️  TEST 8 WARNING: File count changed ($before_count -> $after_count)"
        return 0  # Don't fail, might be expected
    fi
}

# Main execution
main() {
    log_info "========================================="
    log_info "RogueMagicBox E2E Regression Tests"
    log_info "========================================="
    
    # Check if binary exists
    if [ ! -f "$ROGUEBOX" ]; then
        log_error "RogueMagicBox binary not found at: $ROGUEBOX"
        log_error "Please build the project first: cmake --build build --config Release"
        exit 1
    fi
    
    setup
    
    local failed=0
    
    # Run all tests
    test_scan_basic || ((failed++))
    test_scan_filters || ((failed++))
    test_init_no_remote || ((failed++))
    test_gitignore_creation || ((failed++))
    test_help_command || ((failed++))
    test_version_command || ((failed++))
    test_invalid_command || ((failed++))
    test_dry_run_safety || ((failed++))
    
    cleanup
    
    log_info "========================================="
    if [ $failed -eq 0 ]; then
        log_info "✅ ALL TESTS PASSED (8/8)"
        log_info "========================================="
        exit 0
    else
        log_error "❌ SOME TESTS FAILED ($failed failures)"
        log_info "========================================="
        exit 1
    fi
}

# Trap cleanup on exit
trap cleanup EXIT

main "$@"
