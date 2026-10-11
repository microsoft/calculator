#!/bin/bash
# AUDIT run_all.sh — every automated check this audit produced, from a clean
# checkout, with only the documented dependencies. Non-zero exit on any failure.
#
#   ./AUDIT/run_all.sh
#
# Optional: CALC_SNAPSHOT_DIR=<dir> also writes rendered PNGs of every mode.
set -uo pipefail
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
LOGDIR="$ROOT/AUDIT/evidence/logs"
mkdir -p "$LOGDIR"
FAIL=0
step() { echo; echo "=== $* ==="; }
check() { if [ "$1" -ne 0 ]; then echo "FAIL: $2 (exit $1)"; FAIL=1; else echo "OK: $2"; fi }

cd "$ROOT" || exit 1

step "1/8 engine build (CMake, documented steps)"
cmake -S linux/src/CalcManager.Linux -B linux/build/calc-manager -DCMAKE_BUILD_TYPE=Release > "$LOGDIR/run_engine_build.log" 2>&1
check $? "cmake configure"
cmake --build linux/build/calc-manager -j"$(nproc)" >> "$LOGDIR/run_engine_build.log" 2>&1
check $? "cmake build"

step "2/8 engine smoke (ctest)"
ctest --test-dir linux/build/calc-manager 2>&1 | tee "$LOGDIR/run_ctest.log"
check ${PIPESTATUS[0]} "ctest"

step "3/8 managed build"
( cd linux && dotnet build Calculator.Linux.slnx ) > "$LOGDIR/run_dotnet_build.log" 2>&1
check $? "dotnet build"

step "4/8 full test suite (engine + VM + headless UI)"
( cd linux && dotnet test Calculator.Linux.slnx --filter "Category!=IsolatedProcess" ) 2>&1 | tee "$LOGDIR/run_dotnet_test.log"
check ${PIPESTATUS[0]} "dotnet test"

step "5/8 locale display tests, in their own process"
# The unmodified engine installs its separator resource provider globally on the
# first instance built in the process and cannot change it afterwards (the
# member is `const`). Locale-dependent display assertions are therefore only
# meaningful in a fresh process - see LocaleMatrixTests for the full reasoning.
( cd linux && dotnet test Calculator.Linux.slnx --filter "Category=IsolatedProcess" ) 2>&1 | tee "$LOGDIR/run_dotnet_test_locale.log"
check ${PIPESTATUS[0]} "dotnet test (isolated locale process)"

step "6/8 packaging"
./linux/packaging/build-deb.sh 1.0.0 > "$LOGDIR/run_deb.log" 2>&1
check $? "build-deb.sh (must exit 0 on success - see AUD-S3-002)"
DEB="$ROOT/linux/publish/calculator-linux_1.0.0_amd64.deb"
[ -s "$DEB" ]; check $? "deb produced"
rm -rf /tmp/opencode/auditdeb && mkdir -p /tmp/opencode/auditdeb
dpkg-deb -x "$DEB" /tmp/opencode/auditdeb > /dev/null 2>&1
check $? "deb extracts"
command -v lintian > /dev/null && { lintian "$DEB" 2>&1 | tee "$LOGDIR/run_lintian.log"; echo "(lintian output above; empty = clean)"; }

step "7/8 data-parity checks vs the reference"
python3 "$ROOT/AUDIT/parity_check.py" 2>&1 | tee "$LOGDIR/run_parity.log"
check ${PIPESTATUS[0]} "converter + resource parity"

echo
if [ "$FAIL" -ne 0 ]; then echo "########## AUDIT RUN: FAILURES PRESENT ##########"; exit 1; fi
echo "########## AUDIT RUN: ALL CHECKS GREEN ##########"