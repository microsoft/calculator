# AUDIT REPORT — Linux port of Windows Calculator

**Auditor role:** independent QA lead / release auditor.
**Project root:** `/home/elshad-guliyev/Documents/calculator`, branch `linux-port`.
**Reference:** `github.com/microsoft/calculator` @ `d125246a4e19842ce1332e6c7839cf0e110027d8`
cloned read-only to `/home/elshad-guliyev/ref-calculator` (never modified).
**Mode:** AUDIT_AND_FIX.
**Date:** 2026-10-11.

---

## 1. Verdict

# NOT READY — one real blocker left, and it is no longer the one I expected

The on-screen blocker is **gone**: after a logout/login the X11 display mapped
windows normally, and the real application was driven end to end with `xdotool`
through every mode, at three window widths, with real mouse and keyboard input
(`AUDIT/ui_drive.sh`, captures in `AUDIT/evidence/screens/`). That pass found two
defects no structural test could see, and both are now fixed (AUD-S3-008) or
were already covered (AUD-S2-001 confirmed on screen).

What the audit has established:

- **Correctness is now grounded in Microsoft's own expectations, not mine.**
  367 assertions transcribed verbatim from the reference's test suites replay
  green: 158 engine vectors, 114 view-model sequences, 61 nav-category facts,
  21 history cases, 9 date-engine cases, 4 copy/paste cases.
- **Three S2 defects found and fixed**, two of them in this pass and neither
  visible to the pre-existing suite: a **High-severity CVE in a shipped
  dependency** (AUD-S2-002) and **scientific-mode calculations silently missing
  from History** (AUD-S2-003).
- **Five S3 defects found and fixed**, including layout-dependent keyboard
  bindings that broke every operator on non-US layouts (AUD-S3-004) and a
  navigation order that disagreed with the reference (AUD-S3-006).

The release is still blocked, but on **incomplete coverage rather than on
anything known to be wrong**:

1. **BLOCKER-1 (coverage):** scaling at 125/150/200 %, accessibility via
   AT-SPI/Orca, and soak/RSS growth are not yet exercised. See §6.
2. **BLOCKER-2 (decision):** AUD-DEV-001 — the Scientific keypad scrolls where
   the reference uses a shift-toggle. It is a superset functionally, but
   presentation parity is part of the contract and this needs your call.

Do not read "NOT READY" as "the calculator is wrong". Read it as "the audit has
not finished the three remaining coverage areas, and one design question is
yours to settle."

---

## 2. Counts

### By final status

| Status | Count |
|---|---|
| FIXED + RETESTED (defects) | 8 — S2-001, S2-002, S2-003, S3-002..S3-008 |
| DEVIATION (documented, user decision) | 4 |
| VERIFIED CORRECT with reference-authored evidence | 367 assertions across 6 suites |
| BLOCKED (not started) | 3 coverage areas — scaling, a11y, soak |
| Retracted false positives (my own harness bugs, recorded so they are not re-raised) | 6 |

### By method

| Method | Items |
|---|---|
| REFERENCE-TEST-REPLAY (verbatim from the unmodified reference) | 367 assertions, 6 suites |
| UI-RUN (real app, real X display, real mouse + keyboard) | 17 captures, all modes |
| UI-RUN (Avalonia.Headless + Skia, real window, real engine) | 121 tests |
| SHELL / clean-checkout build / packaging | 9 gate steps in `run_all.sh` |
| SECURITY (transitive dependency advisories) | 44 packages, 0 advisories after fix |
| LICENCE (transitive dependency licences) | 44 packages, all permissive |

Automated suite after this audit: **567 managed tests + 1 C++ ctest, green.**
`AUDIT/run_all.sh` is a 9-step gate covering all of the above.

---

## 3. Defects found, by severity

### S2 — missing / non-functional functionality, and one security defect (3 defects)

**AUD-S2-001 — Scientific keypad missing 34 of 59 reference functions;
Programmer missing 5 bitwise operations.** FIXED in commit `51ff26d`, and
**confirmed on screen** in the live pass: the full keypad is present and usable.

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

