# AUDIT REPORT — Linux port of Windows Calculator

**Auditor role:** independent QA lead / release auditor.
**Project root:** `/home/elshad-guliyev/Documents/calculator`, branch `linux-port`.
**Reference:** `github.com/microsoft/calculator` @ `d125246a4e19842ce1332e6c7839cf0e110027d8`
cloned read-only to `/home/elshad-guliyev/ref-calculator` (never modified).
**Mode:** AUDIT_AND_FIX.
**Date:** 2026-10-11.

---

## 1. Verdict

# NOT READY — but closer than the raw defect count suggests

One **S2** was found and fixed; the rest of what I verified is solid. The
release is **blocked on two things that are not code defects**:

1. **BLOCKER-1 (environment):** on-screen UI-RUN could not be performed. The
   Wayland session's compositor refuses to map *any* new X11 toplevel — proven
   with a stock `xmessage`. The app is not the cause; the pre-audit build fails
   identically. Requires a logout/login, then the manual script in §7.
2. **BLOCKER-2 (incomplete audit):** Phases 3–5 are only partially done. Keyboard
   shortcuts, locale matrix, scaling, accessibility, soak/performance and the
   remaining packaging/licensing rows are **not verified**. See §6.

Do not read "NOT READY" as "the calculator is wrong". Read it as "the audit is
not finished, and I could not look at the running window."

---

## 2. Counts

### By final status

| Status | Count |
|---|---|
| PASS (verified with evidence) | 25 |
| FIXED + RETESTED | 3 |
| DEVIATION (documented, user decision) | 4 |
| BLOCKED (environment) | 1 class (all on-screen UI items) |
| Not yet reached (see §6) | ~8 checklist areas |

### By method

| Method | Items |
|---|---|
| ENGINE-TEST | 14 |
| UI-RUN (Avalonia.Headless + Skia, real window, real engine) | 6 |
| SHELL / clean-checkout build | 4 |
| CODE-READ-ONLY (counts as unverified per the brief) | 6 |

Automated suite after this audit: **210 managed tests + 1 C++ ctest, green.**

---

## 3. Defects found, by severity

### S2 — missing / non-functional controls (1 defect, 39 controls)

**AUD-S2-001 — Scientific keypad missing 34 of 59 reference functions;
Programmer missing 5 bitwise operations.** FIXED in commit `51ff26d`.

The port shipped with `sec csc cot`, every inverse and hyperbolic function,
`x³ ∛x ʸ√x 2ˣ log_b ⌊x⌋ ⌈x⌋ rand →dms →deg` and `NAND NOR L⇥` unreachable.
A ViewModel-only test suite cannot see this, because **the ViewModel and the
native Ratpack engine already computed every one of them correctly** — I proved
it by driving the unwired enum values straight through the ViewModel:

```
sec(30°)   = 1.1547005383792515290182975610039     (= 1/cos30 ✓)
asinh(1)   = 0.88137358701954302523260932497979    ✓
e          = 2.7182818284590452353602874713527     (30 sig digits — arbitrary precision ✓)
```

Fix is XAML-only. Regression guard added:
`linux/tests/Calculator.Linux.Tests/Audit/KeypadCompletenessTests.cs` walks the
real window's visual tree per mode and fails if any reference `ButtonId` is not
bound to an on-screen button. It fails on the pre-fix tree.

### S3 — behaviour / build-contract deviations (2 defects)

**AUD-S3-002 — `build-deb.sh` exited 2 on success.** The last statement was
`dpkg-deb --contents "$DEB" | head -n 12`; `head` closes the pipe, `tar` takes
SIGPIPE, and that became the script's exit status. Every successful packaging
run printed `dpkg-deb: error: tar subprocess was killed by signal (Broken pipe)`
and returned non-zero — which breaks any `build-deb.sh && deploy` chain.
Fixed (`|| true` on the informational pipeline); verified `exit=0`.

**AUD-S3-003 — Currency dropdowns showed mojibake** (`ᏌᏊ ᎢᏳᎾᎵᏍᏔᏅ - US Dollar`,
`España - Euro`). `RegionInfo.DisplayName` follows the UI culture and the USD
region was picked from `CultureInfo.GetCultures`, whose first `US` entry is the
Cherokee culture `chr-US`. Fixed in `f1d4e93` via `EnglishName` + deterministic
region preference. Re-verified independently in this audit.

