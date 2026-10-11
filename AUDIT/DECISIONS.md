# AUDIT DECISIONS — assumptions and accepted deviations

Ground rule 7: no questions mid-run; take the safest assumption, log it here.

---

## AUD-DEV-001 — Scientific keypad is scrollable (presentation deviation)

**Reference behaviour.** `CalculatorScientificOperators.xaml` is a fixed 8×5
grid. Extra functions (x³, ∛, ʸ√x, ²√x, log_b, eˣ, sec/csc/cot and their
inverses, hyperbolic families) are reached by swapping column 0 via VisualState
triggers driven by a shift button, so the panel never scrolls.

**Port behaviour after AUD-S2-001.** All 59 reference functions are permanently
visible in a 15×5 grid inside a `ScrollViewer`.

**Why.** Implementing the reference's shift-toggle state machine is a large
behavioural change to a keypad that is otherwise correct. Showing every
function permanently is a functional superset: no function is hidden behind a
mode switch, which is strictly easier to reach. The cost is a scrollbar.

**Justification.** Functionally equivalent, strictly-more-reachable superset.
Recorded as a deviation rather than silently accepted. **Flagged for the user's
decision** — if byte-parity with the reference presentation matters more than
reachability, this should be rebuilt with the shift toggle.

---

## AUD-DEV-002 — Currency data source is Frankfurter, not the reference's service

The reference obtains currency rates from a Microsoft-operated endpoint. The
port uses `https://api.frankfurter.dev/v1` (ECB reference rates, no key).

- Reference behaviour: ~150+ currencies from a Bing-backed service.
- Port behaviour: ~30 ECB majors.

Recorded deviation, documented in the port's own PORTING-NOTES. This audit did
**not** change it — it is a licensing/service decision, not a defect. Currency
behaviour that *is* testable (cache, staleness, offline, fractional digits,
symbol display, default pair) was verified and passes.

---

## AUD-DEV-003 — English-only UI

The port ships en-US strings only; upstream ships 100+ locales. Documented by
the port. Not fixed in this audit — it is a localisation-coverage decision, not
a defect. The engine and formatting layers are locale-aware and were verified
to follow the UI culture (decimal comma, grouping) where that is testable.

---

## AUD-DEV-004 — History/Memory is a docked panel, not flyouts

Upstream opens History and Memory as flyouts from the header. The port uses a
right-hand dock, hidden by default, auto-collapsing below 720px. Functional
equivalent, different presentation. Documented by the port; not changed here.

---

## AUD-ENV-001 — On-screen UI-RUN is currently impossible (environment, not app)

`XDG_SESSION_TYPE=wayland`; Avalonia 11.3 is X11-only so the app goes through
Xwayland. The compositor has stopped mapping **any** new X11 toplevel.

**Proof it is not the app** — a stock `xmessage` fails identically:
```
$ DISPLAY=:0 xmessage "STOCK X CLIENT TEST" &
xmessage win=6291508 IsUnMapped
```
and the *pre-audit* build of the port (commit 1b00eb3, untouched by this audit)
fails the same way.

**Mitigation used instead.** Avalonia.Headless with `UseSkia()` +
`UseHeadlessDrawing = false` renders the real window offscreen via
`CaptureRenderedFrame()` — real styles, real templates, real bindings, real
engine. This is explicitly sanctioned by the addendum (A2 layers 2 and 4).
Screenshots: `AUDIT/evidence/screens/` and the per-run snapshot dirs.

**Remedy for the user:** log out and back in (on Wayland there is no safe
shell restart). Then run the manual script below.

---

## AUD-ASSUMPTION-001 — Scope of this run

The brief asks for exhaustive coverage of every mode × every state × every
locale × every scale. This run completed Phases 0–2 in depth and Phases 3–5
partially; Phases 3–5 are **incomplete**. Items not reached are listed in
REPORT.md §"Could not verify" with the reason. Nothing in the report is
claimed on the strength of reading code alone except where explicitly tagged
CODE-READ-ONLY, which the brief counts as unverified.