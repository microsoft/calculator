# Stateless Epsilon expression engine

`EpsilonEngine::Evaluate(source, limits)` is a C++17-compatible, stateless native
entry point. It returns an `EvaluationResult`: either an owned `EpsilonValue` or
an `ExpressionException` containing an error code and source span. Internal
implementation files use C++23.

```text
ScientificCalculator -> complete canonical source
                      -> re2c Lexer -> Pratt Parser -> owned AST
                      -> bounded, iterative Epsilon evaluator -> EpsilonValue
                      -> numeric formatting -> localized display
```

The engine has no session fields, command dependency, display callback, resource
provider, angle setting, history, memory, RNG, or retained-result environment.
Every invocation owns its source, AST, proof metadata, work accounting and lazy
real graph. Independent requests can execute concurrently. Values survive the
source and AST; copies own independent lazy graphs. Do not format the **same**
value concurrently: Epsilon's approximation cache is mutable.

`ScientificCalculator` owns all interaction state and features. It keeps original
expression provenance even after formatting and submits that source again for
continuations. Thus `1/3=`, then `*3=` evaluates `(1/3)*3`, not rounded display text.
No hidden value bindings, `<retained>` tokens, `Ans`, RatPack fallback, or C#
evaluator participate in Scientific calculations.

## Grammar

Canonical input is ASCII and locale-independent. Numbers include `2`, `.5`,
`2.`, `1.e+3`, and `1e-3`. Prefix signs are operators; an exponent requires digits.
Constants are `pi` and `e`.

| Syntax | Meaning |
|---|---|
| `+x`, `-x`, `(x)` | Prefix signs and grouping |
| `x+y`, `x-y`, `x*y`, `x/y` | Arithmetic |
| `x mod y` | Scientific modulus; sign follows the divisor |
| `x^y`, `pow(x,y)` | Real power |
| `x root y`, `root(x,y)` | y-th root of x |
| `x logbase y`, `logbase(x,y)` | Logarithm of x to base y |
| `x!`, `fact(x)` | Factorial, including real gamma continuation |
| `sqr`, `cube`, `sqrt`, `cbrt`, `recip`, `abs` | Unary mathematical functions |
| `ln`, `log`/`log10`, `exp` | Natural/base-10 logarithm and exponential |
| `floor`, `ceil`, `trunc`, `frac`, `dms`, `degrees` | Discrete and angle-display conversions |
| `sin`, `cos`, `tan`, `sec`, `csc`, `cot` | Trigonometric functions |
| `asin`, `acos`, `atan`, `asec`, `acsc`, `acot` | Principal inverse trigonometric functions |
| `sinh`, `cosh`, `tanh`, `sech`, `csch`, `coth` | Hyperbolic functions |
| `asinh`, `acosh`, `atanh`, `asech`, `acsch`, `acoth` | Inverse hyperbolic functions |

Unary names use parentheses. Trigonometric and inverse-trigonometric calls accept
an optional second argument `deg`, `rad`, or `grad`; absent units mean radians.
Units are not standalone values. Hyperbolic functions do not accept units.
`ScientificCalculator` always emits the selected unit explicitly and preserves it
when the expression is reused under a different angle mode.

Calls/postfix factorial bind most tightly, followed by power/root/logbase, unary
signs, multiplication/division/modulus, then addition/subtraction. Power is
right-associative; the other infix operators are left-associative. `-2^2` means
`-(2^2)`. Button entry preserves Calculator's grouping policy by explicitly
grouping already committed operations; it need not match an unparenthesized text
power chain.

The parser never evaluates or repairs input. It requires EOF after a complete
expression. Unknown functions, wrong arity/units, malformed numbers, embedded
NUL, adjacent operands and unmatched delimiters produce diagnostics.
There are no variables, assignments, imports, implicit multiplication, percent
operator, commands, or random function in the engine.

## Interaction normalization

ScientificCalculator handles incomplete **editor** state without sending
incomplete expressions to the engine. First equals closes unmatched opening
parentheses and supplies the displayed operand for a trailing operator (`4+=`
becomes `4+4`). Empty groups remain errors. Repeated equals reuses the last
effective operation and its original operand expression.

Immediate unary operations submit a complete function call for the current
operand/group. EXP editing of a computed value emits `(<original source>)*1eN`.
Implicit multiplication becomes `*`. Contextual percent becomes ordinary
arithmetic: `200+10%` becomes `200+(200*10/100)`, while `200*10%` becomes
`200*(10/100)`. Random is sampled in ScientificCalculator and converted to an
exact terminating decimal before entering source; replay never samples again.

