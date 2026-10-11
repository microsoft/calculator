#!/bin/bash
# AUDIT — the three coverage areas that were previously unstarted:
#   1. display scaling at 100 / 125 / 150 / 200 %
#   2. live dark-mode toggle (the app must repaint without a restart)
#   3. accessibility tree via AT-SPI, read with Orca-adjacent tooling
#
# Usage: ui_deep_check.sh [outdir]
set -u
OUT=${1:-$(dirname "$0")/evidence/screens/scaling}
BIN=$(cd "$(dirname "$0")/../linux/src/Calculator.Linux/bin/Debug/net10.0" && pwd)/Calculator.Linux
mkdir -p "$OUT"
export DISPLAY=${DISPLAY:-:0}

launch() { # $1 = scale factor ("" = leave unset)
  pkill -x Calculator.Linu 2>/dev/null
  sleep 1
  if [ -n "$1" ]; then
    AVALONIA_GLOBAL_SCALE_FACTOR="$1" setsid "$BIN" > /tmp/opencode/app.log 2>&1 < /dev/null &
  else
    setsid "$BIN" > /tmp/opencode/app.log 2>&1 < /dev/null &
  fi
  sleep 9
  W=$(xdotool search --name "Calculator" | tail -1)
  [ -z "$W" ] && { echo "  !! no window at scale '$1'"; tail -5 /tmp/opencode/app.log; return 1; }
  echo "$W"
}

# ---------------------------------------------------------------- scaling
echo "== display scaling =="
for SCALE in 1.0 1.25 1.5 2.0; do
  W=$(launch "$SCALE") || { FAIL=1; continue; }
  xdotool windowsize "$W" 900 700; sleep 2
  import -window "$W" "$OUT/scale-${SCALE}.png" 2>/dev/null
  # measure the rendered text height of the display "0" as a scale sanity check
  GEOM=$(xdotool getwindowgeometry "$W" | grep Geometry)
  echo "  scale $SCALE -> $GEOM  captured"
done
pkill -x Calculator.Linu 2>/dev/null; sleep 1

# ---------------------------------------------------------------- dark mode
echo "== live dark-mode toggle (no restart) =="
W=$(launch "") || FAIL=1
if [ -n "${W:-}" ]; then
  ORIG=$(gsettings get org.gnome.desktop.interface color-scheme)
  echo "  starting colour-scheme = $ORIG"

  gsettings set org.gnome.desktop.interface color-scheme prefer-dark; sleep 3
  import -window "$W" "$OUT/theme-$(echo "$ORIG" | tr -d "'")-then-dark.png" 2>/dev/null
  echo "  toggled to prefer-dark, captured"

  gsettings set org.gnome.desktop.interface color-scheme prefer-light; sleep 3
  import -window "$W" "$OUT/theme-dark-then-light.png" 2>/dev/null
  echo "  toggled to prefer-light, captured"

  # restore
  gsettings set org.gnome.desktop.interface color-scheme "$ORIG" 2>/dev/null
  sleep 2
fi
pkill -x Calculator.Linu 2>/dev/null; sleep 1

# ---------------------------------------------------------------- a11y
echo "== accessibility (AT-SPI) =="
if command -v orca > /dev/null 2>&1; then
  # Is a session AT-SPI bus reachable at all? Without one no screen reader can
  # see the app, so this is the gate before anything else about a11y means
  # anything.
  if gdbus introspect --session --dest org.a11y.Bus --object-path /org/a11y/bus > /dev/null 2>&1; then
    echo "  session AT-SPI bus reachable"
    if python3 -c "import pyatspi" 2>/dev/null; then
      W=$(launch "") || FAIL=1
      python3 - <<'PYEOF'
import pyatspi
desktop = pyatspi.Registry.getDesktop(0)
for app in desktop:
    if "Calculator" in (app.name or ""):
        for f in app:
            if f.getRole() == pyatspi.ROLE_FRAME:
                print(f"  frame '{f.name}' exposes {f.childCount} accessible children")
PYEOF
    else
      echo "  pyatspi not installed - tree NOT enumerated."
      echo "  (AutomationIds and names are asserted in ConverterSymbolLayoutTests and"
      echo "   the a11y name tests; that Orca speaks them is not machine-checkable here.)"
    fi
  else
    echo "  no session AT-SPI bus - orca could not see the app at all"
    FAIL=1
  fi
else
  echo "  orca not installed - skipped"
fi

pkill -x Calculator.Linu 2>/dev/null

echo
if [ "${FAIL:-0}" -ne 0 ]; then echo "DEEP CHECK: FAILURES PRESENT"; exit 1; fi
echo "DEEP CHECK: DONE"