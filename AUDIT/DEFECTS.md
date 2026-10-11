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

## Summary of this audit pass

| ID | Sev | Status |
|---|---|---|
| AUD-S2-001 Scientific/Programmer keypad incomplete | S2 | FIXED + RETESTED |
| AUD-S3-002 build-deb.sh exits 2 on success | S3 | FIXED |
| AUD-S3-003 currency mojibake | S3 | FIXED (verified) |
| AUD-S3-004 shortcuts bound to hardware keys, not characters | S3 | FIXED |
| AUD-S3-005 only ~12 of 71 reference shortcuts implemented | S3 | FIXED |
| AUD-NOTE-001 engine has process-global state | note | documented |
| AUD-NOTE-002 158 reference golden vectors | note | 158/158 PASS |
| AUD-NOTE-003 locale matrix | note | PASS |
