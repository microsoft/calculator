# AUDIT PROGRESS

PROJECT_ROOT: /home/elshad-guliyev/Documents/calculator   MODE: AUDIT_AND_FIX
REFERENCE: microsoft/calculator @ d125246a4e19842ce1332e6c7839cf0e110027d8

## Done

- [x] Phase 0 RECON — clean-checkout build via documented steps only
- [x] Phase 1 INVENTORY — reference side complete (3264 rows, 200 buttons, 220 shortcuts)
- [x] Phase 2 ENGINE — **158/158 of the reference's own golden vectors replay green**
- [x] Phase 3 UI — headless-Skia real-window rendering, visual-tree completeness diff,
      **and a live on-screen pass over every mode with real mouse/keyboard input**
      (`AUDIT/ui_drive.sh`, `AUDIT/evidence/screens/`)
- [x] Phase 4 CROSS-CUTTING — keyboard shortcut parity (42), locale matrix (22),
      mode navigation (15+61), converter layout (3)
- [x] Phase 5 PACKAGING — deb + lintian + extract + exit code verified;
      NuGet transitive licence + CVE audit green and gated;
      display scaling (100/125/150/200 %) and live dark-mode toggle verified
- [x] Phase 6 FIX + REGRESS — 8 defects fixed, suite green

## Reference test replay (Microsoft-authored expectations)

| Reference source | Port test | Result |
|---|---|---|
| `CalculatorUnitTests/CalculatorManagerTest.cpp` (158 vectors) | `ReferenceGoldenVectorTests` | 158/158 |
| `Calculator.Tests/StandardCalculatorViewModelTests.cs` (21 sequences / 114 assertions) | `ReferenceVmGoldenVectorTests` | 22/22 |
| `Calculator.Tests/DateCalculatorTests.cs` | `ReferenceDateEngineTests` | 9/9 |
| `Calculator.Tests/CopyPasteManagerTests.cs` | `ReferenceCopyPasteTests` | 4/4 |
| `Calculator.Tests/NavCategoryTests.cs` | `ReferenceNavCategoryTests` | 61/61 |
| `Calculator.Tests/HistoryTests.cs` | `ReferenceHistoryTests` | 21/21 |

Extractors: `AUDIT/extract_golden.py`, `AUDIT/extract_vm_vectors.py`.
Both report what they deliberately skip rather than mis-replaying it.

## Defects fixed

- **S2-001** scientific/programmer keypad incomplete
- **S2-002** CVE-2026-39959 (High) in Tmds.DBus.Protocol, shipped in the .deb
- **S2-003** scientific-mode calculations never recorded in History
- **S3-002** build-deb.sh exit code · **S3-003** currency mojibake
- **S3-004** layout-dependent keys · **S3-005** missing shortcuts
- **S3-006** nav order · **S3-007** mode shortcuts · **S3-008** currency symbols

S2-002 and S2-003 were both found by replaying reference tests and by the
vulnerability gate added alongside the licence audit — neither was visible to
the structural tests.

## Known constraints (documented, not defects)

- The engine holds process-global state: one window per process, and the
  separators baked in by the first engine built. AUD-NOTE-001, AUD-NOTE-006.
- Graphing and Currency are 100% mocks upstream, so neither is numerically
  comparable to the reference. Graphing renders nothing there; the port plots
  for real, which is a superset.

## Still open

1. User decision: keep the scrollable Scientific keypad (AUD-DEV-001, a superset
   of the reference's functions) or rebuild it with the reference's shift-toggle
   for exact presentation parity.
2. **Soak / RSS growth** — the only genuinely unstarted item left.
3. **AT-SPI tree enumeration** — the session bus is confirmed reachable, but
   `pyatspi` is absent so the tree was not walked; Orca announcement quality is
   a human judgement.
4. Remaining reference test files not yet replayed:
   `UnitConverterViewModelTests.cs` (31), `NarratorAnnouncementTests.cs` (19),
   `SnapshotRoundTripTests.cs` (17), `CurrencyConverterTests.cs` (17),
   `MultiWindowTests.cs` (16), `LocalizationSettingsTests.cs` (8),
   `SnapshotJsonTests.cs` (8), `UtilsTests.cs` (6), `KeystrokeLatencyTests.cs` (1).
   Note Graphing and Currency are 100% mocks upstream, so their suites cannot be
   replayed meaningfully against real behaviour.
5. Pushed to the fork `ell-shad/calculator` as branch `linux-port`.

## State

Branch `linux-port`. Suite: **549 + 18 = 567** managed tests, ctest green.
`AUDIT/run_all.sh`: **10 steps**, and `AUDIT_REQUIRE_DISPLAY=1` makes a missing
X display fatal instead of a quiet skip.