### S1 — none found

No wrong-result, crash, hang or data-loss defect was found in anything I was
able to execute.

### S4 — none filed

---

## 4. What I verified correct (the good news, with evidence)

**The arithmetic engine is the real thing.** The port compiles
`src/CalcManager` — *including `Ratpack`* — unmodified via CMake, behind a thin
frozen C ABI, and the managed side reaches it by P/Invoke. Evidence:

| Check | Result |
|---|---|
| `0.1+0.2` | `0.3` (exact, no IEEE residue) |
| `1/3*3` | `1` |
| `sqrt(2.25) - 1.5` | `0` |
| `170!` | `7.2574156153079989673967282111293e+306` — exact, beyond `double` range |
| `10000!` | `Overflow` |
| `e` | 30 significant digits |
| History cap | 20 (matches `MAX_HISTORY_ITEMS`) |

**The unit converter data is byte-faithful** — the single highest-risk
hand-transcription in the project:

```
unit-id constants: ref=158 port=158  missing=0 extra=0 value_mismatch=0
AddUnit calls:     ref=105 port=105  identical_sets=True
conversion factors ref=154 port=154  identical_sets=True
```

**Both resource tables are exact**: `Resources.resw` 1166/1166, 0 mismatches;
`CEngineStrings.resw` 108/108, 0 mismatches; 0 duplicate keys in either.

**Clean-checkout build works** from documented steps only, no undocumented
dependencies: `cmake`=0, `ctest`=0, `dotnet build`=0, `dotnet test`=0, `.deb`
produced and extracts (232 files).

**No unfinished work**: no `NotImplementedException`, no blocking
`.Result`/`.Wait()`/`Thread.Sleep` on the UI thread, 4 empty catches all
narrow and justified, 4 TODO-ish hits all justified in place.

---

## 5. Two findings that change the contract

**The reference's Graphing mode is 100% MOCK.** All of `src/GraphingImpl` lives
in `src/GraphingImpl/Mocks/` under namespace `MockGraphingImpl`.
`ParseInput` returns a `MockExpression` for *any* input, `DrawD2D1` renders
nothing and returns `S_OK`, `GetAnalyzer` returns `nullptr`, `Serialize`
returns `L""`. There are no `throw`s — it **fails silently while reporting
success**. Therefore numeric parity for graphing is impossible and
meaningless; the port's real LiveCharts2 implementation is a superset, not a
deviation to be reconciled.

**The reference's currency path is 100% MOCK.** `CurrencyHttpClient.cs` holds
two hard-coded JSON literals of *fictional planet currencies* (Mars, Moon,
Neptune, Saturn, Uranus, Venus, Jupiter, Mercury). Both methods
`return Task.FromResult(<literal>)`. The `HttpClient` is constructed and never
used. The source comment says: *"Mock data matching the C++ open-source
calculator behavior (fictional planet currencies). The upstream fwlink URLs are
dead."* **There is no base URL, no endpoint constant, no `Uri`, no HTTP verb
anywhere in `DataLoaders/`.** The port's live Frankfurter/ECB feed is likewise
a superset, not a deviation.

This means the brief's instruction to "check which parts of the reference are
real and which are mock" has a decisive answer for the two riskiest areas — and
both are areas where the port is *better* than the reference.

---

## 6. Could not verify — and why

| Area | Why | Status |
|---|---|---|
| **On-screen UI-RUN (all mouse/keyboard/resize/a11y items)** | Wayland compositor will not map new X11 toplevels; stock `xmessage` fails identically. Avalonia 11.3 is X11-only, so the app needs Xwayland. | Needs logout/login → §7 script |
| **Keyboard shortcut parity** | Reference declares **220** `KeyboardShortcutManager` bindings (79 Character, 76 VirtualKey, 28 Ctrl chords, 24 Shift chords, 13 Ctrl+Shift). The port maps keys in code with a far smaller table. **Likely S3; not yet diffed.** | NOT VERIFIED — highest-priority remaining item |
| **Locale matrix** (de_DE, ru_RU, en_IN, RTL) | Not run. Engine/formatting are locale-aware and one de_DE case passes, but the full matrix is outstanding. | NOT VERIFIED |
| **Scaling 125/150/200%, fractional** | Needs a live compositor. | NOT VERIFIED |
| **Accessibility / AT-SPI / Orca** | Needs a live session bus + compositor. | NOT VERIFIED |
| **Soak / performance / RSS growth** | Not run. | NOT VERIFIED |
| **Licence audit of every NuGet transitive dep** | Not run. | NOT VERIFIED |
| **Golden vectors from the reference's 534 tests** | Extracted to `AUDIT/evidence/reference_golden_vectors.md` (952 KB, verbatim) but **not yet replayed against the port**. | Extracted, NOT REPLAYED |
| **Differential fuzz (10k sequences/mode vs an oracle)** | No independent oracle buildable (reference needs Windows); deferred. | NOT RUN |

