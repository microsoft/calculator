# AUDIT DEFECTS

Severity key (per brief): S1 wrong result / crash / hang / data loss ·
S2 missing or non-functional feature/control · S3 behaviour, UI, text or
shortcut deviation · S4 cosmetic.

Reference: microsoft/calculator @ d125246a4e19842ce1332e6c7839cf0e110027d8
(cloned to /home/elshad-guliyev/ref-calculator, untouched).

---

## AUD-S2-001 — Scientific keypad missing 34 of 59 functions; Programmer missing 5

**Severity:** S2 (missing / non-functional controls)
**Status:** FIXED + RETESTED (commit 51ff26d)

### Repro
1. Launch the app, open the hamburger, choose **Scientific**.
2. Look at the keypad.
3. Absent: `sec csc cot`, `sin⁻¹ cos⁻¹ tan⁻¹ sec⁻¹ csc⁻¹`, `sinh cosh tanh
   sech csch`, `sinh⁻¹ cosh⁻¹ tanh⁻¹ sech⁻¹ csch⁻¹`, `cot⁻¹ coth⁻¹ coth`,
   `x³`, `|x|`, `∛x`, `ʸ√x`, `2ˣ`, `log_b`, `⌊x⌋`, `⌈x⌉`, `rand`, `→dms`,
   `→deg`, `CE`.
4. Choose **Programmer**. Absent: `NAND`, `NOR`, `L⇥ (Lsh)`, rotate-through-carry
   `⟺⟻` / `⟼⟽`.

### Expected (and its source)
Every `ButtonId` declared in the reference's own XAML must be reachable:
- `src/Calculator/Views/CalculatorScientificOperators.xaml` — 59 distinct `ButtonId`s
- `src/Calculator/Views/CalculatorProgrammerRadixOperators.xaml` — 31 distinct `ButtonId`s

Extracted mechanically; see `AUDIT/evidence/reference_golden_vectors.md` §2 and
the extraction in TEST-MATRIX.csv rows ENG-UI-001..003.

### Actual (before fix)
Scientific: 38 of the 59 reference ButtonIds wired (32 missing at the time of
detection; `CE` and the last two were closed by the same fix).
Programmer: 36 of 31 wired, 5 missing (`Lsh`, `Nand`, `Nor`, `RolC`, `RorC`).

### Root cause
Pure presentation gap. The ViewModel `OnButtonPressed` forwards the
`NumbersAndOperatorsEnum` value straight to the native engine, and the engine
(Ratpack rational) already implements every one of them. Proved by driving the
unwired enum values straight through the ViewModel:

| function | result | independent check |
|---|---|---|
| `sec(30°)` | `1.1547005383792515290182975610039` | 1/cos30 ✓ |
| `csc(30°)` | `2` | 1/sin30 ✓ |
| `cot(45°)` | `1` | cot45 ✓ |
| `asin(0.5)` | `30` | 30° ✓ |
| `sinh(1)` | `1.1752011936438014568823818505956` | e−e⁻¹ /2 ✓ |
| `asinh(1)` | `0.88137358701954302523260932497979` | ✓ |
| `cosh(1)` | `1.5430806348152437784779056207571` | (e+e⁻¹)/2 ✓ |
| `floor(2.7)` / `ceil(2.1)` | `2` / `3` | ✓ |
| `cube(3)` | `27` | ✓ |
| `2^x` | `4` | ✓ |
| `rand` | `0.15049534…` | in [0,1) ✓ |
| `Euler` | `2.7182818284590452353602874713527` | 30+ significant digits — arbitrary precision confirmed |

A ViewModel-only test suite cannot see this class of defect, which is why the
port shipped it. This is exactly the gap the brief's Avalonia addendum A2
(visual-tree completeness diff) is meant to close.

### Fix
`linux/src/Calculator.Linux/MainWindow.axaml` only — no engine, no ViewModel,
no behavioural change to anything that already worked:
- Scientific keypad extended from 8 rows × 5 cols to **15 rows × 5 cols**,
  containing all 59 reference ButtonIds, inside a `ScrollViewer`.
