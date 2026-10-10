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
- [ ] `2+2=4`, `sin(30°)=0.5`-class vectors compute via P/Invoke against real `.so` (no mocks)
- [ ] History-add and memory-store/recall callbacks fire through the ABI
- [ ] No `Windows.*` usings in new wrapper code

**Verification:**
- [ ] Tests pass: `dotnet test --filter EngineInterop` green
- [ ] Build succeeds: `dotnet build`
- [ ] Manual check: `grep -rn "using Windows" linux/src --include="*.cs"` empty

**Dependencies:** Task 1

**Files likely touched:**
- `linux/src/Calculator.Linux/Interop/CalcManagerPInvoke.cs`
- `linux/src/Calculator.Linux/Interop/UnitConverterPInvoke.cs`
- `linux/tests/Calculator.Linux.Tests/EngineInteropTests.cs`

**Estimated scope:** S (3 files)

## Checkpoint: After Tasks 1-2 (Foundation)

- [ ] All tests pass (`ctest` + `dotnet test --filter EngineInterop`)
- [ ] Application builds without errors (CMake + dotnet)
- [ ] C ABI frozen; review with human before proceeding (high-risk foundation)

## Task 3: Linux Services (localization, settings, clipboard, formatting, dispatcher)

**Description:** Build `Services/` replacing every UWP dependency inventoried in ViewModels: `LinuxNumberFormatter` (replaces `CurrencyFormatter`/`DecimalFormatter`), `LinuxLocalization` (decimal separator/digits/RTL via CultureInfo), `LinuxSettings` (JSON `$XDG_CONFIG_HOME`, replaces `ApplicationData`), `LinuxClipboard` (Avalonia `IClipboard`, replaces `CopyPasteManager` Win32), `LinuxDispatcher` (UIThread, replaces `CoreDispatcher`), `NoopTraceLogger` (replaces ETW). Port `AppResourceProvider` string table to RESX/JSON.

**Acceptance criteria:**
- [ ] Each `using Windows.*` family from the inventory (Xaml, Storage, Globalization, UI.Core, Devices, Data.Json, Networking) has a Linux equivalent or documented no-op
- [ ] Decimal-separator (`,` vs `.`) and digit-localization vectors unit-tested
- [ ] Settings persist across restarts via JSON file

**Verification:**
- [ ] Tests pass: `dotnet test --filter Services`
- [ ] Build succeeds: `dotnet build`
- [ ] Manual check: switch locale `de-DE`, separator renders `,`; restart keeps settings

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
- [ ] Standard (precision 16), Scientific (32, degree/radian/grad, F-to-E, parentheses), Programmer (64, HEX/OCT/BIN, QWORD–BYTE, bit-flip, shifts) all compute via real engine
- [ ] History recall/clear and Memory MS/MR/M+/M-/MC work; error states (divide-by-zero, domain) display correctly
- [ ] Keyboard + copy/paste work in all three modes

**Verification:**
- [ ] Tests pass: `dotnet test --filter StandardCalculator`
- [ ] Build succeeds: `dotnet build`
- [ ] Manual check: scripted button/keyboard matrix (incl. `1/0`, `√-1`, programmer base switches, bit-flip) matches Windows behavior

**Dependencies:** Task 3

**Files likely touched:**
- `linux/src/Calculator.Linux/ViewModels/StandardCalculatorViewModel.cs` (ported)
- `linux/src/Calculator.Linux/Views/Calculator.axaml` (+ NumberPad/operators/display panels)
- `linux/src/Calculator.Linux/Views/HistoryList.axaml`
- `linux/src/Calculator.Linux/Views/Memory.axaml`

**Estimated scope:** L (4 groups; XAML panels counted as one view group — split panels into follow-up commits if review demands)

## Checkpoint: After Tasks 3-4 (Core calc)

- [ ] All tests pass
- [ ] Application builds without errors
- [ ] Core user flow works end-to-end (all 3 calc modes + history/memory + keyboard); review with human

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
- [ ] Every non-currency category converts correctly both directions with supplementary results + one whimsical shown
- [ ] Negate/decimal/backspace/clear input semantics match Windows; invalid paste shows engine error string (not blank)

**Verification:**
- [ ] Tests pass: `dotnet test --filter UnitConverter`
- [ ] Build succeeds: `dotnet build`
- [ ] Manual check: length/volume/temperature/energy/area/speed/time/power/data/pressure/angle round-trip matrix

**Dependencies:** Task 3 (parallel with Task 5)

