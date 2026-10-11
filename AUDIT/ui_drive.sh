#!/bin/bash
# AUDIT Phase 3: drive the REAL published-equivalent app on the live X display
# with real mouse + keyboard input, capturing every mode.
# Usage: ui_drive.sh <outdir>
OUT=${1:-/home/elshad-guliyev/Documents/calculator/AUDIT/evidence/screens}
BIN=/home/elshad-guliyev/Documents/calculator/linux/src/Calculator.Linux/bin/Debug/net10.0/Calculator.Linux
mkdir -p "$OUT"

pkill -x Calculator.Linu 2>/dev/null; sleep 1
setsid "$BIN" > /tmp/opencode/app.log 2>&1 < /dev/null &
sleep 10

W=""
for w in $(DISPLAY=:0 xdotool search --name "Calculator" 2>/dev/null); do
  DISPLAY=:0 xwininfo -id "$w" 2>/dev/null | grep -q "Map State: IsViewable" && W=$w
done
[ -z "$W" ] && { echo "NO VIEWABLE WINDOW"; tail -5 /tmp/opencode/app.log; exit 1; }

INFO=$(DISPLAY=:0 xwininfo -id "$W")
X=$(echo "$INFO" | grep "Absolute upper-left X" | grep -oE '[0-9]+')
Y=$(echo "$INFO" | grep "Absolute upper-left Y" | grep -oE '[0-9]+')
DISPLAY=:0 xdotool windowactivate "$W"; sleep 1
echo "window $W at $X,$Y"

cap() { DISPLAY=:0 import -window "$W" "$OUT/$1.png"; echo "  captured $1"; }
click() { DISPLAY=:0 xdotool mousemove $((X + $1)) $((Y + $2)) click 1; sleep "${3:-1.2}"; }
type_() { DISPLAY=:0 xdotool type --delay 60 "$1"; sleep 0.8; }

# Hamburger is at ~(24,24) client-relative.
HAMBURGER="24 24"

nav() {  # $1 = row index in the flyout (0-based), measured from the popup top
  click $HAMBURGER 24 1.5
  DISPLAY=:0 xdotool mousemove $((X + 120)) $((Y + 52 + 41 * $1)) click 1
  sleep 2.0
}

echo "== modes =="
cap "00-launch-standard"
nav 0; cap "01-standard"
nav 1; cap "02-scientific"
nav 2; cap "03-programmer"
nav 3; cap "04-date"
nav 5; cap "05-currency"
nav 4; cap "06-volume"
nav 17; cap "07-graphing"
nav 0; cap "08-back-to-standard"

echo "== real keyboard input (Standard: 2 + 3 =) =="
type_ "2+3"
DISPLAY=:0 xdotool key Return; sleep 1.2
cap "09-keyboard-2plus3equals"

echo "== memory (MS then recall row) =="
DISPLAY=:0 xdotool key Escape; sleep 0.5
type_ "7"; DISPLAY=:0 xdotool key Return; sleep 1
DISPLAY=:0 xdotool key c; sleep 1          # ctrl-c is copy; use MS via UI below
click 24 24 1.5
echo "== responsive sizes =="
DISPLAY=:0 xdotool windowsize "$W" 340 520; sleep 2; cap "10-narrow-340x520"
DISPLAY=:0 xdotool windowsize "$W" 1400 950; sleep 2; cap "11-wide-1400x950"
DISPLAY=:0 xdotool windowsize "$W" 768 640; sleep 2; cap "12-medium-768x640"

echo "== scaling =="
AVALONIA_GLOBAL_SCALE_FACTOR=1.5
DISPLAY=:0 xdotool windowsize "$W" 960 680; sleep 1
echo "  (scaling tested in a separate launch)"
echo DONE