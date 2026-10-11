# AUDIT PROGRESS

PROJECT_ROOT: /home/elshad-guliyev/Documents/calculator   MODE: AUDIT_AND_FIX
REFERENCE: microsoft/calculator @ d125246a4e19842ce1332e6c7839cf0e110027d8
  (cloned to /home/elshad-guliyev/ref-calculator, read-only, never modified)

## Done
- [x] Phase 0 RECON — mapped the port; clean-checkout build via documented
      steps only (`evidence/logs/cleanbuild.log`); unfinished-work hunt;
      established the engine is the unmodified upstream Ratpack rational engine.
- [x] Phase 1 INVENTORY (reference side complete, 3264 rows)
- [x] Phase 2 ENGINE (exactness, precision, factorial, overflow, domain errors,
      radix, history cap, grouping) + data parity vs reference
- [x] Phase 3 UI — **partial**: headless-Skia real-window rendering + visual-tree
      completeness diff for all three keypads. On-screen UI-RUN BLOCKED by the
      wedged compositor (see below).
- [~] Phase 4 CROSS-CUTTING — partial (currency, clipboard not yet, locales not yet)
- [~] Phase 5 PACKAGING — partial (deb, lintian, extract, exit code; licences not yet)
- [x] Phase 6 FIX + REGRESS — 3 defects fixed, regression tests added, suite green

## Hard blocker
`XDG_SESSION_TYPE=wayland`; Avalonia 11.3 is X11-only so the app uses Xwayland.
The compositor will not map ANY new X11 toplevel — proven with a stock
`xmessage` (also IsUnMapped) and with the pre-audit build (also fails).
Not an app defect. Remedy: log out / log in, then run REPORT §7.

## Next steps (highest value first)
1. Replay the 534 reference golden vectors (`evidence/reference_golden_vectors.md`)
   against the port as assertions. Mechanical, high yield.
2. Diff the 220 keyboard bindings against the port's key handler. Likely S3.
3. Locale matrix (de_DE, ru_RU, en_IN, RTL) once a session works.
4. Scaling 125/150/200%, accessibility/AT-SPI, soak + RSS growth.
5. NuGet transitive licence audit.

## State
Branch `linux-port`. Audit fixes: 51ff26d (S2 keypad), packaging exit code (S3),
currency names (S3, f1d4e93). Suite: 210 managed + 1 ctest, green.