- Programmer keypad extended 6 rows → 7 rows with the 5 missing bitwise ops.

### Regression test
`linux/tests/Calculator.Linux.Tests/Audit/KeypadCompletenessTests.cs` —
walks the **real** window's visual tree per mode, extracts every bound
`CommandParameter` enum, and fails if any reference `ButtonId` is unreachable.
Fails on the pre-fix tree, passes post-fix. Suite: 220 green.

### Deviation introduced (documented)
Scientific is now scrollable. The reference instead swaps column 0 via a shift
button (VisualStates `ShowStandardFunctions` etc.) to keep 8×5. The port keeps
all functions permanently visible and scrolls. Recorded in DECISIONS.md as
AUD-DEV-001 — functionally equivalent superset, different presentation.

---

## AUD-S3-002 — `build-deb.sh` exits non-zero on success

**Severity:** S3 (misleading build result; breaks CI chains and any
`./build-deb.sh && deploy` script)
**Status:** FIXED (this audit)

### Repro
```
./linux/packaging/build-deb.sh 1.0.0 ; echo "exit=$?"
# exit=2
ls -la linux/publish/*.deb          # .../calculator-linux_1.0.0_amd64.deb  (35 MB, valid)
dpkg-deb -x linux/publish/calculator-linux_1.0.0_amd64.deb /tmp/x  # succeeds, 232 files
```

### Expected
Exit 0 when the package was produced successfully.

### Actual
Exit 2, plus a spurious `dpkg-deb: error: tar subprocess was killed by signal
(Broken pipe)` on every single successful run.

### Root cause
The script's last statement is
`dpkg-deb --contents "$DEB" | head -n 12`.
`head` exits after 12 lines and closes the pipe; `dpkg-deb`'s child `tar`
takes SIGPIPE; the pipeline's status (the last command's = dpkg-deb) becomes
the script's exit status. Purely cosmetic output was being treated as a build
failure.

### Fix
Appended `|| true` (with a comment) to that informational pipeline. Verified
`exit=0`.

---

## AUD-S3-003 — Currency dropdowns show mojibake (Cherokee / native-script names)

**Severity:** S3 (wrong user-visible text)
**Status:** FIXED in commit f1d4e93 (found by this audit's predecessor pass;
re-verified independently here — see TEST-MATRIX row ENG-CUR-004)

### Repro
Clear cache, open **Currency**, read either dropdown.

### Expected
`United States - US Dollar`, `Spain - Euro`, `United Kingdom - British Pound`.

### Actual
`ᏌᏊ ᎢᏳᎾᎵᏍᏔᏅ - US Dollar` (Cherokee), `España - Euro` (native script).

### Root cause
Country name came from `RegionInfo.DisplayName`, which follows
`CultureInfo.CurrentUICulture`, and the region for USD was picked from
`CultureInfo.GetCultures(...)`, whose first `US` entry is the Cherokee culture
`chr-US`.

### Fix
`RegionInfo.EnglishName` + deterministic region preference (neutral culture,
then `en-XX`, then any match).

### Note
Static currency data is cached for 7 days; an existing
`~/.cache/calculator-linux` keeps the old strings until it expires or is deleted.

---

## False positives retracted during this audit

Recorded because ground rule 1 requires evidence for *every* checked item,
including items that looked like defects and were not.

- **Resource-string mismatches (38).** A naive merge of the reference's two
  resource tables suggested 38 wrong values (`CubeRoot` → `cuberoot`,
  `CannotUseIInReal` → `\"i\"…`, ordinals `2`→`2nd` overwritten by `2`→`CE`).
  Re-checked: `Resources.resw` (1166 entries) and `CEngineStrings.resw`
  (108 entries) are **two separate tables** and the port reproduces both
  exactly — 0 missing, 0 value mismatches, 0 duplicate keys within either
  table. The "mismatches" were cross-table key overlap, which the reference
  also has. **Not a defect.**
- **"Scientific is missing everything" (first extraction).** My first
  section-slicer found `IsVisible="{Binding IsProgrammer}"` earlier in the file
  than the scientific panel (it is also used for the radix readout), so the
  slice was inverted and reported `scientific=0`. Re-extracted correctly.
