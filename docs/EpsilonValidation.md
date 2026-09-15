# Epsilon prototype validation

## Final result

Validated implementation commit:
`e31e7808411b7e06a1d989211a787bc1e3f1b264`.

Repository: `tilia_microsoft/calc`, branch `hackathon/epsilon_integration`.
Draft PR: <https://github.com/tilia_microsoft/calc/pull/1>.

| Check | Result |
| --- | --- |
| Unchanged baseline | Build passed; 21 native and 114 managed focused tests passed |
| Final Debug x64 build | PASS: 0 errors, 10 existing baseline warnings; 95.66 seconds |
| Final native packaged suite | PASS: 125/125, 0 failures; runner elapsed 9.352 seconds |
| Final managed packaged suite | PASS: 306/306, 0 failures; runner elapsed 13.916 seconds |
| Automated total | **431/431 passed** |
| VM scenario matrix | **56 distinct scenarios verified** across iterative runs |
| Final VM health | Healthy, responding, no crash dialog; 3/3 window checks passed |
| Crash-event query | No matching CalculatorApp crashes in the final four-hour query |
| GitHub-hosted CI | BLOCKED before job steps: hosted runners are disabled for this repository |

The final native suite includes 32 Epsilon-specific tests. The managed suite
includes 12 tests exercising the real ViewModel and WinRT wrapper, not a mock
or a window-visibility proxy. Existing Standard, Programmer, converter, date,
graphing-ViewModel, history, snapshot, lifetime, and multi-window tests also ran.

The inherited GitHub workflow's failed checks are not test failures: their
annotations state, "GitHub Actions hosted runners are disabled for this
repository." A GitHub Enterprise administrator must enable the required
runners. The workflow and required VS toolchain were not downgraded to bypass
this restriction.

## Build and package provenance

- Visual Studio Enterprise 2026, v145 tools, Windows SDK `10.0.26100.0`.
- Only Debug x64 was built and tested locally.
- Repository root: `C:\code\calc`.
- Development identity: `Microsoft.WindowsCalculator.Dev`.
- Installed version: `0.0.2.0`, x64.
- Installed package: `Microsoft.WindowsCalculator.Dev_0.0.2.0_x64__8wekyb3d8bbwe`.
- AUMID: `Microsoft.WindowsCalculator.Dev_8wekyb3d8bbwe!App`.
- Visible title: `Calculator [Dev]`.
- VM: `hyperv://Retail-0717`.
- Pinned Hyperloop: `1.260811.2`.
- Final restarted process: PID `4456`, window `0x1A058E` at evidence capture.

Seven critical deployed files were hash-compared with the fresh extracted
package: the manifest, bootstrap executable, managed application executable,
native CalcManager DLL, ViewModels DLL, resource PRI, and third-party notices.
All seven matched. The final health capture reported approximately 111 MB
process memory and a responsive window.

The source app identity remains distinct from the inbox Calculator. No inbox
uninstall, VM reimage, application-data wipe, or destructive deployment option
was used. The existing Hyperloop server was repaired with user authorization
and the same pinned binaries. An existing guest session was transferred to the
console with explicit permission when RDP disconnection prevented input.
Ownership was released after every completed validation sequence.

Test-only package copies were versioned through `1.0.8.0` to upgrade earlier
installed test packages without uninstalling them. Their compiled payloads came
from the final build; only the test-package manifest version and signature
changed. Signing used user-approved elevated execution of the existing signing
script, with temporary exported signing-key files removed afterward.

## Evidence locations

Evidence is retained locally, not committed as binary or transient artifacts.
The root below is abbreviated as `OUT`:

```text
OUT = C:\code\calc\output\epsilon-debug-x64

Build:
  OUT\Calculator-final-e31e780.log
  OUT\Calculator-final-e31e780.binlog

Final packaged tests:
  OUT\validation-packages\1.0.8.0\TestResults\
    final-e31e780-all-native.trx
    final-e31e780-all-managed.trx
    final-e31e780-all-native.console.log
    final-e31e780-all-managed.console.log
    final-e31e780-all-summary.json

Final deployment and binary hashes:
  OUT\hyperloop\deployment-final-e31e780\

Final health, package identity, release confirmation, and screenshot:
  OUT\hyperloop\final-health\
    health.json
    verify.json
    eventlog.json
    package.json
    final-calculation.json
    scientific-final.png
    release.json

Full per-case actions, assertions, timings, and evidence paths:
  OUT\hyperloop\verified-matrix.json
  OUT\hyperloop\verified-matrix.csv
```

The matrix was executed iteratively, not as one uninterrupted run. Earlier
failed and blocked attempts remain in their original directories. The
consolidated matrix identifies the actual passing run for each scenario;
it does not relabel a deployment, accepted input event, or unexecuted case as
a functional pass. Later presentation-only changes were validated with
additional native/managed regressions and final-build UI checks.

