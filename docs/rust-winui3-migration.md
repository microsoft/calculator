# Rust + WinUI 3 migration plan for this repo

## Goal

Recreate the Windows Calculator experience using a Rust core and a WinUI 3 front-end while preserving the feature set and UX of the current app.

## Recommended architecture

### 1. Rust engine

Use Rust as the primary computation layer for:

- standard arithmetic,
- scientific operations,
- programmer mode conversions,
- date arithmetic,
- unit conversion tables,
- memory and history state.

This repo now includes a starter implementation under `src/rust-calculator`.

### 2. WinUI 3 shell

Keep the app shell in a WinUI 3 project with:

- `MainWindow.xaml`
- keypad controls,
- result display,
- mode switcher,
- theme support,
- accessibility features.

### 3. Interop boundary

Use a thin native bridge to connect UI events to the Rust engine.

- Rust exports a small C-compatible function set.
- WinUI 3 invokes those functions through a native DLL interface.
- The UI remains in XAML while the logic stays in Rust.

## Recommended migration order

1. Port the calculator expression engine first.
2. Add memory and history.
3. Add standard/scientific mode.
4. Add programmer model and radix conversion.
5. Add date and unit conversion.
6. Add graphing shell support last.

## Feature parity checklist

- Standard calculator
- Scientific operations
- Programmer mode
- Date calculation
- History and memory
- Unit conversion
- Graphing UI shell
- Theme support
- Accessibility

## Kicking off the migration

The starter Rust engine is already committed in `src/rust-calculator` and is designed to be the first reusable slice of the migrated app.
