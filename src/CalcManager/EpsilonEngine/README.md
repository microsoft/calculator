# Epsilon scientific adapter

`EpsilonEngine.h` is a C++17-compatible PIMPL boundary. Epsilon types occur
only in `EpsilonEngine.cpp`, which must be compiled as C++23.

## Build integration

Add `EpsilonEngine/EpsilonEngine.cpp` to `CalcManager`. For that item only:

- set `LanguageStandard` to `stdcpp23` (VS v145 maps this to
  `/std:c++23preview`);
- set `PrecompiledHeader` to `NotUsing`, because CalcManager's PCH is built
  under C++20;
- clear `ForcedIncludeFiles` for this item so the project PCH is not forced;
- do not add a global Epsilon include directory. Relative includes deliberately
  keep the generic vendored header names private.

Add `CalculatorUnitTests/EpsilonEngineTests.cpp` to the native test project.
It consumes only the public PIMPL header and remains C++17.

## Deterministic limits

| Resource | Limit |
|---|---:|
| Editable numeric input | 256 characters |
| Pratt tokens | 256 |
| Parenthesis/lazy-operation depth | 32 |
| Operations retained in a lazy value | 128 |
| Input decimal exponent/scale | +/-256 |
| Display precision | 1-100 significant decimal digits (clamped) |
| Bounded zero/sign probe | 640 base-4 places |
| Materialized integer magnitude | 54 32-bit limbs (~520 decimal digits) |
| Trigonometric argument before angle conversion | absolute value <= 1,000,000 |
| Exponential-function argument | absolute value <= 1,000 |
| Formatted output | 1,024 characters |

Limits are checked before or while composing/materializing lazy Epsilon values.
Limit failures use the calculator's overflow display contract. Domain and
divide-by-zero failures use their existing display errors. Unsupported
commands are rejected before state changes with `std::invalid_argument`.
The bounded probe is never treated as a proof that a real is exactly zero:
exact zero, one, sign, and cancellation facts are retained only when they
follow soundly from input or an operation. If a nonzero/sign decision remains
unresolved at the probe limit, the adapter reports overflow instead of
displaying a false zero or a false domain error. Values whose first significant
decimal digit is beyond the 370-place materialization budget likewise report
overflow.

Exact-relation metadata is deliberately small: canonical decimal input atoms,
opaque identities for retained Epsilon values, bounded signed-64-bit decimal
addition/subtraction certificates, and a bounded 64-bit perfect-square
certificate. One-step certificates preserve `(x / y) * y`, `sqr(sqrt(x))`,
base-10 logs of exact powers of ten, and exact quadrant zeros/poles for
decimal DEG/GRAD inputs and pi in RAD mode. The metadata is
not recursive, is not an AST, does not produce runtime results, and is not a
parallel integer/rational evaluator.

Primary results use the configured count as significant decimal digits,
preserve the locale decimal separator, and apply `sThousand`/`sGrouping` to
fixed-format output, including the editable primary display. Grouping changes
only presentation; the editable lexeme and retained Epsilon value are separate.
Terminal-zero grouping patterns such as `3;0` and
`3;2;0` repeat their preceding group. Scientific-format mantissas remain
ungrouped. Large values are normalized before decimal materialization so
significant-digit rounding occurs once. Scientific output preserves the
explicit exponent sign and decimal marker used by the existing UI, including
`0.e+0`, `1.e+3`, and `-1.e+0` when F-E formatting is enabled.

The first equals preserves Calculator button semantics: unmatched opening
parentheses are closed, and a trailing binary operator repeats the current
operand (`4 + =` produces `8`). A completed result ignores subsequent equals.
Pending operators reduce only the current parenthesized group and operators
that bind at least as tightly as the incoming operator. Thus `1 + 2 *` keeps
the primary display at `2`, while `2 * (2) + =` uses the retained displayed
value `4` for the omitted right operand and produces `8`.
Opening a group after an operand and entering a digit after a closed group
insert implicit multiplication. Structurally malformed expressions still
produce an explicit domain error.

Clear Entry removes only the editable or retained current operand and displays
zero. `IsInputEmpty()` then reports true even when a pending expression prefix
remains, allowing the UI to expose Clear so the next Clear removes that prefix.

Editable operands remain on the primary display and are not published as
committed expression tokens. Operators publish with Calculator spacing and
glyphs; failed division retains the last valid prefix. Unary failures publish
the attempted unary expression. History receives one display-only token with
command index `-1` and an empty command list, so Scientific history cannot be
replayed or edited.

Exponent entry always includes an explicit sign (`1.e+3`). A retained unary or
constant value remains an Epsilon value while its exponent is edited; formatted
display text is never reparsed. Backspace restores the retained value, and a
binary operator cancels an incomplete retained exponent before continuing.
