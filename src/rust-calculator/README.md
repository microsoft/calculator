# Rust Calculator Engine for a WinUI 3 Migration

This folder contains a Rust-based calculator engine prototype intended to replace the current Windows calculator math core while preserving the WinUI 3 desktop shell.

## Why this is relevant

The current project is a large WinUI/UWP C# + C++ app. A realistic migration path is:

- use Rust for the computation and state-management engine,
- keep a WinUI 3 front-end for the actual app shell,
- bridge the UI to Rust via a native interop boundary.

## Included prototype

The code in `src/lib.rs` implements:

- calculator mode selection,
- expression evaluation through `meval`,
- memory operations,
- history tracking,
- test coverage for the core engine.

This is a foundation for the migration, not a full Windows app clone.

## Recommended migration layout

```text
src/
  rust-calculator/
    Cargo.toml
    src/
      lib.rs
      main.rs
  Calculator/
    existing WinUI/UWP app (current state)
```

## WinUI 3 integration strategy

1. Keep the Rust engine in a standalone crate.
2. Expose a small `extern "C"` API for the window shell.
3. Call the Rust engine from the WinUI 3 app through a native bridge.
4. Keep the XAML UI for layout and controls.
5. Move advanced calculator logic into Rust incrementally.

## Example Rust engine API

The engine exposes a simple state-based model with:

- `CalculatorEngine::new()`
- `set_mode(...)`
- `append_expression(...)`
- `evaluate_expression(...)`
- `memory_add(...)`
- `memory_clear()`
- `history()`

This is designed to be the core of the migrated app while the WinUI 3 shell remains the presentation layer.

## Run it

```bash
cd src/rust-calculator
cargo run
```

## Important note

This prototype intentionally does not attempt to fully recreate the entire Windows Calculator app in one step. It focuses on the most portable and reusable part: the engine.
