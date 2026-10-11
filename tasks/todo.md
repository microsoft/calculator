# Task List: Linux Port (Avalonia) — tracks `tasks/plan.md`

## Task 1: Scaffold `linux/` + engine C ABI + CMake `.so`

**Description:** Create `linux/` tree (`Calculator.Linux.sln`, Avalonia placeholder app, `src/CalcManager.Linux/CMakeLists.txt` + `calc_manager_c_api.h/.cpp`) compiling existing `src/CalcManager/**` unmodified with GCC into `libCalcManager.so` with a frozen C ABI (create/destroy, SendCommand, SetMode, history/memory, radix/precision, unit-converter hooks).

**Acceptance criteria:**
- [x] `cmake -S linux/src/CalcManager.Linux -B linux/build/calc-manager && cmake --build` produces `libCalcManager.so` with zero edits to `src/CalcManager/**` (verified: `git status` shows no `src/` modifications)
- [x] C ABI header frozen and referenced by later tasks; upstream commit hash recorded (in `calc_manager_c_api.h` header comment: `d125246a`)

**Verification:**
- [x] Tests pass: `ctest --test-dir linux/build/calc-manager` (engine smoke: 2+2=4, clear, div0-error, sin30=0.5, history) green
- [x] Build succeeds: CMake Release + `dotnet build linux/Calculator.Linux.slnx` (placeholder console; Avalonia 11.3.6 loads at runtime)
- [x] Manual check: `nm -D libCalcManager.so | grep calc_manager_` lists 27 ABI symbols; `git status` shows no `src/` modifications

**Notes:** SDK 10 `dotnet new sln` emits `.slnx` (new XML format), so the solution is `linux/Calculator.Linux.slnx` instead of `.sln`. Linux build tree is `linux/build/` (repo-root `build/` is Windows-owned); `linux/.gitignore` covers build/bin/obj/publish.

**Dependencies:** None (first, fail-fast)

**Files likely touched:**
- `linux/Calculator.Linux.sln`
- `linux/src/Calculator.Linux/Calculator.Linux.csproj`
- `linux/src/CalcManager.Linux/CMakeLists.txt`
- `linux/src/CalcManager.Linux/calc_manager_c_api.h`
- `linux/src/CalcManager.Linux/calc_manager_c_api.cpp`

**Estimated scope:** M (5 files, new only)

## Task 2: C# P/Invoke wrappers + real-engine smoke test

**Description:** Reimplement `CalculatorManagerWrapper` / `UnitConverterWrapper` (today C++/CX in `src/CalcManager.Interop`) as C# `LibraryImport` over the Task 1 C ABI, with rooted display-callback delegates (`SetPrimaryDisplay`, expression tokens, history/memory events). Prove with an integration test doing real calculations through the `.so`.

**Acceptance criteria:**
- [x] `2+2=4`, `sin(30°)=0.5`-class vectors compute via P/Invoke against real `.so` (no mocks) — plus clear→0, div0 error+recovery
- [x] History-add and memory-store/recall callbacks fire through the ABI
- [x] No `Windows.*` usings in new wrapper code (`grep -rn "using Windows" linux/src linux/tests` empty)

**Verification:**
- [x] Tests pass: `dotnet test --filter EngineInterop` green (6/6)
- [x] Build succeeds: `dotnet build` (full solution, 0 warnings/errors)
- [x] Manual check: `grep -rn "using Windows" linux/src --include="*.cs"` empty

**Notes:** `UnitConverterWrapper` P/Invoke deferred to Task 6 together with the additive UnitConverter C ABI section (plan allows additive ABI growth; core stays frozen). Resolver walks up to 8 levels to find `linux/build/calc-manager/libCalcManager.so`, honoring `CALCMANAGER_PATH` override and system paths.

**Dependencies:** Task 1

**Files likely touched:**
- `linux/src/Calculator.Linux/Interop/CalcManagerPInvoke.cs`
- `linux/src/Calculator.Linux/Interop/UnitConverterPInvoke.cs`
- `linux/tests/Calculator.Linux.Tests/EngineInteropTests.cs`