**Files likely touched:**
- `linux/src/Calculator.Linux/ViewModels/UnitConverterViewModel.cs`
- `linux/src/Calculator.Linux/Views/UnitConverter.axaml`
- `linux/src/Calculator.Linux/Views/SupplementaryResults.axaml`

**Estimated scope:** M (3 files)

## Checkpoint: After Tasks 5-6 (Converters)

- [ ] All tests pass
- [ ] Date + all non-currency converters verified; review with human before currency/graphing

## Task 7: Frankfurter currency client + cache/offline/stale UI

**Description:** Rewrite `CurrencyHttpClient` on `System.Net.Http` against Frankfurter (`/v1/latest?base=USD`, symbols/metadata mapping to existing `CurrencyStaticData`/`CurrencyRatio` shapes); keep `CurrencyDataLoader` cache/stale/week-old/refresh/network-behavior logic on file cache; wire currency symbols, ratio equality string, timestamp, loading/failed/week-old states into the Task 6 view.

**Acceptance criteria:**
- [ ] Live rates load with timestamp; refresh works; >24h triggers web reload; >7d shows week-old warning; offline/metered shows cached + failed states (no crash, no mock planet data)
- [ ] `DefaultFromToCurrency.json` locale defaults + last-used persistence work

**Verification:**
- [ ] Tests pass: `dotnet test --filter Currency` (recorded Frankfurter JSON + forced-failure + stale-clock cases)
- [ ] Build succeeds: `dotnet build`
- [ ] Manual check: online load → airplane-mode reload → refresh, ratio string + symbols correct for USD→EUR and JPY (0-decimal) + JOD (3-decimal)

**Dependencies:** Task 6 (parallel with Task 8)

**Files likely touched:**
- `linux/src/Calculator.Linux/Services/FrankfurterCurrencyClient.cs`
- `linux/src/Calculator.Linux/ViewModels/CurrencyDataLoader.cs` (ported/adapted)
- `linux/tests/Calculator.Linux.Tests/CurrencyTests.cs`

**Estimated scope:** M (3 files)

## Task 8: OxyPlot graphing (input, render, key features)

**Description:** Reimplement `GraphingInterfaces` as adapter over `OxyPlot.Avalonia` (`PlotView` + `PlotModel`/`FunctionSeries`); port equation input area, style panel, settings, key-features panel; discard `GraphingImpl/Mocks`. Equations evaluate locally (no proprietary engine).

**Acceptance criteria:**
- [ ] Linear/quadratic/trig equations render correctly with zoom/pan; style (color/width) + visible toggle work; key features (intercepts/extrema) list correctly
- [ ] Invalid equation shows inline error, never a blank/mock panel

**Verification:**
- [ ] Tests pass: `dotnet test --filter Graphing` (equation sampling vectors)
- [ ] Build succeeds: `dotnet build`
- [ ] Manual check: `y=x`, `y=x^2`, `y=sin(x)` render + trace; settings apply

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
- [ ] Every mode reachable from nav with correct header; settings persist; accelerators match UWP map; window resizes without clipped controls at all four breakpoints

**Verification:**
- [ ] Tests pass: `dotnet test` (full suite, no regressions)
- [ ] Build succeeds: Release `dotnet publish -r linux-x64 --self-contained`
- [ ] Manual check: Tab keyboard walk, theme switch, 4-breakpoint resize, .desktop launch

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
- [ ] Full `dotnet test` + `ctest` green; PORTING-NOTES lists all swaps with Q3(b) escalations (or "none outstanding")
- [ ] `.deb` installs (`dpkg -i`) and launches on clean Ubuntu 26.04 with all SPEC success criteria met
- [ ] `grep -r "using Windows" linux/` empty; no `src/` edits; fork push instructions ready

**Verification:**
- [ ] Tests pass: `dotnet test Calculator.Linux.sln` + `ctest --test-dir build/calc-manager` (all green)
- [ ] Build succeeds: `dotnet publish -c Release -r linux-x64 --self-contained` + `dpkg-deb --build`
- [ ] Manual check: clean-machine install → launch → full SPEC checklist (calc modes, history/memory, date, units, live currency, graphing, clipboard, persistence)

**Dependencies:** Task 9

**Files likely touched:**
- `linux/tests/Calculator.Linux.Tests/**` (parity suite)
- `linux/docs/PORTING-NOTES.md`
- `linux/packaging/deb/DEBIAN/control`
- `linux/packaging/build-deb.sh`

**Estimated scope:** M (4 files)

## Checkpoint: Complete

- [ ] All acceptance criteria met across Tasks 1–10
- [ ] `.deb` + PORTING-NOTES delivered for review; ready for code-review-and-quality pass; user decides next packaging (Flatpak/deb iterations)