**AUD-S2-002 — CVE-2026-39959 (CVSS 7.1 High) shipped inside the `.deb`.**
`Avalonia.FreeDesktop` 11.3.6 pins `Tmds.DBus.Protocol` 0.21.2, which carries
GHSA-xrw6-gwf8-vvr9: a peer on the same D-Bus bus can impersonate a well-known
name's owner and spoof signals, exhaust file descriptors, or crash the process
with a malformed message body. There is no workaround — only an upgrade. Pinned
the patched 0.21.3; `dotnet list package --vulnerable` is now clean, and
`AUDIT/license_audit.py` fails the build if any advisory reappears.

**AUD-S2-003 — Scientific-mode calculations never appeared in History.**
They computed correctly and were stored, but never displayed. The engine keeps
two independent history lists (`m_pStdHistory` / `m_pSciHistory`), and
`HistoryViewModel._currentMode` — which selects which one is read — was assigned
in exactly one place: `ReloadHistory`, which the port **never called from
anywhere**. So every scientific calculation was written to one list and read from
the other. `StandardCalculatorViewModel.SetCalculatorType` now reloads history on
mode change, matching `MainPage.xaml.cs:281,290` upstream.

This one is a good illustration of why "the tests pass" is not the same as "it
works": 549 structural and headless tests were green while a visible feature was
dead in one of the five calculator modes. It was found by transcribing the
reference's `HistoryTests` and running them.

### S3 — behaviour / build-contract deviations (6 defects)

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
region preference. Re-verified independently in this audit, and confirmed on
screen (`Türkiye - Turkish Lira`, `Japan - Japanese Yen`).

**AUD-S3-004 — Keyboard shortcuts were bound to hardware key *positions*, not
characters.** `MainWindow.OnKeyDown` matched `Key.OemPlus`, `Key.OemMinus`,
`Key.OemPeriod` and friends. Those name a *physical key on a US layout*. On
QWERTZ the `+` character lives where US has `<`, so `+ - * / . ( )` silently did
nothing. The brief (addendum A2 item 3) and the reference both key off the typed
character; the handler contained **zero** references to `KeySymbol` or
`KeyTextInput`. Now dispatched on `e.KeySymbol` through tables transcribed from
the reference, with the `Key.*` switch kept for the numeric keypad and function
keys.

**AUD-S3-005 — Only ~12 of the reference's 71 keyboard shortcuts existed.**
The entire trig/hyperbolic letter family (`s c t u i j o l n p q r x y g m`) and
`! # @ [ ] | & ~ ^ < > \ .` were unreachable from the keyboard, even though —
after AUD-S2-001 — every one of those functions was on screen. Covered by the
same fix as S3-004; `KeyboardShortcutParityTests` drives the real window with a
deliberately non-US `Key` code so only `KeySymbol` can route the character.
Confirmed live: typing `90` then `s` now yields `sin₀( 90 ) = 1`.

**AUD-S3-006 — Graphing was last in the navigation flyout, not third.** The
reference's `CategoryManifest` orders the calculator group Standard, Scientific,
**Graphing**, Programmer, Date. The port had Graphing after all thirteen
converters. Its own test `NavCategoryGroupTests.CreateNavCategoryGroup` pins
Graphing at index 2 — independent confirmation. Note the port's pre-existing
`ModeFlyout_IsClosedByDefault_AndListsEveryMode` had silently recorded the wrong
order; that expectation was corrected to the reference sequence, not relaxed.

**AUD-S3-007 — The reference's `Ctrl+Alt+1..5` mode shortcuts were missing**, so
no mode was reachable from the keyboard at all. Added, plus plain `Alt+1..5` as
the Linux mnemonic form. Deliberately *not* put in XAML as `AccessKey`:
Avalonia 11.3 has no such property on `ListBoxItem` (build error AVLN2000), so
the behaviour lives in the key handler where it can actually work.

**AUD-S3-008 — Currency symbols rendered in the wrong places.** Both symbols sat
on a row *below* the display instead of beside their amounts; the leading one had
no padding so its glyph was clipped by the window edge, and the trailing one
inherited default left alignment inside its `*` grid column, putting it at the
window's horizontal centre. The reference puts each symbol in an `Auto` column
beside its amount with `Padding="16,0,0,0"`. **No structural test caught this** —
both symbols rendered, just in the wrong place. It took driving the real
application and looking at it.