The editable primary display, expression labels, canonical source, and numeric
value are separate. Localization and F-E formatting never replace canonical
source. Scientific memory is separate from Standard/Programmer memory.

## Numerical semantics and resource limits

Expression values are Epsilon reals. Bounded exact rational, rational-pi,
zero/sign and cancellation certificates support sound domain decisions and exact
special cases; they are not a second floating-point evaluator. Structural
identities must represent the actual arguments and angle units.

Domain checks precede numerical shortcuts: neither `0*(1/0)` nor
`sqrt(-1)^2` suppresses an error. Exact negative rational powers and odd roots
have explicit real-domain handling. Unknown zero/sign/integer-boundary decisions
use bounded enclosures, not tolerance-based equality. An unresolved decision
reports a resource error rather than a false zero or a false domain result.

Integer factorial uses Epsilon integers; half-integer factorial uses the exact
recurrence from sqrt(pi). Other factorials use Spouge's gamma formula with
outward-rounded fixed-point enclosures and its positive-argument relative
remainder bound (conservatively `6^-a`). Negative inputs are shifted by recurrence.
The kernel only returns an approximation when its final enclosure establishes
Epsilon's one-base-4-unit error contract; otherwise it reports a resource error.
There is no binary floating-point math fallback.

| Resource | Default/hard ceiling |
|---|---:|
| Canonical source | 65,536 characters |
| Lexer tokens / AST nodes | 8,192 each |
| Parser / AST depth | 32 |
| Estimated expression graph work | 4,096 |
| Editable number / literal length | 256 characters |
| Decimal input exponent/scale | +/-256 |
| Significant display digits | 1-100 (clamped) |
| Sign/zero classification | up to 640 base-4 places |
| Fractional display materialization | 370 decimal places |
| Integer magnitude | 54 32-bit limbs (about 520 decimal digits) |
| Exponential argument magnitude | 1,000 |
| Integer power exponent magnitude | 4,096 |
| Integer factorial argument | 0-250 |
| Half-integer factorial numerator magnitude | 200 |
| Gamma recurrence shifts | at most 101 |
| Gamma positive argument / target precision | bounded at 128 / 1,024 base-4 places |
| Serialized Scientific state | 1 MiB |
| Scientific memory / history | 100 / existing CalculatorHistory limit |

Callers may lower `EvaluationLimits`, not raise them past hard ceilings.
Checks cover parser construction and lazy graph expansion, including repeated
squares. Source re-emission grows with continued operations, percent and memory
arithmetic; long sessions can reach limits even if a displayed result is small.
Exact-real correctness takes precedence over legacy tolerance heuristics.

## Diagnostics and persistence

Syntax, domain, divide-by-zero, undefined and resource-limit diagnostics are
distinct. ScientificCalculator maps them to existing localized display errors.
Materialization may also produce an `ExpressionException`; callers must handle
formatting errors as well as evaluation results.

The versioned `Scientific/1` state stores canonical operand source, editable
input, grouping, result provenance, repeat state, angle/inverse/hyperbolic/F-E
settings and display labels. Restore validates and evaluates into a temporary
calculator before replacing the live state. Memory is deliberately not restored
from a snapshot; restoring Scientific state preserves each mode's memory.

New history records carry this state separately from display text and legacy
commands. They support load, operand/operator edits, delete and clear.
Old records with valid legacy commands use the validated command replay path;
old display-only records remain display-only and removable. Lost exact
provenance is never reconstructed from a formatted result.

## Build

Run `build\scripts\SetupRe2c.ps1` once before building. It downloads the official
re2c 4.6 source archive, checks SHA-256, and builds the tool with Visual Studio
CMake. Ordinary builds never download tools.

`build\Re2c.targets` validates the version and generates
`$(IntDir)EpsilonEngine\Lexer.g.cpp` before compilation. Set `Re2cExe` to use an
already provisioned re2c 4.6 executable. Source/tool/target timestamps invalidate
generation; outputs are configuration/platform-local and tracked for clean.
Generated C++ and the local tool directory are not committed.

Evaluator.cpp, EpsilonEngine.cpp, Parser.cpp and the generated lexer compile as C++23 without
the project's C++20 PCH or forced include. ScientificCalculator and other
CalcManager units retain C++20. WinRT consumers and native tests remain C++17.
Do not edit generated WinRT projections.
