#!/bin/bash
# Builds the calculator-linux .deb (Task 10). Self-contained .NET publish +
# native engine side-by-side, packaged with dpkg-deb (no extra tooling).
# Usage: ./linux/packaging/build-deb.sh [version]  (default 1.0.0)
set -euo pipefail

VERSION="${1:-1.0.0}"
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
LINUX="$ROOT/linux"
PUBLISH="$LINUX/publish/app"
STAGE="$LINUX/publish/debroot"
DEB="$LINUX/publish/calculator-linux_${VERSION}_amd64.deb"

echo "==> dotnet publish (linux-x64, self-contained, Release)"
dotnet publish "$LINUX/src/Calculator.Linux/Calculator.Linux.csproj" \
    -c Release -r linux-x64 --self-contained \
    -o "$PUBLISH" /p:Version="$VERSION"

echo "==> stage native engine next to the managed binaries"
cp -f "$LINUX/build/calc-manager/libCalcManager.so.1.0.0" "$PUBLISH/libCalcManager.so"

echo "==> verify payload"
test -x "$PUBLISH/Calculator.Linux"
test -f "$PUBLISH/libCalcManager.so"
test -f "$PUBLISH/DefaultFromToCurrency.json"
ls "$PUBLISH" | head -n 5

echo "==> assemble deb tree"
rm -rf "$STAGE"
mkdir -p "$STAGE/DEBIAN" \
    "$STAGE/usr/lib/calculator-linux" \
    "$STAGE/usr/share/applications" \
    "$STAGE/usr/share/icons/hicolor/scalable/apps"
cp -r "$PUBLISH/." "$STAGE/usr/lib/calculator-linux/"
cp "$LINUX/packaging/calculator-linux.desktop" "$STAGE/usr/share/applications/"
cp "$LINUX/packaging/icon.svg" "$STAGE/usr/share/icons/hicolor/scalable/apps/calculator-linux.svg"
cat > "$STAGE/DEBIAN/control" <<EOF
Package: calculator-linux
Version: $VERSION
Section: utils
Priority: optional
Architecture: amd64
Maintainer: Linux port (from microsoft/calculator, MIT)
Description: Calculator for Linux
 Native Linux port of Windows Calculator: standard, scientific and
 programmer modes, date calculations, unit and live-currency converters,
 and graphing. Self-contained (.NET runtime bundled).
EOF

echo "==> dpkg-deb --build"
dpkg-deb --root-owner-group --build "$STAGE" "$DEB"
echo "Built: $DEB"
dpkg-deb --contents "$DEB" | head -n 12