### S1 — none found

No wrong-result, crash, hang or data-loss defect was found in anything I was
able to execute. In particular, all 367 assertions transcribed verbatim from the
reference's own test suites replay green on the first run except where a port
defect existed (AUD-S2-003).

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

### Now verified

| Area | Status |
|---|---|
| **On-screen UI-RUN (mouse, keyboard, every mode, resize)** | **DONE** — the display works after logout/login. 17 captures in `AUDIT/evidence/screens/`, driven by `AUDIT/ui_drive.sh`. Found AUD-S3-008. |
| **Keyboard shortcut parity** | **DONE** — the reference's 71 button shortcuts extracted to `reference_shortcuts.json` and transcribed into three dispatch tables. 42 tests, driving the real window with a non-US `Key` code. Found AUD-S3-004/005. (The remaining Ctrl/Shift chord variants for nav and converters are not separately bound; the reference has 28 Ctrl and 24 Shift chords, mostly the same buttons under different modifier contexts.) |
| **Locale matrix** (de_DE, ru_RU, en_IN, RTL) | **DONE** — 15 cultures for separators, en-IN 2/2/3 grouping, RTL detection, invariant-globalization fallback, ar-EG Arabic-Indic digits. No defect found. |
| **NuGet transitive licences + advisories** | **DONE** — 44 packages, all permissive; 0 advisories after the AUD-S2-002 fix. Gated in `run_all.sh`. |
| **Reference golden vectors** | **DONE for the engine** — 158 vectors replayed via `extract_golden.py`. **Partly done for the C# suites** — 114 VM sequences, 61 nav, 21 history, 9 date, 4 copy/paste. Remaining files listed below. |

### Still not verified

| Area | Why | Status |
|---|---|---|
| **Scaling 125 / 150 / 200 %** | Not exercised. `AVALONIA_GLOBAL_SCALE_FACTOR` exists but was never run at those values. | NOT VERIFIED |
| **Accessibility / AT-SPI / Orca** | Needs a live accessibility bus; Avalonia's AT-SPI bridge was not exercised with a screen reader. | NOT VERIFIED |
| **Soak / performance / RSS growth** | No long-running session was held open. | NOT VERIFIED |
| **Reference C# suites not yet replayed** | `UnitConverterViewModelTests` (31), `NarratorAnnouncementTests` (19), `SnapshotRoundTripTests` (17), `CurrencyConverterTests` (17), `MultiWindowTests` (16), `LocalizationSettingsTests` (8), `SnapshotJsonTests` (8), `UtilsTests` (6). Note the Graphing and Currency suites cannot be replayed meaningfully — both are 100% mocks upstream (§5). | EXTRACTED, NOT REPLAYED |
| **Differential fuzz (10k sequences/mode vs an oracle)** | No independent oracle buildable (the reference needs Windows); deferred. | NOT RUN |

I want to be plain about the shape of this: the reference's **534 test methods**
are extracted verbatim in `AUDIT/evidence/reference_golden_vectors.md`, and I have
turned **367 assertions** from six of those suites into executable checks against
the port. What remains is mechanical rather than hard — but it is not nothing, and
scaling/a11y/soak are genuinely unstarted.

---

## 7. What the automated on-screen pass covers, and what still needs your eyes

`AUDIT/ui_drive.sh` now drives the real application on the live display: it
launches it, finds the window, walks the flyout (locating it as its own X
toplevel rather than guessing coordinates), switches to every one of the nineteen
nav entries in the reference's order, types with `xdotool`, and resizes to
340 / 768 / 1400 px. Captures land in `AUDIT/evidence/screens/`.

**Covered automatically**, with the outcome recorded in the captures:

- Every mode renders with the reference's full keypad (Scientific and Programmer
  verified button by button).
- Navigation order matches the reference, including Graphing in third place.
- Currency loads live rates with a correct timestamp and correct English names.
- The new character shortcuts work through real X11 key events (`90` then `s` ->
  `sin( 90 ) = 1`).
- `Ctrl+Alt+1..5` switches modes.
- Nothing is clipped at 340, 768 or 1400 px.
- Currency symbols sit beside their amounts with no clipping.

**Still needs a human**, because a script cannot judge them:

1. **Every button, clicked by hand, per mode** - confirm the display and the
   enabled/disabled state of the other buttons match Windows Calculator. The
   automated check proves each button *exists and is bound*; it does not prove
   every disabled-state transition matches upstream.
2. **Dark mode toggled live** while the app is running
   (`gsettings set org.gnome.desktop.interface color-scheme prefer-dark`) - the
   theme must repaint without a restart.
3. **Screen reader** - run under `orca` and Tab through every control. The
   automation names are asserted in tests; that Orca actually announces them
   sensibly is not something a headless test can establish.
4. **Display scaling** at 125 / 150 / 200 % - never exercised.
5. **Soak** - leave it running and watch RSS.

---

## 8. Reproduce everything

```bash
cd ~/Documents/calculator
./AUDIT/run_all.sh          # 9-step gate: engine, ctest, build, suite,
                            #   isolated locale run, deb, reference parity,
                            #   licences + CVEs, live on-screen drive
python3 AUDIT/parity_check.py
python3 AUDIT/license_audit.py
python3 AUDIT/extract_golden.py      # reference engine vectors -> JSON
python3 AUDIT/extract_vm_vectors.py  # reference VM sequences   -> JSON
cd linux && dotnet test Calculator.Linux.slnx --filter "Category!=IsolatedProcess"
cd linux && dotnet test Calculator.Linux.slnx --filter "Category=IsolatedProcess"
```

`run_all.sh` exits non-zero if any check fails.

**Final full run (this audit, after all eight fixes):**

```
=== 1/9 engine build ===        OK: cmake configure / OK: cmake build
=== 2/9 engine smoke ===        100% tests passed, 0 tests failed out of 1    OK: ctest
=== 3/9 managed build ===       OK: dotnet build
=== 4/9 full test suite ===     Passed! - Failed: 0, Passed: 549              OK: dotnet test
=== 5/9 locale (own process) == Passed! - Failed: 0, Passed: 18               OK: dotnet test
=== 6/9 packaging ===          OK: build-deb.sh / OK: deb produced / OK: deb extracts
=== 7/9 data parity ===         OK: converter + resource parity
=== 8/9 licences + CVEs ===     OK: license_audit.py (44 packages, all permissive, 0 advisories)
=== 9/9 on-screen drive ===     OK: ui_drive.sh
########## AUDIT RUN: ALL CHECKS GREEN ##########
```

---

## 9. Artifacts

| Path | What |
|---|---|
| `AUDIT/DEFECTS.md` | 8 defects, full repro/root-cause/fix; 6 retracted false positives |
| `AUDIT/PROGRESS.md` | phase-by-phase state and what is still open |
| `AUDIT/evidence/screens/` | 17 captures of the real app, every mode, three widths |
| `AUDIT/evidence/reference_golden_vectors.md` | 534 reference test bodies, verbatim |
| `AUDIT/evidence/golden_engine_vectors.json` | 158 engine vectors, extracted (`extract_golden.py`) |
| `AUDIT/evidence/golden_vm_vectors.json` | 21 VM sequences / 114 assertions (`extract_vm_vectors.py`) |
| `AUDIT/evidence/reference_shortcuts.json` | 71 reference button shortcuts |
| `AUDIT/extract_golden.py` · `AUDIT/extract_vm_vectors.py` | extractors; both report what they skip rather than mis-replaying |
| `AUDIT/license_audit.py` | transitive licence + CVE gate |
| `AUDIT/parity_check.py` | converter data and resource-table byte parity |
| `AUDIT/ui_drive.sh` | live on-screen driver |
| `AUDIT/run_all.sh` | the 9-step gate |
| `AUDIT/TEST_MATRIX.csv` | ID → steps → expected → expected-source → actual → method → result |
| `AUDIT/DECISIONS.md` | 4 deviations, 1 assumption, 1 environment blocker (now resolved) |
| `AUDIT/INVENTORY.md` | reference contract |
| `AUDIT/evidence/reference_inventory_raw.md` | 3264 rows: buttons, shortcuts, limits, data |
| `AUDIT/evidence/logs/` | clean build, parity, lintian, licence audit, on-screen drive |
| `AUDIT/clean_build.sh` | clean-checkout build from documented steps only |