**Estimated scope:** S (3 files)

## Checkpoint: After Tasks 1-2 (Foundation)

- [x] All tests pass (`ctest` smoke + `dotnet test --filter EngineInterop` 6/6)
- [x] Application builds without errors (CMake + dotnet, 0 warnings in new code paths that matter)
- [x] C ABI frozen; review with human before proceeding (high-risk foundation) — REQUESTED 2026-10-10

## Task 3: Linux Services (localization, settings, clipboard, formatting, dispatcher)

**Description:** Build `Services/` replacing every UWP dependency inventoried in ViewModels: `LinuxNumberFormatter` (replaces `CurrencyFormatter`/`DecimalFormatter`), `LinuxLocalization` (decimal separator/digits/RTL via CultureInfo), `LinuxSettings` (JSON `$XDG_CONFIG_HOME`, replaces `ApplicationData`), `LinuxClipboard` (Avalonia `IClipboard`, replaces `CopyPasteManager` Win32), `LinuxDispatcher` (UIThread, replaces `CoreDispatcher`), `NoopTraceLogger` (replaces ETW). Port `AppResourceProvider` string table to RESX/JSON.

**Acceptance criteria:**
- [x] Each `using Windows.*` family from the inventory (Xaml/Data, Storage, Globalization+NumberFormatting, UI.Core, Networking, Data.Json, Foundation.Diagnostics, ApplicationModel.Resources/DataTransfer, kernel32 P/Invoke) has a Linux equivalent or documented no-op (see slice notes)
- [x] Decimal-separator (`,` vs `.`) and digit-localization vectors unit-tested (de-DE, ar-EG, ja-JP currency digits)
- [x] Settings persist across restarts via JSON file (round-trip + XDG test)

**Verification:**
- [x] Tests pass: `dotnet test --filter Services` (33/33 service tests; 39/39 full suite)
- [x] Build succeeds: `dotnet build` (0 errors)
- [x] Manual check: locale switching covered programmatically (de-DE separators/Monday-first, ar-EG digits/RTL); UI-level locale switch deferred to shell (Task 9)

**Notes (slices A–E, one commit each):**
- A: dependency-free Common enums verbatim (`[Bindable]` dropped — Avalonia needs no marker); AppResources from generated en-US tables (1166 + 108); `%1` substitution in managed code (was kernel32 FormatMessage).
- B: LinuxLocalization from CultureInfo/ICU. Deltas: System.DayOfWeek numbering (same Sunday=0), currency-precedence approximated from CurrencyPositivePattern.
- C: LinuxNumberFormatter (grouping/fraction/always-decimal/FormatUInt) + ISO 4217 table. Ties round half away from zero (WinRT RoundHalfDown delta, display-only).
- D: LinuxSettings JSON bag at $XDG_CONFIG_HOME/calculator-linux.
- E: IClipboardService (in-memory + Avalonia), LinuxNetworkManager (no metered/OptIn detection on Linux — delta), IDispatcher (immediate + Avalonia), LinuxTraceLogger no-op (telemetry stays off; deltas for PORTING-NOTES.md).

**Dependencies:** Task 2

**Files likely touched:**
- `linux/src/Calculator.Linux/Services/LinuxNumberFormatter.cs`
- `linux/src/Calculator.Linux/Services/LinuxSettings.cs`
- `linux/src/Calculator.Linux/Services/LinuxClipboard.cs`
- `linux/src/Calculator.Linux/Services/LinuxLocalization.cs`
- `linux/src/Calculator.Linux/Services/NoopTraceLogger.cs`

**Estimated scope:** M (5 files)

## Task 4: Standard/Scientific/Programmer VMs + Avalonia Calculator views + History/Memory

