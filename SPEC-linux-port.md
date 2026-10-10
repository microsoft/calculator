# Spec: Windows Calculator — Native Linux Port (Avalonia .NET XAML)

## Objective

Port the cloned Windows Calculator repo (`/home/elshad-guliyev/Documents/calculator`) to run **natively on Linux** (Ubuntu 26.04+, x64; ARM64 best-effort) with **all functionality**:

- Standard, Scientific, Programmer calculators (incl. bit-flip panel, radix, angle types, F-to-E, precision 16/32/64)
- History + Memory (persist for session, recall, clear)
- Date calculation (difference + add/subtract)
- Unit converters (all categories: length, volume, weight, temperature, energy, area, speed, time, power, data, pressure, angle + whimsical units)
- Currency converter (live rates, not static mock)
- Graphing mode (equation input + render; repo currently ships a **mock** because the real engine is proprietary)
- Keyboard + clipboard (copy/paste), localization (decimal separator, digits), accessibility basics, settings/about

Success = a user on Linux can `dotnet run` / launch a single binary and do everything the Windows app does, without Wine, without Windows SDK, without Visual Studio.

## Capability Map (Phase 0)

One request bundles independently testable capabilities. Build order follows dependencies (engine first, shell last).

| Module id | Responsibility | Depends on |
|---|---|---|
| `calc-engine` | Portable C++ math core: Ratpack (infinite precision), CEngine, UnitConverter, CalculatorManager → `libCalcManager.so` via CMake + C ABI + C# P/Invoke | — |
| `core-converters` | Unit conversion data/logic on Linux (reuses engine, replaces `Windows.Globalization` formatting) | `calc-engine` |
| `std-sci-prog` | Standard/Scientific/Programmer ViewModels ported off UWP (`StandardCalculatorViewModel`, History, Memory) | `calc-engine` |
| `date-calc` | DateCalculator logic (mostly pure C#, low risk) | — |
| `currency` | Live FX replacement for Bing/mock data (provider + cache + offline fallback) | `core-converters` |
| `graphing` | Open-source graphing engine replacing proprietary MS engine + mock | `std-sci-prog` |
| `shell-ui` | Avalonia MainWindow, NavigationView, NumberPad, Operators panels, History/Memory flyouts, UnitConverter + Date views, Settings, dialogs | all above |

Build order: `calc-engine` → `core-converters`, `date-calc` → `std-sci-prog` → `currency`, `graphing` → `shell-ui`.

Each module gets its own task slice later (`tasks/plan.md`); this SPEC is the parent contract.

## Tech Stack

- **UI:** Avalonia UI 11.x (XAML, MVVM), Desktop profile. Why: closest mapping from UWP XAML (`x:Bind`→`{Binding}`, `VisualState`→AdaptiveTriggers/Classes, `NavigationView`→custom nav), runs natively on X11/Wayland, self-contained publish.
- **Language/Runtime:** .NET 10 SDK (already installed: 10.0.112), C# 12, `net10.0` target. CommunityToolkit.Mvvm (already used) stays.
- **Engine:** Existing `src/CalcManager` C++17, built with CMake + GCC into `libCalcManager.so`. New thin C ABI (`extern "C"`) replaces `src/CalcManager.Interop` (C++/CX + `.idl`, Windows-only, **deleted on Linux**). C# side uses `[DllImport]` / `LibraryImport` (`CalculatorManagerWrapper`, `UnitConverterWrapper` reimplemented on P/Invoke).
- **Graphing (open-source replacement):** PROPOSED `OxyPlot.Avalonia` (MIT, mature, Avalonia control). Alternatives: ScottPlot.Avalonia (Apache-2.0, faster, newer API), LiveCharts2 (MIT). Decision recorded as Open Question Q1 — default to OxyPlot unless human picks otherwise. The proprietary MS graphing engine + `src/GraphingImpl/Mocks` are **not ported**; only `src/GraphingInterfaces` shapes are re-implemented on top of the OSS engine.
- **Currency (open-source replacement):** PROPOSED Frankfurter API (`https://api.frankfurter.app/v1`, free, no key, ECB data) with `CurrencyHttpClient` rewritten on `System.Net.Http`. Fallback: bundled static snapshot (`DefaultFromToCurrency.json` shape) + 7-day stale warning preserved. Bing FX feed is **not used** (proprietary, key-less access unavailable).
- **Settings/persistence:** `System.Text.Json` file at `$XDG_CONFIG_HOME/calculator-linux/settings.json` replaces `Windows.Storage.ApplicationData.Current.LocalSettings`.
- **Clipboard:** `Avalonia.Input.Platform.IClipboard` (via `TopLevel.GetTopLevel(view).Clipboard`) replaces `CopyPasteManager` Win32 clipboard.
- **Telemetry:** `TraceLogging` / `TraceLogger` Windows ETW replaced with no-op + optional OpenTelemetry/file log (off by default). No data leaves the machine.
- **Tests:** xUnit + FluentAssertions for C# ViewModel/converter/date/currency-mapping tests; GoogleTest or CTest asserts for C++ engine smoke tests; Avalonia headless tests for key bindings where feasible.

## Commands

```bash
# Engine (C++)
cmake -S src/CalcManager -B build/calc-manager -DCMAKE_BUILD_TYPE=Release
cmake --build build/calc-manager -j
ctest --test-dir build/calc-manager

# App (.NET, from linux/ root once created)
dotnet build Calculator.Linux.sln
dotnet test Calculator.Linux.sln
dotnet run --project src/Calculator.Linux/Calculator.Linux.csproj
dotnet publish src/Calculator.Linux/Calculator.Linux.csproj -c Release -r linux-x64 --self-contained -o publish/linux-x64

# Lint/format
dotnet format --verify-no-changes
clang-format --dry-run -Werror src/CalcManager/CEngine/*.cpp  # new CMake tree only
```

Existing Windows commands (`msbuild src/Calculator.slnx`, WinAppDriver UI tests) are **not used** on Linux.

## Project Structure

New Linux tree lives under `linux/` so the Windows source stays untouched:

```text
linux/                          → Linux port root (new)
  SPEC.md                       → copy/link of this spec (this file is SPEC-linux-port.md at repo root)
  Calculator.Linux.sln          → Avalonia app + tests solution
  src/Calculator.Linux/         → Avalonia app (App.axaml, MainWindow, Views/, ViewModels-ported/, Services/)
    Views/                      → Calculator.axaml, DateCalculator.axaml, UnitConverter.axaml, Graphing.axaml, HistoryList, Memory, NumberPad, Settings
    ViewModels/                 → ported StandardCalculatorViewModel, DateCalculatorViewModel, UnitConverterViewModel, History/Memory VMs (Windows.* namespaces removed)
    Services/                   → LinuxClipboard, LinuxSettings, LinuxLocalization, CurrencyClient (Frankfurter), GraphRenderer (OxyPlot adapter)
    Assets/                     → icons, manifest, .desktop file
  src/CalcManager.Linux/        → CMakeLists.txt + C-ABI shim (calc_manager_c_api.h/.cpp) wrapping existing ../src/CalcManager sources (no fork)
  tests/Calculator.Linux.Tests/ → xUnit (parity tests vs CalculatorUnitTests cases where portable)
  docs/PORTING-NOTES.md         → every proprietary→OSS swap documented (required by user)
  packaging/                    → .desktop, flatpak manifest / deb notes (phase 2)
```

Do NOT edit `src/Calculator/` (UWP) or `src/*.vcxproj` for the port. Read-only reference only.

## Code Style

- C#: nullable enable, file-scoped namespaces, `CommunityToolkit.Mvvm` `[ObservableProperty]` (match existing ViewModels), `dotnet format` clean. No `Windows.*` usings in `linux/` — build must fail on them (grep gate in CI).
- C++ shim: C++17, `clang-format` (repo `.clang-format`), `extern "C"` functions prefixed `calc_manager_*`, no exceptions across ABI (return error codes), no SAL annotations (keep `sal_cross_platform.h` stubs).
- Avalonia XAML: `**/*.axaml` + `*.axaml.cs` code-behind, `{Binding}` (not `x:Bind`), design system tokens in `Styles/`, keyboard accelerators on all operators.
- Example — Windows API replacement pattern (the port's core mechanical edit):

```csharp
// BEFORE (Windows-only, do not copy into linux/):
using Windows.Globalization.NumberFormatting;
var f = new CurrencyFormatter("USD");

// AFTER (Linux):
using System.Globalization;
var culture = CultureInfo.CurrentCulture; // + custom fraction-digits logic in Services/LinuxNumberFormatter.cs
```

## Testing Strategy

- Framework: xUnit (`tests/Calculator.Linux.Tests`), plus `ctest` for engine.
- Locations: `linux/tests/...` mirrors `src/CalculatorUnitTests` cases that don't need UWP (engine math, expression serialization, unit ratios, date diffs).
- Coverage: all `std-sci-prog` commands (incl. error paths: divide-by-zero, domain errors), programmer radix/bit-length matrix, unit converter round-trips per category, currency cache/stale/offline paths, date leap-year edges, history/memory add/load/clear.
- Levels: C++ unit (Ratpack/CEngine) → C# ViewModel unit (mocked engine ABI) → integration (real `.so` via P/Invoke) → manual Avalonia checklist (keyboard map, clipboard, resize 320/768/1024/1440, light/dark).
- WinAppDriver / `CalculatorUITests` are Windows-only and excluded; replaced by manual checklist + headless Avalonia smoke tests.

## Boundaries

- Always: reuse `src/CalcManager` sources unmodified (wrap, don't fork); document every proprietary→OSS swap in `docs/PORTING-NOTES.md`; keep MIT license + NOTICE attribution; run `dotnet test` + engine `ctest` before each checkpoint.
- Ask first: adding a NuGet/native dependency; changing engine math semantics; changing currency provider; dropping any calculator mode or converter category; packaging (flatpak/deb/snap) decisions.
- Never: commit secrets/API keys; copy Bing FX data or proprietary graphing engine binaries; edit UWP `src/Calculator/*` to "make Linux work"; silently keep mock currency/graphing while claiming full functionality; break Windows build.

## Success Criteria

1. `dotnet run` on Ubuntu 26.04 launches Avalonia Calculator with Standard/Scientific/Programmer/Date/Unit/Currency/Graphing navigation.
2. Engine parity: 100% of portable `CalculatorUnitTests` cases for Standard/Scientific/Programmer pass against `libCalcManager.so` via P/Invoke.
3. Converters: every non-currency category converts correctly (incl. supplementary + whimsical units); currency shows live Frankfurter rates with timestamp + stale/offline states.
4. Graphing renders equations via OSS engine (at least linear/quadratic/trig) — no "mock" blank panel.
5. History/Memory, copy/paste, keyboard map, localization (`,` vs `.`), settings persistence all work; no `Windows.*` references in `linux/` (`grep -r "using Windows" linux/ ` empty).
6. `docs/PORTING-NOTES.md` lists every proprietary item, its OSS replacement (or "asked user", if none), and behavior deltas.
7. No Wine/Windows SDK required; publish produces a self-contained `linux-x64` binary.

## ASSUMPTIONS I'M MAKING

1. Avalonia 11 + .NET 10 is acceptable even though it ships a self-contained runtime (~70MB) — true "native widget" (GTK/Qt) fidelity is explicitly traded for full-functionality speed.
2. `src/CalcManager` compiles with GCC as-is (it already ships `sal_cross_platform.h` / `winerror_cross_platform.h`); any MSVC-isms found will be shimmed in the new CMake tree, not by editing Windows sources.
3. Internet available for live FX at runtime, but app must work fully offline with cached/bundled rates.
4. English-only UI for v1 (localization infra preserved via decimal-separator/digit handling, full RESW translation out of scope).
5. This Ubuntu 26.04 machine (dotnet 10, cmake 4.2, g++, GTK3/4 dev present) is the build/test host.

→ Correct me now or I proceed with these.

## Decisions (approved 2026-10-10 — SPEC review answers)

- Q1 Graphing: `OxyPlot.Avalonia` default ACCEPTED (verified MIT, cross-platform .NET — not proprietary).
- Q2 Currency: Frankfurter API ACCEPTED.
- Q3 Proprietary with no OSS alternative: (b) STOP-AND-ASK per item. No silent stubs.
- Q4 Packaging: `.deb` (dpkg-deb over self-contained publish) for initial test review; further packaging decided after review.
- Q5 Location: this work lands in `linux/` in this repo, pushed to the user's own GitHub fork.

## Open Questions (need human input before Plan)

- **Q1 — Graphing engine:** default `OxyPlot.Avalonia` (stable, simple). Prefer `ScottPlot.Avalonia` (faster, modern) or `LiveCharts2` instead? Any objection to OxyPlot?
- **Q2 — Currency provider:** default Frankfurter (ECB, free, no key). Acceptable? Or must support a different free source (ECB XML direct / exchangerate.host)?
- **Q3 — Scope of "no alternative" escalations:** if a proprietary piece has NO viable OSS equivalent (expected: nothing critical — telemetry and Store packaging are dropped by design), do you want me to (a) stub with a documented no-op, or (b) stop and ask per-item? Default: (b) stop-and-ask per your instruction.
- **Q4 — Packaging:** v1 exit = `dotnet publish` self-contained folder. Want Flatpak manifest and/or `.deb` + `.desktop` icon in v1, or defer to follow-up?
- **Q5 — Location:** new `linux/` tree alongside Windows source (proposed) vs. separate branch/fork? Default: `linux/` in this repo.

---

*Skill trace: `spec-driven-development` Phase 0 (capability map) + Phase 1 (Specify). Next gate: human approves this spec → Phase 2 `planning-and-task-breakdown` (`tasks/plan.md` + `tasks/todo.md`). No code written.*