Run abbreviations used below resolve to `OUT\hyperloop\<directory>\cases.json`.
Each record contains the exact action list and individual JSON/screenshot paths.

| Run | Directory |
| --- | --- |
| M | `math-target-2` |
| C | `math-committed-completion` |
| I | `interaction-committed-1` |
| A | `interaction-final-a` |
| B | `interaction-final-b` |
| D | `interaction-final-c` |
| F | `interaction-final-formatting` |
| E | `interaction-final-fe` |

## VM arithmetic and function matrix

Nonexact references were independently generated with .NET BigInteger
fixed-point arithmetic: Newton square root, Taylor exponential/sine/cosine,
an atanh-series logarithm, and Machin's formula for pi. Working precision was
90 decimal places, with 65-place reference strings. The comparisons below use
absolute tolerance `1e-30` for the listed nonexact results of magnitude below
10; they do not use Epsilon or binary floating-point references.

All rows below are **PASS**. `sign` means the existing negate button; unary
results were checked before pressing equals unless indicated.

| Case | Input/actions | Expected | Actual | Run |
| --- | --- | --- | --- | --- |
| h-precedence-buttons | Buttons `2+3*4=` | 14 | 14 | M |
| h-precedence-keys | Keyboard `2+3*4=` | 14 | 14 | C |
| h-parentheses | `(2+3)*4=` | 20 | 20 | M |
| h-divide-assoc | `8/4/2=` | 1 | 1 | M |
| h-subtract-assoc | `8-3-2=` | 3 | 3 | M |
| h-decimal | `0.1+0.2=` | 0.3 | 0.3 | M |
| h-negative | `(2 sign)*(3 sign)=` | 6 | 6 | M |
| h-scientific-notation | `1 EXP sign 3 =` | 0.001 | 0.001 | M |
| h-nested | `((2+3)*4)=` | 20 | 20 | M |
| h-incomplete | `(2+3 =`; clear; `4+ =` | 5; 8 | 5; 8 | M |
| h-square | `9`, square | 81 | 81 | M |
| h-square-negative | `3`, sign, square | 9 | 9 | M |
| h-reciprocal | `8`, reciprocal | 0.125 | 0.125 | M |
| h-sqrt-zero | `0`, sqrt | 0, no hang | 0 | M |
| h-sqrt-four | `4`, sqrt | 2 | 2 | M |
| h-sqrt-fraction | `2.25`, sqrt | 1.5 | 1.5 | M |
| h-sqrt-two | `2`, sqrt | Independent sqrt(2) | `1.4142135623730950488016887242097` | M |
| h-closed-group-unary | `(2+3)`, square, `+1=` | 25 immediately; 26 | 25; 26 | M |
| h-pi | pi button | Independent pi | `3.1415926535897932384626433832795` | M |
| h-euler | e button | Independent e | `2.7182818284590452353602874713527` | M |
| h-sin-deg | DEG; `30`, sin | 0.5 | 0.5 | M |
| h-sin-rad | RAD; `pi/2=`, sin | Approximately 1 | 1 | M |
| h-sin-grad | GRAD; `100`, sin | Approximately 1 | 1 | M |
| h-cos | RAD; `1`, cos | Independent cos(1) | `0.54030230586813971740093660744298` | M |
| h-tan | RAD; `1`, tan | Independent tan(1) | `1.5574077246549022305069748074584` | M |
| h-ln-one | `1`, ln | 0 | 0 | M |
| h-ln-ten | `10`, ln | Independent ln(10) | `2.3025850929940456840179914546844` | M |
| h-log-ten | `100`, log | 2, base 10 | 2 | M |
| h-exp-zero | `0`, second function, e^x | 1 | 1 | C |
| h-exp-two | `2`, second function, e^x | Independent exp(2) | `7.389056098930650227230427460575` | C |
| h-backspace | `123`, backspace twice | 1 | 1 | M |
| h-clear-entry | `12+34`, CE, `5=` | 17 | 17 | M |
| h-clear-all | `((12+3`, C, `2+3=` | 5 | 5 | M |
| h-continue-result | `2+3=`, `*4=` | 20 | 20 | M |
| h-repeat-equals | `2+3===` | 5, no repeated operation | 5 | M |
| h-retained-value | `1/3=`, `*3=` | 1 from retained value | 1 | M |
| h-negative-sqrt | `2`, sign, sqrt | Invalid input | Invalid input | M |
| h-zero-log | `0`, log | Invalid input | Invalid input | M |
| h-negative-ln | `1`, sign, ln | Invalid input | Invalid input | M |
| h-div-zero | `1/0=`, C, `2+3=` | Divide-by-zero error; recovery | Error; 5 | C |