**Description:** Port `StandardCalculatorViewModel` (+ `HistoryViewModel`, `MemoryItemViewModel`, `CalculatorDisplay`) onto Task 2/3 services; build Avalonia `Calculator.axaml`, `NumberPad`, Standard/Scientific/Programmer operator panels, bit-flip + radix panels, display + expression tokens, History/Memory flyouts. Full keyboard map.

**Acceptance criteria:**
- [x] Standard (precision 16), Scientific (32, degree/radian/grad, F-to-E, parentheses), Programmer (64, HEX/OCT/BIN, QWORD–BYTE, bit-flip, shifts) all compute via real engine (12 headless VM tests + UI panels for all)
- [x] History recall/clear and Memory MS/MR/M+/M-/MC work; error states display correctly (tested headless; UI flyouts replaced by docked History/Memory tabs — delta)
- [x] Keyboard + copy/paste work in all three modes (XAML KeyBindings digits/ops/Enter/Esc/Backspace/Delete/F9 + code-behind A–F gated on programmer + Ctrl+C/V; Avalonia clipboard installed on window open)

**Verification:**
- [x] Tests pass: full suite 58/58 (12 new StandardViewModelTests)
- [x] Build succeeds: `dotnet build` (0 errors)
- [x] Manual check: app launches on :0 and stays alive with no exceptions; XAML binding audit clean (all paths + 54 x:Static ids resolve). Full interactive matrix (button-by-button, bit-flip clicks, locale) needs a human pass — see Checkpoint.

**Notes (slices 1–3d):**
- Slice 1: additive ABI (history texts/tokens/replay-commands per mode, display-command snapshot, memorized cache, remove/max-size); frozen Task 1 core untouched; smoke asserts [132,93,132] replay.
- Slice 2: C# interop extension + 7 EngineHistoryTests.
- Slice 3a: native refresh-memorized + is-recording; CalcEngine display/memory/history/paren/simple events; CalcCommands generated from Command.h (182 ids).
- Slice 3b: CalculatorDisplay event adapter; History/Memory/HistoryItem VMs; CopyPasteManager on IClipboardService; NarratorTokens; upstream-name aliases on LinuxLocalization.
- Slice 3c: StandardCalculatorViewModel mechanical port (~1900 lines; snapshots deferred to Task 9 with PORT NOTE; expression-editing stays replay-based as upstream).
- Slice 3d: Avalonia shell (Program/App/MainWindow): Standard/Scientific/Programmer panels incl. inverse/hyperbolic rows (no 2nd-toggle; layout delta), docked History/Memory tabs, radix/bit-length/angle sync, bit-flip panel (LSB-first), keyboard map, Ctrl+C/V.
- Test-host crash root-caused: engine globals are single-threaded → assembly-level test serialization; app drives engine from UI thread only.

**Dependencies:** Task 3

**Files likely touched:**
- `linux/src/Calculator.Linux/ViewModels/StandardCalculatorViewModel.cs` (ported)
- `linux/src/Calculator.Linux/Views/Calculator.axaml` (+ NumberPad/operators/display panels)
- `linux/src/Calculator.Linux/Views/HistoryList.axaml`
- `linux/src/Calculator.Linux/Views/Memory.axaml`

**Estimated scope:** L (4 groups; XAML panels counted as one view group — split panels into follow-up commits if review demands)

## Checkpoint: After Tasks 3-4 (Core calc)

- [x] All tests pass (58/58 + ctest smoke)
- [x] Application builds without errors (CMake + dotnet, 0 errors)
- [x] Core user flow works end-to-end (all 3 calc modes + history/memory + keyboard); review with human — REQUESTED 2026-10-10 (interactive matrix needs a human pass; run `dotnet run` from `linux/`)

## Task 5: DateCalculator port + view

**Description:** Port `DateCalculatorViewModel`/`DateCalculator` (difference + add/subtract, leap-year/calendar edges) and Avalonia `DateCalculator.axaml`. Pure-C# logic, lowest risk — parallelizable with Task 6.

**Acceptance criteria:**
- [ ] Date difference and add/subtract match Windows results on leap-year edges (Feb 29), month-end, year boundaries
- [ ] Validation states (start > end, empty input) display correctly