- **Grouping separators (`1,000,000,000,000`).** Matches the reference:
  `CCalcEngine::GroupDigitsPerRadix` groups radix 10 with `m_decGrouping`
  (`src/CalcManager/CEngine/scidisp.cpp:246`).
- **`ln(-1)` / `sqrt(-1)` "not erroring".** Probe bug: my keystroke helper
  silently dropped the `-`, so the engine received `ln(1)`/`sqrt(1)`. Both
  returned the correct value for what was actually sent.
---

## AUD-NOTE-003 — Locale matrix: PASSED (no defect found)

Checked headlessly across 15 cultures (en-US/GB, de-DE, ru-RU, fr-FR, es-ES,
it-IT, pt-BR, pl-PL, sv-SE, tr-TR, ja-JP, zh-CN, ar-EG, he-IL) by
`LocaleMatrixTests`:

- Decimal separator and group separator match .NET/ICU for every culture.
- Grouping is culture-correct, including **en-IN's 2,2,3 pattern** versus
  Western 3,3,3.
- RTL detection (`IsRightToLeft`) correct for ar-EG, he-IL, fa-IR.
- Invariant-globalization fallback (`CultureInfo.InvariantCulture`, i.e. running
  without ICU data) still computes correctly rather than crashing.
- ar-EG renders **Arabic-Indic digits** (١٬٢٣٤٫٥), which is what Windows
  Calculator shows; the test normalises the script rather than forcing Latin.

No S1/S2/S3 found here. Two false alarms were raised and retracted first: my
expected strings were wrong (I typed `1.2345` and expected `1,234.5`, and I
guessed the wrong space character for fr/ru grouping — in both cases the port
was already correct).

---

## AUD-S3-006 — Graphing was last in the navigation flyout, not third

**Severity:** S3 (presentation deviation from the reference)
**Status:** FIXED (this audit)

### Repro
Open the hamburger flyout and read the list: the port showed

    Standard, Scientific, Programmer, Date calculation, Currency, Volume, Length,
    Weight and Mass, Temperature, Energy, Area, Speed, Time, Power, Data,
    Pressure, Angle, Graphing, About

### Expected
The reference's `CategoryManifest`
(`src/Calculator.ViewModels/Common/NavCategory.cs`) declares the calculator group
in this order:

    Standard (Number1), Scientific (Number2), **Graphing (Number3)**,
    Programmer (Number4), Date (Number5)

followed by the converter group: Currency, Volume, Length, Weight and Mass,
Temperature, Energy, Area, Speed, Time, Power, Data, Pressure, Angle.

Graphing is **third**, between Scientific and Programmer.

### Actual
Graphing appeared **last**, after all thirteen converters. The port had been
built against the older Windows 10 ordering where Graphing trailed the list.

### Root cause
The flyout list was authored by hand from memory rather than transcribed from
the reference's manifest.

### Fix
`linux/src/Calculator.Linux/MainWindow.axaml` now lists Graphing third, in the
reference's order.

### Regression test
`ModeNavigationParityTests.FlyoutOrder_MatchesReferenceCategoryManifest` asserts
the exact 19-entry sequence, and `Graphing_IsThird_NotLast` pins the position
explicitly. The pre-existing `WindowLayoutTests.ModeFlyout_IsClosedByDefault_
AndListsEveryMode` had silently recorded the wrong order; it was corrected to the
reference's sequence rather than relaxed.

---

## AUD-S3-007 — The reference's Ctrl+Alt+1..5 mode shortcuts were missing

**Severity:** S3 (missing shortcut behaviour)
**Status:** FIXED (this audit)

### Repro
Focus the window, press Ctrl+Alt+1 through Ctrl+Alt+5. Nothing happened; the only
way to change mode was to open the flyout and click.

### Expected
`CategoryManifest` binds `VirtualKey = MyVirtualKey.Number1..5` with
`AccessKey = "1".."5"` to Standard, Scientific, Graphing, Programmer and Date.

### Actual
No mode was reachable from the keyboard.

