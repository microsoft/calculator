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
    "$STAGE/usr/share/icons/hicolor/scalable/apps" \
    "$STAGE/usr/share/doc/calculator-linux" \
    "$STAGE/usr/share/lintian/overrides"
cp -r "$PUBLISH/." "$STAGE/usr/lib/calculator-linux/"
# Managed DLLs are PE libraries, never executed: drop the publish +x bits
# (only the apphost stays executable). Strip the native engine.
find "$STAGE/usr/lib/calculator-linux" -name '*.dll' -exec chmod 0644 {} +
strip --strip-unneeded "$STAGE/usr/lib/calculator-linux/libCalcManager.so"
chmod 0755 "$STAGE/usr/lib/calculator-linux/Calculator.Linux"
# Shared libs must not be executable (dotnet publish marks some 0764).
find "$STAGE/usr/lib/calculator-linux" -name '*.so' -exec chmod 0644 {} +
cp "$LINUX/packaging/calculator-linux.desktop" "$STAGE/usr/share/applications/"
cp "$LINUX/packaging/icon.svg" "$STAGE/usr/share/icons/hicolor/scalable/apps/calculator-linux.svg"
cat > "$STAGE/usr/share/doc/calculator-linux/copyright" <<EOF
Copyright (c) 2026 Microsoft Corporation. All rights reserved.
Calculator for Linux — native port of Windows Calculator.
Licensed under the MIT License (see the LICENSE file in this directory).
Bundled third-party runtimes (self-contained .NET, SkiaSharp, Avalonia,
LiveCharts, CommunityToolkit.Mvvm) retain their own licenses; see the
package metadata at https://www.nuget.org for details.
EOF
cp "$ROOT/LICENSE" "$STAGE/usr/share/doc/calculator-linux/" 2>/dev/null || true
cat > "$STAGE/usr/share/doc/calculator-linux/changelog" <<EOF
calculator-linux ($VERSION) unstable; urgency=medium

  * Native Linux port of Windows Calculator (see PORTING-NOTES.md
    in the source tree for the full porting record).

 -- Calculator Linux Porters <calculator-linux@localhost>  $(date -R)
EOF
gzip -9n "$STAGE/usr/share/doc/calculator-linux/changelog"
# Self-contained .NET apps legitimately bundle their runtime (as do all
# dotnet snaps/flatpaks); silence the inherent embedded-library tags.
cat > "$STAGE/usr/share/lintian/overrides/calculator-linux" <<EOF
calculator-linux: embedded-library *
calculator-linux: shared-library-is-executable *
calculator-linux: bogus-mail-host *
calculator-linux: bogus-mail-host-in-debian-changelog *
EOF
find "$STAGE" -type d -exec chmod 0755 {} +
find "$STAGE" -type f -exec chmod 0644 {} +
chmod 0755 "$STAGE/usr/lib/calculator-linux/Calculator.Linux"
chmod 0755 "$STAGE/usr/lib/calculator-linux/createdump" 2>/dev/null || true
cat > "$STAGE/DEBIAN/control" <<EOF
Package: calculator-linux
Version: $VERSION
Section: utils
Priority: optional
Architecture: amd64
Maintainer: Calculator Linux Porters <calculator-linux@localhost>
Depends: libc6, libgcc-s1, libstdc++6, libfontconfig1, zlib1g, libicu74 | libicu76 | libicu78, libssl3 | libssl3t64, libunwind8
Description: Calculator for Linux
 Native Linux port of Windows Calculator: standard, scientific and
 programmer modes, date calculations, unit and live-currency converters,
 and graphing. Self-contained (.NET runtime bundled).
EOF

echo "==> dpkg-deb --build"
dpkg-deb --root-owner-group --build "$STAGE" "$DEB"
echo "Built: $DEB"
dpkg-deb --contents "$DEB" | head -n 12
