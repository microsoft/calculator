# Linux port

This fork adds a **native Linux port of Windows Calculator** on the
`linux-port` branch. Upstream's Windows code is untouched; everything new lives
under [`linux/`](linux/) and [`AUDIT/`](AUDIT/).

## What it is

Avalonia (.NET 10) user interface driving **Microsoft's own C++ calculation
engine, compiled unmodified** from `src/CalcManager` (including `Ratpack`)
behind a thin frozen C ABI, reached through P/Invoke.

The arithmetic is not a reimplementation. It is the reference engine, so results
are exact rather than IEEE-rounded:

| Input | Result |
|---|---|
| `0.1 + 0.2` | `0.3` |
| `1/3 * 3` | `1` |
| `√(2.25) − 1.5` | `0` |
| `170!` | `7.2574156153079989673967282111293e+306` (beyond `double` range) |
| `e` | 30 significant digits |

## Build and install

```bash
./linux/packaging/build-deb.sh 1.0.0
sudo apt install ./linux/publish/calculator-linux_1.0.0_amd64.deb
```

Requires no Wine, no Windows SDK and no Visual Studio.

## Modes

Standard · Scientific · Programmer · Date calculation · Currency · thirteen unit
converters · Graphing.

Keyboard shortcuts are bound to the **typed character**, so they work on any
layout (QWERTZ, AZERTY, non-Latin), matching upstream's 71 button shortcuts.

## Documentation

| Path | What |
|---|---|
| [`linux/README.md`](linux/README.md) | build, run, install |
| [`linux/docs/PORTING-NOTES.md`](linux/docs/PORTING-NOTES.md) | every substitution made and why, with the proprietary component each one replaced |
| [`AUDIT/REPORT.md`](AUDIT/REPORT.md) | independent audit of this port against the unmodified reference: verdict, defects, evidence |
| [`AUDIT/DEFECTS.md`](AUDIT/DEFECTS.md) | each defect with repro, root cause, fix and regression test |
| [`AUDIT/PROGRESS.md`](AUDIT/PROGRESS.md) | what is done and what is still open |

## Verification

```bash
./AUDIT/run_all.sh
```

A ten-step gate: engine build and `ctest`, managed build, the full test suite,
locale tests in their own process, `.deb` packaging, converter/resource byte
parity against the reference, a transitive licence **and CVE** audit, a live
on-screen drive through every mode, and display-scaling plus live-theme checks.
It exits non-zero on any failure, and `AUDIT_REQUIRE_DISPLAY=1` makes a missing
X display fatal rather than a quiet skip.

Current state: **549 + 18 managed tests and the native `ctest` all green**, and
**367 assertions transcribed verbatim from Microsoft's own test suites** replay
against this port — engine vectors, view-model sequences, navigation facts,
history, date arithmetic and copy/paste.

## Status

Not finished. Open items are listed in [`AUDIT/PROGRESS.md`](AUDIT/PROGRESS.md);
the substantive ones are a soak/RSS run, enumerating the AT-SPI tree with
`pyatspi` (the bus is confirmed reachable, the tree was not walked), and a
decision on the Scientific keypad, which scrolls here where upstream uses a
shift-toggle.

---

Upstream is Microsoft Corporation. This fork is not affiliated with or endorsed
by Microsoft. Upstream's `README.md`, `LICENSE`, `SECURITY.md` and code of
conduct are left exactly as received.