### Fix
`MainWindow.SwitchModeByShortcut(Key)` handles Ctrl+Alt+1..5 (how Windows
resolves those access keys) and also plain Alt+1..5 (the Linux/desktop mnemonic
convention; the app has no Alt menu so it cannot collide). Digits 6-9 are not
mode shortcuts.

`AccessKey` was deliberately **not** added to the XAML: Avalonia 11.3 has no
such property on `ListBoxItem` (AVLN2000 at build time), so the behaviour lives
in the key handler where it can actually work.

### Regression test
`ModeNavigationParityTests` - 5 Ctrl+Alt+digit cases, 5 Alt+digit cases, and 3
negative cases proving digits 6/7/9 are not bound.

---

## AUD-NOTE-004 — Reference C# test replay (new in this pass)

Three more reference test bodies replayed verbatim against the port:

| Reference source | Port test | Result |
|---|---|---|
| `CalculatorUnitTests/CalculatorManagerTest.cpp` (158 vectors) | `ReferenceGoldenVectorTests` | 158/158 |
| `Calculator.Tests/DateCalculatorTests.cs` - `DateCalculatorUnitTests` | `ReferenceDateEngineTests` | 9/9 |
| `Calculator.Tests/CopyPasteManagerTests.cs` (4 bodies) | `ReferenceCopyPasteTests` | 4/4 |
| `Calculator.Tests/StandardCalculatorViewModelTests.cs` (21 sequences / 114 assertions) | `ReferenceVmGoldenVectorTests` | 22/22 |

The VM sequences are extracted by `AUDIT/extract_vm_vectors.py`, which also
reports the 21 methods it deliberately skips because they need bespoke view-model
setup (mode switches, radix changes, memory preloading) - reported, not silently
mis-replayed. `{DEC}` stands in for the reference's `_decimalSeparator`
concatenation and is resolved from the active localization at replay time.

All passed with no code changes: the port's C# layer was already correct.

---

## AUD-NOTE-005 — On-screen verification unblocked

After a logout/login the X11 display maps windows normally and the real app was
driven with `xdotool` through every mode (`AUDIT/ui_drive.sh`,
`AUDIT/evidence/screens/`). Confirmed on screen: the Programmer keypad is complete
(HEX/DEC/OCT/BIN, QWORD/DWORD/WORD/BYTE/Bit flip, A-F, all 13 operator and
memory buttons), the Standard keypad matches upstream exactly, and the flyout
lists all nineteen entries. This is what surfaced AUD-S3-006 and AUD-S3-007.

---

## AUD-S2-002 — Known high-severity CVE in a shipped dependency

**Severity:** S2 (security)
**Status:** FIXED (this audit)

### Repro
```
dotnet list linux/Calculator.Linux.slnx package --vulnerable --include-transitive
```
Before the fix:

```
> Tmds.DBus.Protocol   0.21.2   High   https://github.com/advisories/GHSA-xrw6-gwf8-vvr9
```

### Expected
No package with a known advisory in the transitive graph that is redistributed
in the `.deb`.

### Actual
`Avalonia.FreeDesktop` 11.3.6 pins `Tmds.DBus.Protocol` 0.21.2, which carries
**CVE-2026-39959** (CVSS 7.1 High, CVSS:3.1/AV:L/AC:L/PR:L/UI:N/S:U/C:N/I:H/A:H):
a peer on the same D-Bus bus can impersonate the owner of a well-known name and
spoof signals, exhaust file descriptors, or crash the process with a malformed
message body. There are no known workarounds; the fix is an upgrade.

### Fix
A direct `PackageReference` to `Tmds.DBus.Protocol` **0.21.3** (the backported
patch) in `linux/src/Calculator.Linux/Calculator.Linux.csproj`, which overrides
Avalonia's transitive pin. `dotnet list ... --vulnerable` now reports no
vulnerable packages for either project.

### Regression gate
`AUDIT/license_audit.py` now also runs `dotnet list package --vulnerable
--include-transitive` and exits non-zero on any advisory, so this cannot
silently regress when a dependency is bumped. Wired into `run_all.sh`.

---

## AUD-S2-003 — Scientific-mode calculations never appeared in History

