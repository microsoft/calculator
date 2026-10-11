#!/bin/bash
# AUDIT Phase 3: drive the REAL app on the live X display with real mouse and
# keyboard input, capturing every mode.
#
# Flyout geometry was measured from a live session: the nav Popup is its own X
# toplevel 240x620 anchored below the hamburger, rows are 39px apart starting
# 25px down. Row order is the reference's CategoryManifest.
#
# Usage: ui_drive.sh [outdir]
set -u
OUT=${1:-$(dirname "$0")/evidence/screens}
BIN=$(dirname "$0")/../linux/src/Calculator.Linux/bin/Debug/net10.0/Calculator.Linux
BIN=$(cd "$(dirname "$BIN")" && pwd)/$(basename "$BIN")
mkdir -p "$OUT"
export DISPLAY=${DISPLAY:-:0}

pkill -x Calculator.Linu 2>/dev/null
sleep 1
setsid "$BIN" > /tmp/opencode/app.log 2>&1 < /dev/null &
sleep 10

W=$(xdotool search --name "Calculator" | tail -1)
if [ -z "$W" ]; then echo "NO WINDOW"; tail -5 /tmp/opencode/app.log; exit 1; fi
INFO=$(xwininfo -id "$W")
X=$(echo "$INFO" | awk -F: '/Absolute upper-left X/{gsub(/ /,"",$2);print $2}')
Y=$(echo "$INFO" | awk -F: '/Absolute upper-left Y/{gsub(/ /,"",$2);print $2}')
xdotool windowactivate "$W"; sleep 1
echo "window $W at $X,$Y"

cap() { import -window "$W" "$OUT/$1.png" 2>/dev/null && echo "  $1"; }
type_() { xdotool type --delay 60 "$1"; sleep 0.8; }
key() { xdotool key "$1"; sleep "${2:-0.9}"; }
click() { xdotool mousemove $((X + $1)) $((Y + $2)) click 1; sleep "${3:-1.2}"; }

# Nav via the reference's Ctrl+Alt+1..5 mode shortcuts (AUD-S3-007).
mode_key() { xdotool key ctrl+alt+"$1"; sleep 2.0; }

# Nav to any flyout row by INDEX (0-based, reference CategoryManifest order).
# The flyout is a separate X toplevel, so its position cannot be derived from the
# window origin: find it, then click row-relative coordinates inside it.
flyout_popup() {
  xwininfo -root -children | awk '/\(has no name\): \(\)  240x620/{print $1}' | tail -1
}

open_flyout() {
  local pop
  for attempt in 1 2 3; do
    click 24 24 1.0
    pop=$(flyout_popup)
    [ -n "$pop" ] && { echo "$pop"; return 0; }
    sleep 1.0
  done
  return 1
}

nav() { # $1 = row index
  local pop
  pop=$(open_flyout) || { echo "  !! flyout would not open (row $1)"; return 1; }
  local px py
  px=$(xdotool getwindowgeometry "$pop" | sed -n 's/.*Position: \([0-9]*\),.*/\1/p')
  py=$(xdotool getwindowgeometry "$pop" | sed -n 's/.*Position: [0-9]*,\([0-9]*\).*/\1/p')
  xdotool mousemove $((px + 120)) $((py + 25 + 39 * $1)) click 1
  sleep 2.0
}

echo "== every mode, in the reference's order =="
cap "00-launch"
mode_key 1; cap "01-standard"
mode_key 2; cap "02-scientific"
mode_key 3; cap "03-graphing"
mode_key 4; cap "04-programmer"
mode_key 5; cap "05-date"
nav 5; cap "06-currency"
nav 6; cap "07-volume"
nav 17; cap "08-angle"
nav 18; cap "09-about"
mode_key 1; cap "10-back-to-standard"

echo "== real keyboard: 2 + 3 = =="
type_ "2+3"
key Return 1.4
cap "11-keyboard-2plus3"

echo "== reference character shortcut (Scientific sin via 's') =="
mode_key 2
type_ "90"
key s 1.4
cap "12-scientific-sin-90"

echo "== Ctrl+Alt+4 jumps to Programmer =="
xdotool key ctrl+alt+4; sleep 2
cap "13-ctrl-alt-4-programmer"

echo "== responsive widths =="
mode_key 1
xdotool windowsize "$W" 340 520; sleep 2.5; cap "14-narrow-340"
xdotool windowsize "$W" 1400 950; sleep 2.5; cap "15-wide-1400"
xdotool windowsize "$W" 768 640; sleep 2.5; cap "16-medium-768"
xdotool windowsize "$W" 960 680; sleep 2
echo DONE