I want to be plain about this: the reference has **534 test methods** whose
bodies are now sitting in my evidence directory, and I have not yet turned them
into assertions against the port. That is the single biggest remaining piece of
work and it is mechanical, not hard.

---

## 7. Manual script for the UI items I could not run

After `log out` / `log in`, then:

```bash
APP=~/Documents/calculator/linux/src/Calculator.Linux/bin/Debug/net10.0/Calculator.Linux
$APP & sleep 4
W=$(xdotool search --name "^Calculator$" | tail -1)
xwininfo -id $W | grep "Map State"        # expect IsViewable
```

Then, per mode (Standard / Scientific / Programmer / Date / Currency / one
converter / Graphing):

1. Every button: click it, confirm the display and the enabled/disabled state
   of the others change as Windows Calculator does.
2. **Scientific: confirm the newly restored functions work** —
   `sec(30)=1.1547005383792515290182975610039`, `asinh(1)=0.88137358701954302523260932497979`,
   `e=2.7182818284590452353602874713527`, and that the keypad scrolls.
3. **Programmer**: `NAND`, `NOR`, `L⇥`, rotate-through-carry.
4. Keyboard: digits/operators/Enter/Esc/Backspace/F9 on the numpad and top row;
   confirm keys are not swallowed by a focused button.
5. Resize 340 → 1400 wide in every mode; nothing clipped.
6. Toggle dark mode live (`gsettings set org.gnome.desktop.interface color-scheme prefer-dark`).
7. Run the app under `orca` and Tab through every control.

---

## 8. Reproduce everything

```bash
cd ~/Documents/calculator
./AUDIT/run_all.sh          # clean-checkout equivalent: engine, ctest, dotnet build+test, deb, parity
python3 AUDIT/parity_check.py
cd linux && dotnet test Calculator.Linux.slnx
```

`run_all.sh` exits non-zero if any check fails.

**Final full run (this audit, after the S2 fix):**

```
=== 1/6 engine build ===   OK: cmake configure / OK: cmake build
=== 2/6 engine smoke ===   100% tests passed, 0 tests failed out of 1   OK: ctest
=== 3/6 managed build ===  OK: dotnet build
=== 4/6 full test suite == Passed! - Failed: 0, Passed: 210              OK: dotnet test
=== 5/6 packaging ===     OK: build-deb.sh / OK: deb produced / OK: deb extracts
=== 6/6 data parity ===    OK: converter + resource parity
########## AUDIT RUN: ALL CHECKS GREEN ##########
```

---

## 9. Artifacts

| Path | What |
|---|---|
| `AUDIT/DEFECTS.md` | 3 defects, full repro/root-cause/fix; plus 4 retracted false positives |
| `AUDIT/TEST_MATRIX.csv` | 29 rows, ID → steps → expected → expected-source → actual → method → result |
| `AUDIT/DECISIONS.md` | 4 deviations, 1 assumption, 1 environment blocker |
| `AUDIT/INVENTORY.md` | reference contract (see below) |
| `AUDIT/evidence/reference_golden_vectors.md` | 534 reference tests, verbatim, 952 KB |
| `AUDIT/evidence/reference_inventory_raw.md` | 3264 rows: buttons, shortcuts, limits, data |
| `AUDIT/evidence/logs/` | clean build, parity, lintian, unfinished-work hunt |
| `AUDIT/parity_check.py`, `AUDIT/run_all.sh`, `AUDIT/clean_build.sh` | automation |