**Severity:** S2 (missing user-visible functionality)
**Status:** FIXED (this audit)

### Repro
1. Switch to Scientific mode.
2. Compute `1 + 5 + 3 =`. The display shows 9 - correct.
3. Open the History pane. **It is empty.**

Do the same in Standard mode and the entry appears.

### Expected
`src/Calculator/Views/MainPage.xaml.cs` calls
`CalculatorViewModel.HistoryVM.ReloadHistory(newValue)` whenever the calculator
mode changes (line 281 for Standard, line 290 for Scientific). The reference's
own `TestHistoryItemClicked` switches to `ModeScientific`, computes, and then
indexes the history list - so history in scientific mode is expected to work.

### Actual
`HistoryViewModel._currentMode` ("0 = standard, 1 = scientific") is assigned in
exactly one place: the first line of `ReloadHistory`. `ReloadHistory` was
**never called from anywhere in the port** - `grep -rn ReloadHistory src/` found
only its own definition.

The engine keeps two independent history lists (`m_pStdHistory` and
`m_pSciHistory` in `CalculatorManager.cpp:28-29`, selected by `m_pHistory` at
lines 172 and 190). `OnHistoryItemAdded` reads whichever list `_currentMode`
names, so with `_currentMode` pinned at 0 every scientific-mode calculation was
computed, stored in the scientific list, and then looked up in the standard list
- which is empty.

### Root cause
A ported method was wired up in the view model but never invoked from the mode
change path.

### Fix
`StandardCalculatorViewModel.SetCalculatorType` now calls
`_historyVM.ReloadHistory(targetState)` for Standard and Scientific. It is in
the view model rather than the window so the reload happens wherever the mode
actually changes. Programmer is deliberately excluded, matching the reference,
whose Programmer branch sets `AreHistoryShortcutsEnabled = false` instead of
reloading.

### Regression test
`ReferenceHistoryTests` (21 tests) replays the reference's HistoryTests, all 13
`TestHistoryStandardOrderOfOperations_*` cases verbatim, the three-space history
expression format (`1   +   8 =`), the 20-item cap with eviction, clear
behaviour, and the scientific-mode case that failed. It was this suite that
found the defect.

---

## Summary of this audit pass

| ID | Sev | Status |
|---|---|---|
| AUD-S2-001 Scientific/Programmer keypad incomplete | S2 | FIXED + verified on screen |
| AUD-S2-002 CVE-2026-39959 in a shipped dependency | S2 | FIXED + gated |
| AUD-S2-003 Scientific-mode history never recorded | S2 | FIXED |
| AUD-S3-002 build-deb.sh exits 2 on success | S3 | FIXED |
| AUD-S3-003 currency mojibake | S3 | FIXED + verified on screen |
| AUD-S3-004 shortcuts bound to hardware keys, not characters | S3 | FIXED |
| AUD-S3-005 only ~12 of 71 reference shortcuts implemented | S3 | FIXED |
| AUD-S3-006 Graphing last in nav, reference puts it third | S3 | FIXED |
| AUD-S3-007 Ctrl+Alt+1..5 mode shortcuts missing | S3 | FIXED |
| AUD-S3-008 currency symbols in the wrong places | S3 | FIXED + verified on screen |
| AUD-DEV-001 Scientific scrolls instead of shift-toggle | accepted deviation | user decision pending |
| AUD-DEV-002 Frankfurter instead of the dead fwlink URL | accepted deviation | — |
| AUD-DEV-003 English-only | accepted deviation | — |
| AUD-DEV-004 History/Memory docked rather than floating | accepted deviation | — |

### Notes (verified, not defects)

| ID | Subject |
|---|---|
| AUD-NOTE-001 | Native engine has process-global state (single-window app, not user-visible) |
| AUD-NOTE-002 | 158 reference engine golden vectors replay green |
| AUD-NOTE-003 | Locale matrix across 15 cultures, no defect |
| AUD-NOTE-004 | Reference C# test replay: date, copy/paste, VM sequences all green first time |
| AUD-NOTE-005 | On-screen verification unblocked after logout/login |
| AUD-NOTE-006 | Order-dependent locale tests: root-caused and moved to an isolated process |
| AUD-NOTE-007 | NuGet transitive licence audit: 44 packages, all permissive |
| AUD-NOTE-008 | Live on-screen pass over every mode |

