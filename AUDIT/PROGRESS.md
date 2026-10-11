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
- [~] Phase 5 PACKAGING — deb + lintian + extract + exit code verified;
      NuGet transitive licence audit added and green
- [x] Phase 6 FIX + REGRESS — 8 defects fixed, suite green

## Reference test replay (Microsoft-authored expectations)

| Reference source | Port test | Result |
|---|---|---|
| `CalculatorUnitTests/CalculatorManagerTest.cpp` (158 vectors) | `ReferenceGoldenVectorTests` | 158/158 |
| `Calculator.Tests/StandardCalculatorViewModelTests.cs` (21 sequences / 114 assertions) | `ReferenceVmGoldenVectorTests` | 22/22 |
| `Calculator.Tests/DateCalculatorTests.cs` | `ReferenceDateEngineTests` | 9/9 |
| `Calculator.Tests/CopyPasteManagerTests.cs` | `ReferenceCopyPasteTests` | 4/4 |
| `Calculator.Tests/NavCategoryTests.cs` | `ReferenceNavCategoryTests` | 61/61 |

Extractors: `AUDIT/extract_golden.py`, `AUDIT/extract_vm_vectors.py`.
Both report what they deliberately skip rather than mis-replaying it.

## Defects fixed

S2-001 keypad completeness · S3-002 deb exit code · S3-004 layout-independent
keys · S3-005 missing shortcuts · S3-006 nav order · S3-007 mode shortcuts ·
S3-008 currency symbol placement. (S3-003 currency names fixed earlier.)

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
2. Scaling at 125 / 150 / 200 %, accessibility/AT-SPI/Orca, soak + RSS growth —
   not yet exercised.
3. Remaining reference test files not yet replayed: `HistoryTests.cs` (36),
   `UnitConverterViewModelTests.cs` (31), `NarratorAnnouncementTests.cs` (19),
   `MultiWindowTests.cs` (16), `SnapshotRoundTripTests.cs` (17).
4. `git remote add fork <url> && git push -u fork linux-port` — not pushed yet.

## State

Branch `linux-port`. Suite: **525 + 18 = 543** managed tests, ctest green.
`AUDIT/run_all.sh`: 9 steps including the licence audit and the on-screen drive.