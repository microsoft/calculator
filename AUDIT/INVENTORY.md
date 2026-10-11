# AUDIT INVENTORY — reference contract

Full machine-extracted inventory: `AUDIT/evidence/reference_inventory_raw.md`
(4,331 lines, 3,264 rows, `file:line` provenance for every row).

## Headline counts (reference @ d125246a)

| Area | Count | Status vs port |
|---|---|---|
| Modes (`NavCategory`) | 19 ViewMode values; 18 categories (5 Calculator + 13 Converter) | 18/18 present in nav |
| Buttons | 200 in the 9 operator/memory/history/bitflip views (+184 elsewhere) | Standard 13/13 · Scientific 59/59 after AUD-S2-001 · Programmer 31/31 after AUD-S2-001 |
| Keyboard bindings | 220 `KeyboardShortcutManager` (79 Char, 76 VK, 28 Ctrl, 24 Shift, 13 Ctrl+Shift) | **NOT VERIFIED** (see REPORT §6) |
| Options/toggles | 13 VM toggles w/ defaults, 3 scientific, 4 radix, 4 shift, 3 theme, 4 bit-length | partial — 2nd/inverse/hyperbolic toggle not implemented (all functions now shown directly, AUD-DEV-001) |
| Resource strings | 1166 `Resources.resw` + 108 `CEngineStrings.resw` | 1166/1166 and 108/108 **exact** |
| Converter data | 158 units, 155 factors, 9 affine temperature entries | **byte-identical** (158/158, 105/105 AddUnit, 154/154 factors) |
| Engine limits | precision 16/32/64, MAX_HISTORY_ITEMS=20, DEFAULT_MAX_DIGITS=32, MAX_STRLEN=84, C_EXP_MAX_DIGITS=4 | inherited from the unmodified shared engine |
| Reference tests | 534 methods (86 C++, 288 C#, 160 UI) | extracted verbatim; **not yet replayed** |

## REAL vs MOCK in the reference (decides what can be compared)

| Area | Verdict | Consequence |
|---|---|---|
| Graphing | **100% MOCK** — `src/GraphingImpl/Mocks/`, namespace `MockGraphingImpl`; `ParseInput` mocks any input, `DrawD2D1` renders nothing and returns S_OK, `GetAnalyzer` returns nullptr, `Serialize` returns L"". No throws: fails silently reporting success. | Graphing numbers **cannot** be compared. Port (real LiveCharts2) is a superset. |
| Currency | **100% MOCK** — `CurrencyHttpClient.cs` returns two hard-coded JSON literals of fictional planet currencies (Mars/Moon/Neptune/…). `HttpClient` constructed, never used. No `Uri`, no URL constant, no HTTP verb anywhere. | Currency numbers **cannot** be compared. Port (live Frankfurter/ECB) is a superset. |
| Unit converter | **REAL** — 155 full double-precision SI/US-customary factors + 9 affine temperature entries | Fully comparable, and the port matches exactly. |

## Known gaps in this inventory
- Settings page / theme toggle parity: not diffed (port is English-only, no Settings page).
- Always-on-top / compact overlay: port does not implement; reference has it.
  **NOT VERIFIED** — flagged for the next pass.