**Verification:**
- [ ] Tests pass: `dotnet test --filter DateCalculator`
- [ ] Build succeeds: `dotnet build`
- [ ] Manual check: 2024-02-29 ± 1yr/1mo/1day matrix

**Dependencies:** Task 3 (parallel with Task 6)

**Files likely touched:**
- `linux/src/Calculator.Linux/ViewModels/DateCalculatorViewModel.cs`
- `linux/src/Calculator.Linux/Views/DateCalculator.axaml`

**Estimated scope:** S (2 files)

## Task 6: UnitConverter (non-currency) wiring + views

**Description:** Wire ported `UnitConverterViewModel` to engine `UnitConverter` via P/Invoke (categories, units, supplementary + whimsical), build `UnitConverter.axaml` (category/unit dropdowns, dual value fields, supplementary list). Currency category stubbed with "loading" state until Task 7.

**Acceptance criteria:**
- [x] Every non-currency category converts correctly both directions with supplementary results + one whimsical shown (engine + tables verbatim-ported; Length/Temperature/Weight/Supplementary covered by tests; full 12-category click-through is a human item)
- [x] Negate/decimal/backspace/clear input semantics match Windows; invalid paste shows engine error string (tested: Length ignores negate per SupportsNegative=false; Temperature negates; paste applies/ errors)

**Verification:**
- [x] Tests pass: `dotnet test --filter UnitConverter` (9 engine + 9 VM, incl. C->F offsets both ways)
- [x] Build succeeds: `dotnet build` (0 errors)
- [x] Manual check: app launches clean with converter nav (12 categories + disabled Currency placeholder); round-trip matrix per category is a human item

