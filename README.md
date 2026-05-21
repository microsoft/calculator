# Windows Calculator (official repo)

Welcome — this repository contains the modern Windows Calculator (C++ and C#). It ships with Windows and provides several modes and converters in a compact, accessible UI.

[![Continuous Integration](https://github.com/microsoft/calculator/actions/workflows/action-ci.yml/badge.svg)](https://github.com/microsoft/calculator/actions/workflows/action-ci.yml)

<img src="docs/Images/CalculatorScreenshot.png" alt="Calculator Screenshot" width="450" />

Why this project is great
- Multiple modes: Standard, Scientific, Programmer, Date calculation, Unit & Currency converters.
- Precise arithmetic with arbitrary precision for many operations.
- Accessible, localizable, and test-covered UI.

Quick start
1. Clone the repository:

```bash
git clone https://github.com/Microsoft/calculator.git
cd calculator
```

2. Open the solution file `src/Calculator.sln` in Visual Studio (Windows 11 is required for UWP packaging and some SDKs).

Prerequisites (short)
- Windows 11 (build 22000+) for local development targeting the packaged app.
- Visual Studio with the "Universal Windows Platform Development" workload and the Windows 11 SDK.
- (Optional) XAML Styler Visual Studio extension for consistent XAML formatting.

Architecture & tests
- See `docs/ApplicationArchitecture.md` for a high-level overview.
- UI tests are in `src/CalculatorUITests`; they require WinAppDriver for automated UI runs.

Contributing
- We welcome contributions of all sizes. See `CONTRIBUTING.md` for the workflow, coding guidelines, and how to get started.
- File issues on GitHub and include reproduction steps and environment details.

Roadmap & features
- Graphing mode is a planned feature (the UI is in the repo, the proprietary graphing engine is not). See `docs/Roadmap.md` and issue #338 for details.

Privacy & diagnostic data
- The project may collect diagnostic data in non-development builds. See the privacy statement: https://go.microsoft.com/fwlink/?LinkId=521839. Diagnostic data is disabled by default in development builds.

Currency converter
- Developer builds use mock currency data (planets, static values) that differs from the retail Microsoft service data.

Reporting security issues
- Please follow `SECURITY.md`.

License
- Licensed under the MIT License. See `LICENSE` for full terms.

Thanks for checking out the project — if you'd like, pick a "good first issue" and open a PR! 🎉