## VM interaction and isolation matrix

All rows below are **PASS**.

| Case | Input/actions | Expected | Actual | Run |
| --- | --- | --- | --- | --- |
| i-unsupported-controls | Inspect primary/secondary, function, inverse/hyperbolic controls | Deferred controls disabled; e^x accessible | Disabled states and e^x availability verified | I |
| i-keyboard-unsupported | Type `12`, unsupported `^`, then `+1=` | Preserve 12, then 13 | 12 unchanged; 13 | I |
| i-history-read-only | Create `2+3=`, enter 1, click history row, inspect clear | Display history; no replay/clear | Result 5 visible; primary remains 1; clear disabled | A |
| i-standard-memory | Standard: store 42; Scientific: inspect disabled memory; return/recall | Preserve 42 | 42 recalled; all Scientific memory controls disabled | I |
| i-standard-order | Standard `2+3*4=` | Legacy immediate result 20 | 20 | I |
| i-programmer | HEX `A+1=`; DEC `6 AND 3=` | B; 2 | B; 2 | B |
| i-navigation-smoke | Length conversion; Date; Graphing; Scientific | Existing other-mode surfaces work | 2.54 cm = 1 inch; same-date result; graphing input list; Scientific keypad | A |
| i-repeated-switches | Standard/Programmer/Scientific three times; `2+3=` | Modes/capabilities restored | 5 | I |
| i-limit-exponent | `1 EXP 257 =`, clear, `2+3=` | Bounded overflow and recovery | Overflow; 5 | B |
| i-limit-depth | 33 opening parentheses, clear, `2+3=` | Bounded overflow and recovery | Overflow; 5 | D |
| i-limit-input | 257 digits, clear, `2+3=` | Bounded input rejection/error and recovery | Overflow; 5 | D |
| i-malformed | Empty `()`, clear, `2+3=` | Explicit invalid-input error and recovery | Invalid input; 5 | D |
| i-fe-grouping | Type 1234567; toggle F-E on/off with state assertions | Grouped live input; scientific/fixed output | `1,234,567`; `1.234567e+6`; `1,234,567` | E |
| i-long-grouping | `1234567890123=` | Repeating grouping | `1,234,567,890,123` | F |
| i-narrow-wide | Resize narrow/wide; calculate | Usable controls and correct results | 5 narrow; 7 wide; screenshots retained | F |
| i-restart | Close Dev window; relaunch exact AUMID; `2+3=` | Real engine initializes after restart | 5; screenshot retained | F |

Native locale tests additionally cover decimal comma and repeating Indian
grouping (`3;2;0`), including continued calculations from retained values.
The VM used its existing English locale; no claim is made that every Windows
display language was manually exercised.

## Failures investigated and resolved

- Incorrect finite-precision zero certification, double rounding, and terminal
  grouping repetition were corrected with regression coverage.
- Exponent editing no longer destroys retained values and preserves explicit
  exponent signs and the existing F-E notation.
- Prefix precedence, first-equals completion, closed-group unary display, and
  exact trigonometric zeros/poles were corrected.
- CE now exposes C while preserving the pending prefix; constants and live
  locale grouping retain their actual numeric state.
- Native constructor callbacks and raw mode commands no longer expose the
  wrong engine to managed Programmer queries.
- Scientific snapshots restore display-only history, reset live input to zero,
  and preserve legacy memory instead of presenting a stale formatted result.
- Zero-length WinRT arrays are handled without snapshot exceptions.
- Scientific multi-window tests now assert intentionally disabled capabilities
  and display-only history rather than legacy replay or memory behavior.
- Same-version loose deployment was detected as stale by hash verification;
  the Dev version upgrade and stable deployment directory resolved it.
- Earlier VM attempts were blocked by disconnected/headless desktops or by
  incorrect UIA patterns/hidden raw-only targets. Input readiness, actual
  before/after assertions, supported patterns, and accessible row targets were
  used for the verified reruns.
- One unchanged currency test failed in an intermediate full run, then passed
  alone, in its complete 22-test class, and in the final full managed run.
  No unrelated currency implementation was changed.

## Remaining limitations

This remains the bounded prototype described in
[EpsilonIntegration.md](EpsilonIntegration.md) and the
[adapter documentation](../src/CalcManager/EpsilonEngine/README.md).
Unsupported Scientific features are deliberately disabled, not emulated by
RatPack. Unresolved real-number zero/sign decisions report a resource error
rather than using an arbitrary tolerance as equality. Graphing smoke coverage
uses the repository's existing development/mock engine, not the proprietary
retail graphing implementation. Hosted CI requires repository administration
before it can execute.
