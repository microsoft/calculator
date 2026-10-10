# Implementation Plan: Windows Calculator — Native Linux Port (Avalonia)

## Overview

Port Windows Calculator to native Linux on the approved stack: Avalonia 11 XAML + .NET 10 + `libCalcManager.so` (CMake/GCC) + P/Invoke, with Frankfurter FX and OxyPlot graphing. New tree lives in `linux/` in this repo (pushable as the user's own GitHub fork). First shippable review artifact is a `.deb` (per Q4). Every proprietary→OSS swap is documented in `docs/PORTING-NOTES.md`. Per Q3(b), any proprietary item with NO viable OSS equivalent stops the build for a human decision — no silent stubs.

Approved decisions (from SPEC review, 2026-10-10): Q1 OxyPlot.Avalonia MIT confirmed, default accepted. Q2 Frankfurter accepted. Q3 stop-and-ask per item. Q4 `.deb` for initial review via `dpkg-deb` over `dotnet publish` output. Q5 fork model — `linux/` tree + SPEC + PORTING-NOTES push to user's fork.

## Architecture Decisions

- **Wrap, don't fork engine:** `linux/src/CalcManager.Linux/CMakeLists.txt` compiles existing `src/CalcManager/**` sources unmodified + one new `calc_manager_c_api.{h,cpp}` with `extern "C"` ABI. Rationale: keeps Windows build intact, isolates MSVC-isms to the shim.
- **Kill `CalcManager.Interop` (C++/CX/WinRT `.idl`):** reimplement `CalculatorManagerWrapper` / `UnitConverterWrapper` as C# `LibraryImport` P/Invoke over the C ABI. No exceptions or STL types cross the boundary — error codes + UTF-16 strings only.
- **Windows.* replacement layer (`Services/`):** `LinuxNumberFormatter` (System.Globalization), `LinuxSettings` (JSON at `$XDG_CONFIG_HOME`), `LinuxClipboard` (Avalonia `IClipboard`), `LinuxLocalization` (CultureInfo decimal separator/digits), `AvaloniaDispatcher` (UIThread). Build gate: `grep -r "using Windows" linux/` must be empty.
- **Currency:** rewrite `CurrencyHttpClient` on `System.Net.Http` → Frankfurter `GET /v1/{date}..?base=USD` + symbols; keep `CurrencyDataLoader` cache/stale/week-old logic and `DefaultFromToCurrency.json` shape, backed by files not `ApplicationData`.
- **Graphing:** `GraphingInterfaces` shapes re-implemented as adapter over `OxyPlot.Avalonia` `PlotView` + `PlotModel`/`FunctionSeries`; `GraphingImpl/Mocks` discarded. Equation parsing stays in ViewModel; rendering + key-features panel are new.
- **Telemetry:** `TraceLogger` → no-op + file log opt-in. No network telemetry.
- **Packaging:** `dotnet publish -r linux-x64 --self-contained` → `linux/packaging/deb/` root (`DEBIAN/control`, `.desktop`, icon, `usr/lib/calculator-linux/`) → `dpkg-deb --build`. No Flatpak/snap in v1.

## Dependency Graph

```text
calc C-ABI + CMake (Task 1)
    ├── P/Invoke wrappers + engine smoke test (Task 2)
    │       ├── Linux Services: localization/settings/clipboard/format (Task 3)
    │       │       ├── std/sci/prog VMs + Calculator views (Task 4)
    │       │       │       ├── DateCalculator (Task 5, parallel with 6 after 3)
    │       │       │       ├── UnitConverter non-currency (Task 6)
    │       │       │       │       ├── Currency Frankfurter (Task 7)
    │       │       │       │       ├── Graphing OxyPlot (Task 8, parallel with 7 after 4)
    │       │       │       │       │       ├── Shell/nav/keyboard/settings (Task 9)
    │       │       │       │       │       │       └── tests + PORTING-NOTES + .deb (Task 10)
```

## Task List

### Phase 0 — Foundation (fail fast)

- [ ] Task 1: Scaffold `linux/` + engine C ABI + CMake `.so`
- [ ] Task 2: C# P/Invoke wrappers + real-engine smoke test

### Checkpoint: Foundation
- [ ] `libCalcManager.so` builds with GCC, no Windows-source edits
- [ ] `2+2=4` via P/Invoke passes; `grep using Windows linux/` empty in new code

### Phase 1 — Core calculation

- [ ] Task 3: Linux Services (localization, settings, clipboard, formatting, dispatcher)
- [ ] Task 4: Standard/Scientific/Programmer VMs + Avalonia Calculator views + History/Memory

### Checkpoint: Core calc
- [ ] Standard/Scientific/Programmer keyboard + button parity manual check; history/memory round-trip works

### Phase 2 — Date + Units

- [ ] Task 5: DateCalculator port + view
- [ ] Task 6: UnitConverter (non-currency) wiring + views

### Checkpoint: Converters
- [ ] Date leap-year edges pass; every non-currency category round-trips incl. whimsical/supplementary

### Phase 3 — Currency + Graphing

- [ ] Task 7: Frankfurter currency client + cache/offline/stale UI
- [ ] Task 8: OxyPlot graphing (input, render, key features)

### Checkpoint: Full function
- [ ] Live FX with timestamp + offline fallback; linear/quadratic/trig render, no mock panel

### Phase 4 — Ship

- [ ] Task 9: Shell (MainWindow nav, keyboard map, settings, themes, .desktop/icon)
- [ ] Task 10: Parity tests + PORTING-NOTES.md + `.deb` + fork push

### Checkpoint: Complete
- [ ] All SPEC success criteria met; `.deb` installs and launches on clean Ubuntu 26.04; ready for review

## Risks and Mitigations

| Risk | Impact | Mitigation |
|---|---|---|
| CalcManager has hidden MSVC-isms (`__int64`, `#pragma`, ATL) failing on GCC | High | Task 1 is fail-fast spike; shim types in C ABI layer only; stop-and-ask if engine edit required |
| Callback-heavy `ICalcDisplay` doesn't map cleanly to P/Invoke delegates lifetime | High | Keep delegate references rooted in wrapper; Task 2 proves with history/memory callbacks before UI work |
| `Windows.Globalization` fraction-digit/RTL formatting parity is subtle (currency decimals, Arabic digits) | Med | Isolate in `LinuxNumberFormatter` with unit tests vs. known vectors; English-only v1, RTL best-effort |
| Frankfurter API shape/rate limits differ from Bing schema | Med | Adapter converts Frankfurter `{base, rates}` to existing `CurrencyRatio` map; bundled snapshot fallback; offline tests with recorded JSON |
| OxyPlot Avalonia styling/perf vs. proprietary engine (zoom, trace, key features) | Med | Scope v1 to plot + key features list; document deltas in PORTING-NOTES; confirm OxyPlot MIT satisfies Q1 |
| Avalonia `NavigationView`/flyout parity with UWP XAML | Low | Custom nav + `FluentTheme`; responsive checks at 320/768/1024/1440 |
| `.deb` dependency closure (icu, libx11) on clean machine | Med | Self-contained publish; test install in clean container/VM before review |
| Fork divergence from upstream microsoft/calculator | Low | Never edit `src/`; `linux/` + docs only; note upstream commit hash in PORTING-NOTES |

## Open Questions

- None blocking. Q1–Q5 resolved (see Overview). New proprietary-no-alternative finds during build will stop per Q3(b) — asked then, not now.

## Parallelization Opportunities

- Safe to parallelize: Task 5 (Date) with Task 6 (Units) after Task 3; Task 7 (Currency) with Task 8 (Graphing) after Task 4/6.
- Must be sequential: Task 1 → 2 → 3 → 4 (ABI → wrappers → services → calc UI); Task 9 → 10 (shell before packaging).
- Needs coordination: P/Invoke ABI header is the contract — freeze `calc_manager_c_api.h` in Task 1 before Tasks 2–8 consume it.

Tasks tracked in `tasks/todo.md` (default markdown target; no external tracker designated).