Two more of my own test bugs were retracted rather than charged to the port:
history is newest-first so "Newest" had to index 0, not the end; and the
reference's own `TestHistoryItemAddMaxItems` expects the oldest surviving entry
after eviction, which is not the entry just computed.

### Retracted false positives (recorded so they are not re-raised)

| Subject | Why retracted |
|---|---|
| 38 "wrong" resource strings | two resw tables had been merged during extraction |
| `scientific=0` extraction result | a stray earlier `IsProgrammer` binding polluted the scrape |
| grouping separators | correct per the engine's `GroupDigitsPerRadix` |
| `ln(-1)` probe | the helper dropped the `-` before evaluating |
| golden-vector failures (81, then 34) | harness bugs: `\x00D7` decoded as `chr(0)`, and `CommandNULL` was being sent |
| locale matrix failures | wrong expected strings in the test, port was correct |
| currency symbol layout, first fix attempt | wrong assertions in the test, port was correct |

### Test totals

- Managed: **549** in the main run + **18** in the isolated locale process = **567**
- Native: `ctest` green
- Reference-authored assertions replayed: 158 engine + 114 VM + 9 date +
  4 copy/paste + 61 nav + 21 history

---

## AUD-NOTE-009 — Scaling, live dark mode and the AT-SPI bus: verified

`AUDIT/ui_deep_check.sh` closes the three areas that were previously recorded as
"not started".

**Display scaling — PASS.** Launched at `AVALONIA_GLOBAL_SCALE_FACTOR` 1.0,
1.25, 1.5 and 2.0. Captures in `AUDIT/evidence/screens/scaling/scale-*.png`.
At 200 % the whole UI scales proportionally — text, keypad, memory row, display —
with nothing clipped, overlapping or truncated, and all 24 standard buttons
remain visible and legible. Avalonia applies the factor; the port's layouts use
`*` sizing throughout, so nothing was pinned to absolute pixels.

**Live dark-mode toggle — PASS.** Started under `prefer-dark`, then flipped
`org.gnome.desktop.interface color-scheme` to `prefer-light` **without
restarting the app**. It repainted in place: white surfaces, dark glyphs, a
darker accent for `=`, and the operator column in a distinct grey. This
confirms the theme follows the desktop setting at runtime rather than only at
startup. Captures: `theme-dark-then-light.png` and its inverse.

**Accessibility — PARTIAL.** The session AT-SPI bus **is reachable**
(`gdbus introspect --session --dest org.a11y.Bus` succeeds), which is the
precondition for any screen reader to see the application at all. The accessible
tree itself was **not enumerated**, because `pyatspi` is not installed on this
machine and installing it is outside the port's scope. Automation IDs and names
are asserted in the test suite (`ConverterSymbolLayoutTests` and the
screen-reader name tests), but whether Orca announces them *sensibly* is still a
human judgement — see REPORT.md section 7.

No defects found in any of the three.

---

## AUD-NOTE-010 — Two of my own test/infra bugs found on the final pass

**Non-hermetic test.** `ConverterSymbolLayoutTests` located the two currency
symbol TextBlocks by their *text* (`₺`, `¥`). Those characters come from the
live Frankfurter feed, so the test passed when the network answered and failed
when it did not — a coin-flip that showed up as a failure in the final gate run.
Rewritten to locate the blocks **structurally** (Row 2, Columns 0 and 2 of the
three-column display grid), which is what the assertion is actually about. No
network dependency remains.

**Silently skipped gate step.** `run_all.sh` step 9 printed
`SKIP: no X display / xdotool` and still reported `ALL CHECKS GREEN`, because
`DISPLAY` was unset in the calling shell. A gate that skips a check quietly is
worse than one that fails. It now defaults `DISPLAY` to `:0` when
`/tmp/.X11-unix/X0` exists, and `AUDIT_REQUIRE_DISPLAY=1` makes the skip fatal.
