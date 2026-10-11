# AUDIT PROGRESS

PROJECT_ROOT: /home/elshad-guliyev/Documents/calculator   MODE: AUDIT_AND_FIX
REFERENCE: microsoft/calculator @ d125246a4e19842ce1332e6c7839cf0e110027d8

## Done
- [x] Phase 0 RECON - clean-checkout build via documented steps only
- [x] Phase 1 INVENTORY (reference side complete: 3264 rows)
- [x] Phase 2 ENGINE - **158/158 of the reference's own golden vectors replay green**
- [x] Phase 3 UI - headless-Skia real-window rendering + visual-tree completeness
      diff; **all 3 keypads now expose 100% of the reference ButtonIds**
- [x] Phase 4 CROSS-CUTTING (part) - keyboard shortcut parity (42 tests) and
      locale matrix (22 tests) now automated and green
- [~] Phase 5 PACKAGING (part) - deb + lintian + extract + exit code verified
- [x] Phase 6 FIX + REGRESS - 5 defects fixed, suite green

## Defects fixed this pass
S2-001 keypad completeness | S3-002 build-deb exit code | S3-004 layout-independent
keys | S3-005 missing shortcuts  (S3-003 currency names fixed earlier)

## Hard blocker (unchanged)
On-screen UI-RUN impossible: the Wayland compositor will not map any new X11
toplevel (stock `xmessage` fails identically). Avalonia 11.3 is X11-only.
Remedy: log out / log in, then run REPORT.md section 7.

## Next steps
1. Re-verify the keypads and shortcuts on a working display (REPORT section 7).
2. Scaling 125/150/200%, accessibility/AT-SPI/Orca, soak + RSS growth.
3. NuGet transitive licence audit.
4. Reference C# unit tests (src/Calculator.Tests, 288 methods) still not replayed
   - only the C++ engine tests (86 methods, 158 vectors) are.

## State
Branch `linux-port`. Suite: **432 managed tests + 1 ctest, green**.
AUDIT/run_all.sh: ALL CHECKS GREEN.
