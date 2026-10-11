#!/bin/bash
# Clean-checkout build + test using ONLY the documented steps
# (linux/README.md "Build & run" and linux/docs/PORTING-NOTES.md).
set -u
ROOT=/home/elshad-guliyev/cleanbuild
LOG=/home/elshad-guliyev/Documents/calculator/AUDIT/evidence/logs
mkdir -p "$LOG"
exec > >(tee "$LOG/cleanbuild.log") 2>&1

echo "=== CLEAN CHECKOUT BUILD $(date -Is) ==="
echo "source: git archive HEAD of linux-port, extracted to $ROOT"
cd "$ROOT" || exit 1

echo
echo "### STEP 1: engine (CMake) - exactly as documented"
cmake -S linux/src/CalcManager.Linux -B linux/build/calc-manager -DCMAKE_BUILD_TYPE=Release
echo "cmake configure exit=$?"
cmake --build linux/build/calc-manager -j"$(nproc)"
echo "cmake build exit=$?"
ctest --test-dir linux/build/calc-manager
echo "ctest exit=$?"

echo
echo "### STEP 2: app (dotnet) - exactly as documented"
cd "$ROOT/linux" || exit 1
dotnet build Calculator.Linux.slnx
echo "dotnet build exit=$?"

echo
echo "### STEP 3: full test suite"
dotnet test Calculator.Linux.slnx
echo "dotnet test exit=$?"

echo
echo "### STEP 4: packaging (build-deb.sh)"
cd "$ROOT" || exit 1
./linux/packaging/build-deb.sh 1.0.0
echo "build-deb exit=$?"
ls -la linux/publish/*.deb 2>&1

echo
echo "### DONE $(date -Is)"