# PORTING-NOTES — Windows Calculator → Native Linux Port

Upstream: `microsoft/calculator` @ `d125246a4e19842ce1332e6c7839cf0e110027d8`
Port branch: `linux-port` (intended for the maintainer's own GitHub fork).
Spec: `SPEC-linux-port.md` at repo root. Plan + tasks: `tasks/plan.md`, `tasks/todo.md`.

> Rule enforced throughout: upstream sources under `src/` are **read-only**.
> Every Linux difference lives in `linux/` (app, shims, ABI) plus generated
> tables. `git status` must never show modifications under `src/`.

## 1. Architecture

```
src/CalcManager/** (UNMODIFIED, compiled by GCC 15 via CMake)
        ↓ wraps, never forks
linux/src/CalcManager.Linux/
  shims/            ppltasks.h (eager task<T>), intsafe.h, winerror.h,
                    linux_prelude.h (portable PCH replacement)
  calc_manager_c_api.{h,cpp}   frozen UTF-8 C ABI for the calculator core
  unit_converter_bridge.{h,cpp} C++ UnitConverter + C#-side data/display
  engine_strings_en.inc         generated from en-US CEngineStrings.resw
  CMakeLists.txt                builds libCalcManager.so + ctest smoke
        ↓ P/Invoke (UTF-8, GCHandle-routed callbacks)
linux/src/Calculator.Linux/ (net10.0 + Avalonia 11 + LiveCharts2, MIT)
  Interop/      CalcManagerNative.cs, UnitConverterNative.cs
  Common/       verbatim-ported enums, CopyPasteManager, CalculatorDisplay, ...
  Services/     Linux* replacements (ICU/clipboard/settings/format/...)
  DataLoaders/  unit database port, Frankfurter FX, file cache
  ViewModels/   Standard/Date/Unit/Graphing ports (+ history/memory)
  MainWindow / App / AboutWindow (Avalonia XAML)
```

String rule: **UTF-8 across every native boundary**. `wchar_t` is 4 bytes on
Linux but 2 bytes in C#, so `CharSet.Unicode` marshalling would corrupt —
all ABI strings are UTF-8 (`IntPtr` + `PtrToStringUTF8` / `StringToHGlobalAnsi`).

Threading rule: the engine keeps process-global state
(`CCalcEngine::InitialOneTimeOnlySetup`) and is **single-threaded by design**
(upstream drives it from the UI thread only). The app touches it exclusively
from the Avalonia UI thread; the test assembly is serialized
(`AssemblyInfo.cs`: `DisableTestParallelization`) because concurrent engine
instances crash the process (observed as a test-host crash, Task 4).

## 2. Proprietary / Windows-only inventory → replacements

| # | Upstream item | Replacement | Behavior deltas |
|---|---|---|---|
| 1 | `CalcManager.Interop` (C++/CX, WinRT `.idl`) | `calc_manager_c_api.h` + C# P/Invoke (`Interop/`) | Same engine, plain C ABI. `wchar_t`→UTF-8 (see above). History-item marshalling is pull-based (count/item/tokens/flat replay commands) instead of COM arrays. |
| 2 | UWP XAML views (`src/Calculator/Views`) | Avalonia AXAML (`MainWindow`, panels per mode) | Layout re-authored, not transliterated. History/Memory are a **docked tab panel**, not flyouts; narrow windows (<720px) auto-collapse it (toggle to reopen). Scientific inverse/hyperbolic functions are always-visible rows (no 2nd-toggle). Bit-flip panel is LSB-first. |
| 3 | `Windows.Globalization.*Formatting` (`DecimalFormatter`, `CurrencyFormatter`, `DateTimeFormatter`) | `LinuxNumberFormatter` (ICU/`CultureInfo` backed) + ISO 4217 minor-units table | Grouping/fraction/decimal-always semantics preserved. **Ties round half-away-from-zero** (WinRT currency path used RoundHalfDown; display-only delta). Currency codes come from the ISO table (unknown → 2 digits). Long dates use the culture `D` pattern (region-subtag nuances dropped). |
| 4 | `LocalizationSettings` (WinRT formatter + `kernel32 GetLocaleInfoEx`) | `LinuxLocalization` (`CultureInfo`/ICU) | Separators, grouping (`NumberGroupSizes` → `"3;0"` shape), native digits, RTL, week start, calendar all match. `DayOfWeek` is `System.DayOfWeek` (same Sunday=0 numbering). Currency-symbol precedence approximated from `CurrencyPositivePattern`. Calendar identifiers use WinRT well-known strings (`"gregorian"`, …). Upstream helper method names kept as aliases. |
| 5 | `kernel32 FormatMessage` (`LocalizationStringUtil`) | Managed `%1..%9` substitution | Identical observable output for the patterns used in `Resources.resw`. |
| 6 | `ResourceLoader` + `.resw` | Generated dictionaries (`GeneratedResources.cs`: 1166 UI + 108 engine strings, en-US) | **English-only v1.** Same fallback semantics (UI key → key itself; engine key → empty). |
| 7 | `ApplicationData.LocalSettings` | `LinuxSettings` (JSON at `$XDG_CONFIG_HOME/calculator-linux/`) | Typed via `System.Text.Json` (string/long/bool/double/`DateTimeOffset`). Write-through; corrupt file → starts empty, never crashes. |
| 8 | `ApplicationData.LocalCacheFolder` + `StorageFile`/`FileIO` | `LinuxFileCache` (`$XDG_CACHE_HOME/calculator-linux/`) | Same read/write-text contract; missing file → null (graceful, no throw). |
| 9 | WinRT `Clipboard`/`DataPackage` | `IClipboardService` (in-memory default; Avalonia `IClipboard` installed on window open) | Pure validation logic (`CopyPasteManager`, 645 lines) ported verbatim. |
| 10 | `CoreDispatcher`/`CoreWindow` | `IDispatcher` (`ImmediateDispatcher` default; `AvaloniaDispatcher` installed by shell) | Engine callbacks are synchronous on the caller (UI) thread, so immediate dispatch is correct in-app; the Avalonia dispatcher covers future background callbacks (FX refresh). |
| 11 | `TraceLogging`/ETW (`TraceLogger`) | `LinuxTraceLogger` no-op, same call sites | **No telemetry leaves the machine.** All `UpdateButtonUsage`/`Log*` calls compile and do nothing. Revisit only with explicit opt-in. |
| 12 | `NetworkInformation` (incl. metered/cost) | `LinuxNetworkManager` (`NetworkInterface` + `NetworkChange`) | **No metered/`OptIn` detection on Linux** (no .NET API): Normal when any NIC is up, else Offline. |
| 13 | Bing currency feed + mock planet data | **Frankfurter/ECB** (`api.frankfurter.dev/v1`; the old `.app` host now redirects) + `RegionInfo` runtime metadata | Live rates for ~30 ECB majors (vs 150+ Bing set — free-data reality). Base USD added as 1.0. Metadata (country/symbol/name) derived from .NET regions with issuer heuristic (`USD`→`US`, current-region, else first). `DefaultFromToCurrency.json` linked from upstream (not copied). |
| 14 | Proprietary graphing engine + `GraphingImpl/Mocks` | Local parser + numeric analysis + **LiveCharts2** (MIT, Avalonia-11-native) | Equations render (linear/quadratic/trig verified). Key features are numeric (roots/extrema/parity/observed range). **No symbolic analysis**: asymptotes, periodicity, monotonicity, inflection points omitted. Parameters are value editors (no min/max sliders). `OxyPlot.Avalonia` was tried first and rejected: 2.1.0 binds Avalonia 0.10 assemblies → `FileNotFoundException` at XAML load under Avalonia 11. |
| 15 | `ppltasks.h` / `intsafe.h` / `winerror.h` (MSVC-only, pulled via `pch.h`) | `shims/` (eager `task<T>` + `.then`, empty `intsafe`, HRESULT macros) + `linux_prelude.h` (`-include`d STL baseline) | Engine compiles unmodified with GCC 15. Currency async path in C++ is synchronous-by-construction (web loading is driven C#-side). |
| 16 | `GraphControl` (UWP control) | `CartesianChart` + `GraphingViewModel` | Zoom/pan via chart defaults; grid toggle; radians/degrees; style (color/width/solid/dash/dot); per-equation visibility + inline errors. |
| 17 | WinAppDriver UI tests | xUnit headless parity (171 tests) + manual matrix | UI tests are Windows-only by nature; logic is proven headless, pixels need human eyes (checklist §5). |
| 18 | Store packaging (`Package.appxmanifest`) | `.deb` (`linux/packaging/build-deb.sh` → self-contained `linux-x64` + side-by-side `.so`) | No Store, no MSIX. `.desktop` + SVG icon included. No hard `Depends` (self-contained). |
| 19 | Segoe MDL2 glyphs / `LanguageFontGroup` | Text labels + Inter font | Icon font has no Linux equivalent; nav uses text. |
| 20 | Narrator live regions (`NarratorNotifier`) | Data-only announcements (`NarratorAnnouncement` preserved) | Screen-reader live-region wiring is Task-9 UI work; announcement strings/codes intact. |
| 21 | Multi-window snapshots (`Snapshots.cs`, `ICalcDisplayTarget` snapshot APIs) | Deferred (PORT NOTE in code) | History recall works via flat command replay (equivalent observable behavior single-window). |
| 22 | `GetLocaleInfoEx`/`ResolveLocaleName` P/Invoke | Deleted (ICU covers it) | See #4. |
| 23 | `DateTimeFormatter` calendar/clock args | Culture patterns | `longdate` → `D` pattern; 24h clock flag irrelevant (no time shown). |
| 24 | `GeographicRegion` (unit/currency defaults) | `RegionInfo.CurrentRegion` (2-letter code, `US` fallback) | Region-dependent unit defaults (US customary, Fahrenheit, pyeong, kW-vs-W…) preserved via the ported loader. |
| 25 | `UserProfile.GlobalizationPreferences` | `RegionInfo` currency symbol (fallback `USD`) | User-override list has no Linux equivalent. |
| 26 | WinRT `Calendar` (date engine) | `LinuxCalendar` (`System.Globalization`) | 1601 floor preserved (throws `ArgumentException` like WinRT → same OOB paths). Both identifier spellings accepted. Persian→Hijri mapping preserved. `FirstMonthInThisYear` = 1. |
| 27 | `Windows.Data.Json` | `System.Text.Json` | Same skip-on-bad-item semantics. |
| 28 | `CalculatorMode`/`CalculatorCommand` (WinRT enums) | `CalcCommands.cs` generated from `Command.h` (182 ids) | Numeric identity with the engine guaranteed by generation. |
| 29 | `[Bindable]` attributes | Dropped | Avalonia needs no equivalent marker. |
| 30 | `BitLength`/`NumberBase`/nav enums | Verbatim ports | Values double as engine ids; do not renumber. |

Additionally fixed during the port (behavior-preserving robustness):
- Completion status is set **after** `FinalizeUnits`, not before (upstream sets it
  before and masks the race with dispatcher-ordered callbacks; `LoadFinished()`
  polling would otherwise observe empty data — found via a long debug hunt).
- Loader-direction P/Invoke strings are pooled (native copies **after** the
  callback returns; freeing inside the callback is use-after-free).
- Engine locale separators (decimal/grouping) are passed at engine creation
  from the UI locale (German decimal comma verified); upstream resolves them
  per-call via delegate.
- `SpinWait`-polled async completion in tests only (production uses callbacks).

## 3. Build / run / package

```bash
# Engine
cmake -S linux/src/CalcManager.Linux -B linux/build/calc-manager -DCMAKE_BUILD_TYPE=Release
cmake --build linux/build/calc-manager -j
ctest --test-dir linux/build/calc-manager

# App
dotnet build linux/Calculator.Linux.slnx
dotnet test linux/Calculator.Linux.slnx
dotnet run --project linux/src/Calculator.Linux/Calculator.Linux.csproj

# .deb (self-contained linux-x64 + engine, ~36MB)
./linux/packaging/build-deb.sh [version]

# Install (needs root) and launch
sudo dpkg -i linux/publish/calculator-linux_1.0.0_amd64.deb
/usr/lib/calculator-linux/Calculator.Linux
```

Requirements: .NET 10 SDK, CMake 3.20+, GCC with C++17, `dpkg-deb`
for packaging. NuGet access for restore (Avalonia 11.3.6, LiveCharts2 2.0.0,
xUnit, CommunityToolkit.Mvvm). No Wine, Windows SDK, or Visual Studio.

## 4. Tests

- `ctest`: engine smoke through the frozen C ABI (2+2=4, clear, div0-error,
  sin30=0.5, history texts, `[132,93,132]` command replay, memory).
- `dotnet test` (171, serialized — engine is single-threaded): engine
  interop/history, all services (localization incl. de-DE/ar-EG, formatting,
  settings, infra), Standard VM (3 modes, radix, memory, history recall,
  errors, paste, domain errors, F-to-E, shifts, angles, de-DE display),
  date engine + VM (upstream full-range/leap/OOB vectors + DST),
  converter engine + VM (12-category matrix, offsets, supplementary, prefs,
  negate gating), currency (recorded feed, cache/stale/week-old/failed,
  JPY/JOD digits, VM end-to-end), graphing (parser, analysis, VM),
  paste validation per mode.
- Live checks performed: Frankfurter fetch (30 currencies + timestamp),
  app launch per milestone (no exceptions), installed-layout launch.

## 5. Human verification checklist (needs eyes)

Covered headlessly already (no human pass needed): 12-category round-trips +
negate gating, JPY/JOD fraction digits, week-old/stale/failed currency states,
DST date math, German decimal comma, domain errors, F-to-E, shifts, angle
modes, paste validation per mode, graphing parser/analysis/VM.

- [ ] Interactive feel: button-by-button in all 3 calc modes, bit-flip clicks,
      radix/bit-length switches (logic proven; this is about feel).
- [ ] Airplane-mode currency on a real machine (code paths tested forced).
- [ ] Graphing visuals: `y=x`, `y=x^2`, `y=sin(x)` render, zoom/pan feel.
- [ ] Resize 320/768/1024/1440 (dock auto-collapses <720px; confirm usability).
- [ ] RTL UI mirroring + full non-English session sanity.
- [ ] Clean-machine `dpkg -i` install + launch (couldn't `sudo` here; verified
      via extract-and-run of the same payload instead).

## 6. Fork push

```bash
git remote add fork <your-github-fork-url>   # e.g. git@github.com:<you>/calculator.git
git push -u fork linux-port
```

`origin` still points at `microsoft/calculator` (read-only reference);
all port work is on branch `linux-port` + untracked-never `TEMP FOLDER SKILLS/`
(reference material, intentionally uncommitted).
