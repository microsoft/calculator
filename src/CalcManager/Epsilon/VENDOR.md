# Epsilon vendor record

- Upstream: <https://github.com/tian-lt-personal/epsilon>
- Source: the complete upstream `src/epsilon` directory
- License: MIT; see `LICENSE`
- Vendored: 2026-09-14

The upstream source files were copied mechanically with their relative names,
namespaces, copyright notices, and SPDX identifiers intact.

## Local patches

1. `ops.hpp`: changed `#include <z.hpp>` to `#include "z.hpp"`. This is an
   include-resolution-only fix: the vendored headers are consumed from their
   own directory without adding a globally visible `z.hpp` include path. It
   has no numeric or API effect.
2. `r.hpp`: constrained the forwarding constructor so it cannot accept
   `r` itself, and explicitly defaulted copy/move operations. Without this,
   MSVC selects the forwarding constructor for a non-const `r` lvalue and
   attempts to construct the internal `std::function` from an `r`. This is a
   compilation-only fix that preserves the intended value semantics.
3. `chars.hpp`: replaced the native `double` approximation used to convert
   decimal digits to base-4 realization precision with the conservative
   integer upper bound `ceil(k * 1661 / 1000)`. This prevents binary floating
   point and truncation from controlling Epsilon result materialization.
   `SquareRootRegressionValues` covers the rounded irrational output.

The adapter applies input, operation, nesting, precision, magnitude, and
realization budgets before invoking the library. If another compilation or
numerical defect requires a source patch later, record the file, exact change,
reason, and regression test in this section.

## Compiler requirement

The headers use C++23 concepts, ranges, coroutines, and designated
initializers. Translation units that include them must use C++23. Consumers of
`EpsilonEngine.h` do not include Epsilon and remain C++17-compatible.
