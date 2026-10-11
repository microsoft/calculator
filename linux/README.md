# Calculator for Linux

Native Linux port of [Windows Calculator](https://github.com/microsoft/calculator)
(MIT). The portable C++ engine is reused **unmodified**; the UI is rebuilt on
Avalonia; currency is live via Frankfurter/ECB; graphing renders locally via
LiveCharts.

- **Spec:** [`../SPEC-linux-port.md`](../SPEC-linux-port.md)
- **Plan & tasks:** [`../tasks/plan.md`](../tasks/plan.md), [`../tasks/todo.md`](../tasks/todo.md)
- **Every proprietary swap documented:** [`docs/PORTING-NOTES.md`](docs/PORTING-NOTES.md)
  (start here for the what/why of each replacement)

## Build & run

```bash
# Engine (needs CMake 3.20+, GCC with C++17)
cmake -S linux/src/CalcManager.Linux -B linux/build/calc-manager -DCMAKE_BUILD_TYPE=Release
cmake --build linux/build/calc-manager -j
ctest --test-dir linux/build/calc-manager

# App (needs .NET 10 SDK; NuGet for restore)
dotnet build linux/Calculator.Linux.slnx
dotnet test linux/Calculator.Linux.slnx        # 199 tests, serialized (engine is single-threaded); headless UI layout + pixel checks
dotnet run --project linux/src/Calculator.Linux/Calculator.Linux.csproj
```

No Wine, Windows SDK, or Visual Studio required.

## Package

```bash
./linux/packaging/build-deb.sh [version]   # needs dpkg-deb; ~36MB self-contained .deb
sudo dpkg -i linux/publish/calculator-linux_*_amd64.deb
/usr/lib/calculator-linux/Calculator.Linux
```

## Layout

- `src/Calculator.Linux/` — Avalonia app (`Program`, `App`, `MainWindow`,
  `AboutWindow`; `Common/`, `Services/`, `DataLoaders/`, `Interop/`, `ViewModels/`,
  `Styles/` — theme brushes + control styles ported from upstream `App.xaml`)
- `src/CalcManager.Linux/` — CMake + shims + frozen UTF-8 C ABI + converter bridge
- `tests/Calculator.Linux.Tests/` — xUnit parity suite
- `docs/PORTING-NOTES.md` — the full porting record
- `packaging/` — `.desktop`, icon, `build-deb.sh`
- `build/` — CMake output (git-ignored)