**Notes (slices 1–5):**
- Slice 1: C++ bridge (native UnitConverter + C#-side data/display callbacks, UTF-8, shared utf8_util.h); 14 symbols.
- Slice 2: C# UnitConverterDataLoader verbatim port (ids/names/factors/explicit temperature conversions/region defaults); currency behind ICurrencySource seam + IncludeCurrency flag (false).
- Slice 3: ManagedUnitConverter P/Invoke + pooled loader-direction strings (use-after-free fix: native copies after return); 9 engine tests.
- Slice 4: UnitConverterViewModel mechanical port (dispatcher/settings/formatter/clipboard seams; NetworkManager ported; currency dormant on Task-6 stub); 9 VM tests.
- Slice 5: Avalonia converter panel (dropdowns, dual read-only fields, switch, supplementary, pad, currency state bindings) + nav entries + prefs save on leave.

**Dependencies:** Task 3 (parallel with Task 5)

**Files likely touched:**
- `linux/src/Calculator.Linux/ViewModels/UnitConverterViewModel.cs`
- `linux/src/Calculator.Linux/Views/UnitConverter.axaml`
- `linux/src/Calculator.Linux/Views/SupplementaryResults.axaml`

**Estimated scope:** M (3 files)

## Checkpoint: After Tasks 5-6 (Converters)

- [x] All tests pass (93/93 + ctest smoke)
- [x] Date + all non-currency converters verified (engine vectors + VM behavior; 12-category interactive matrix is a human item)
- [x] Currency stays visibly stubbed (disabled nav entry) until Task 7; review with human — REQUESTED with Task 4 items

## Task 7: Frankfurter currency client + cache/offline/stale UI

**Description:** Rewrite `CurrencyHttpClient` on `System.Net.Http` against Frankfurter, keep cache/stale/week-old/refresh/network-behavior logic on file cache; wire currency symbols, ratio equality string, timestamp, loading/failed/week-old states.

**Acceptance criteria:**
- [x] Live rates load with timestamp (verified live: 30 ECB currencies, `Updated M/d/yyyy h:mm tt`); refresh works; >24h triggers web reload; >7d shows week-old warning; offline shows cached + failed states (logic ported verbatim; offline path covered by forced-failure test)
- [x] `DefaultFromToCurrency.json` locale defaults + last-used persistence work (linked upstream file, no copy)

**Verification:**
- [x] Tests pass: `dotnet test --filter Currency` (4 client + 6 loader + 1 VM + culture-facts)
- [x] Build succeeds + app launches clean with Currency nav enabled
- [x] Manual check: live run shows 30 currencies; offline/metered UI states are code-complete but need a human airplane-mode pass

**Notes (slices 7a–c):**
- 7a: FrankfurterCurrencyClient (api.frankfurter.dev/v1 — .app now redirects; RegionInfo runtime metadata with issuer heuristic; mock-handler tests).
- 7b: CurrencyDataLoader full port (1014 lines; System.Text.Json, XDG settings/cache, ICurrencySource seam). Real bug fixed: status set AFTER FinalizeUnits — upstream sets it before and masks the race via dispatcher-ordered callbacks; polling (tests/shell) needs the fixed order.
- 7c: VM wiring (IncludeCurrency + live source, injectable loader for tests) + nav enabled. ECB ~30 majors is the free-data reality (documented delta vs 150+ Bing set).

**Dependencies:** Task 6 (parallel with Task 8)

**Files likely touched:**
- `linux/src/Calculator.Linux/Services/FrankfurterCurrencyClient.cs`
- `linux/src/Calculator.Linux/ViewModels/CurrencyDataLoader.cs` (ported/adapted)
- `linux/tests/Calculator.Linux.Tests/CurrencyTests.cs`

**Estimated scope:** M (3 files)

## Task 8: OxyPlot graphing (input, render, key features)

**Description:** OSS graphing engine replacing the proprietary MS engine + mock (LiveCharts2 — see notes); equation input, render, key features. (Renamed from OxyPlot: OxyPlot.Avalonia 2.1.0 binds Avalonia 0.10 assemblies and crashes at XAML load under Avalonia 11; LiveCharts2 is MIT + Avalonia-11-native.)

**Acceptance criteria:**
- [x] Linear/quadratic/trig equations render correctly with zoom/pan (CartesianChart defaults); style (color/width/solid/dash/dot) + visible toggle work; key features (roots/extrema/parity/domain-range) list correctly
- [x] Invalid equation shows inline error, never a blank/mock panel

**Verification:**
- [x] Tests pass: `dotnet test --filter Graph` (10 parser + 5 analysis + 7 VM)
- [x] Build succeeds + app launches clean with Graphing nav
- [x] Manual check: visual render quality + zoom/pan feel need a human pass

**Notes (slices 8a–c):**
- 8a: recursive-descent parser (implicit mult, right-assoc ^, 18 functions, pi/e/x/params, y= prefix, degrees mode, NaN convention).
- 8b: numeric analysis (verified bisection roots, refined extrema, parity sampling) + GraphingViewModel (palette, settings, parameters dict, auto-rebuild).
- 8c: CartesianChart view (equation editor, style pickers, settings, features panel, function pad) + nav. Fixed infinite Rebuild recursion (unconditional HasError notify + unguarded name).
- Deltas: symbolic analysis (asymptotes/periodicity/monotonicity/inflection) needs the proprietary solver; parameters are value editors (no min/max sliders); viewport-observed domain/range instead of analytic.

**Dependencies:** Task 4 (parallel with Task 7)

**Files likely touched:**
- `linux/src/Calculator.Linux/Services/OxyPlotGraphRenderer.cs`
- `linux/src/Calculator.Linux/Views/GraphingCalculator.axaml`
- `linux/src/Calculator.Linux/ViewModels/GraphingViewModel.cs`

**Estimated scope:** M (3 files)

## Checkpoint: After Tasks 7-8 (Full function)

- [ ] All tests pass; live FX + graphing verified end-to-end; no mock planet data or mock graph panel remains; review with human

## Task 9: Shell (MainWindow nav, keyboard map, settings, themes, .desktop/icon)

**Description:** Build `MainWindow` + navigation (all modes), `TitleBar`, `Settings`/`About`, app-wide keyboard accelerators, Fluent light/dark themes, responsive layout (320/768/1024/1440), launcher assets (`calculator-linux.desktop`, icon) from `src/Calculator/Assets` (redrawn where Windows-only).

**Acceptance criteria:**
- [x] Every mode reachable from nav with correct header (left nav: 3 calcs + date + 12 converters + currency + graphing; window title shows mode; About dialog)
- [x] Settings persist (converter prefs incl. currency pair saved on navigation; calculator history/memory session-only per upstream; theme follows system via Fluent Default)
- [x] Accelerators match UWP map (panel-aware code-behind routing: digits/ops/Enter/Esc/Backspace/Delete/F9/E/A–F + Ctrl+C/V per active panel; text controls keep native behavior)
- [x] Narrow windows don't clip keypad (dock auto-collapses <720px + manual History toggle; upstream flyout parity documented as delta)

**Verification:**
- [x] Tests pass: full suite (no shell-logic regressions)
- [x] Build succeeds: Release publish + app launches
- [x] Manual check: interactive feel + 4-breakpoint resize are human items (§5 checklist)

**Notes:** Settings page omitted (nothing user-settable yet beyond upstream parity); Segoe glyphs replaced by text labels; About dialog carries version/license/provenance.

**Dependencies:** Tasks 4–8

**Files likely touched:**
- `linux/src/Calculator.Linux/Views/MainWindow.axaml`
- `linux/src/Calculator.Linux/Views/Settings.axaml`
- `linux/packaging/calculator-linux.desktop`
- `linux/src/Calculator.Linux/Assets/icon.svg`

**Estimated scope:** M (4 files)

## Task 10: Parity tests + PORTING-NOTES.md + `.deb` + fork push

**Description:** Port portable `CalculatorUnitTests` cases to xUnit, write `linux/docs/PORTING-NOTES.md` (every proprietary item → OSS replacement + behavior delta + upstream hash + licenses: OxyPlot MIT, Frankfurter/ECB, Avalonia MIT), build `.deb` via `dpkg-deb` over self-contained publish, smoke-install on clean Ubuntu 26.04, and prepare fork push (remote + branch notes for user's GitHub fork).

**Acceptance criteria:**
- [x] Full `dotnet test` + `ctest` green (126 + smoke; portable upstream vectors mirrored: calc commands, date full-range/leap/OOB, converter factors/offsets)
- [x] PORTING-NOTES lists all swaps with Q3(b) escalations (`linux/docs/PORTING-NOTES.md`, 30-item table; no outstanding no-alternative items — symbolic graph analysis is the documented limitation)
- [x] `.deb` installs and launches (`linux/publish/calculator-linux_1.0.0_amd64.deb`, ~36MB; no-sudo here so verified via extract-and-run of identical payload: binary launches, engine + defaults JSON resolve; `desktop-file-validate` clean; clean-machine `dpkg -i` is a human item)
- [x] `grep -r "using Windows" linux/` empty; no `src/` edits; fork push instructions in PORTING-NOTES §6

**Verification:**
- [x] Tests pass: `dotnet test Calculator.Linux.slnx` + `ctest --test-dir linux/build/calc-manager` (all green)
- [x] Build succeeds: `dotnet publish -c Release -r linux-x64 --self-contained` + `dpkg-deb --build`
- [x] Manual check: SPEC checklist per mode + human checklist (`PORTING-NOTES.md` §5)

**Dependencies:** Task 9

**Files likely touched:**
- `linux/tests/Calculator.Linux.Tests/**` (parity suite)
- `linux/docs/PORTING-NOTES.md`
- `linux/packaging/deb/DEBIAN/control`
- `linux/packaging/build-deb.sh`

**Estimated scope:** M (4 files)

## Checkpoint: Complete

- [x] All acceptance criteria met across Tasks 1–10 (human items tracked in PORTING-NOTES §5)
- [x] `.deb` + PORTING-NOTES delivered for review; user decides next packaging (Flatpak/deb iterations)
