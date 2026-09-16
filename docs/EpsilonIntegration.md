# Epsilon Scientific architecture

Scientific mode now uses a stateful `ScientificCalculator` over a stateless
`EpsilonEngine`. Standard and Programmer retain `CCalcEngine`/RatPack; converter,
date and graphing engines are unchanged.

```text
XAML / keyboard / paste / history / snapshots
    -> StandardCalculatorViewModel
    -> CalculatorManagerWrapper (C++/WinRT)
    -> CalculatorManager
         -> Standard / Programmer: CCalcEngine
         -> ScientificCalculator
              -> canonical expression source
              -> re2c lexer -> focused Pratt parser -> owned AST
              -> iterative Epsilon evaluator -> value / diagnostic
              -> numeric formatting -> localized display callbacks
```

## Responsibilities

`EpsilonEngine::Evaluate` takes complete canonical source and explicit limits.
It owns no calculator session state. Independent calls have independent lazy
graphs; values outlive parser/source storage. Public headers remain usable from
C++17. See the [engine reference](..\src\CalcManager\EpsilonEngine\README.md)
for grammar, numerical semantics, diagnostics and exact resource limits.

`ScientificCalculator` owns commands, editable input, original expression
provenance, completion/repeated equals, settings, RNG, errors, formatting,
callbacks, Scientific memory and history. Continuation always re-emits the
original expression, never rounded display text or opaque value bindings.
Angle units and sampled random literals are frozen in source.

The design borrows LUCA's re2c/token/span and Pratt/AST separation patterns,
not its integer runtime, type system, optimizer, modules or build system:
<https://github.com/tian-lt-personal/luca>.
The inspected LUCA revision was `5319b717ef21de7dbf9a5ade39b60330951da6d2`;
it is a design reference, not a linked or vendored dependency.
The mathematical backend remains the vendored Epsilon library; notices and
targeted local patches are recorded in its `VENDOR.md`.

## Restored Scientific features

Power/root and variable-base logarithms, cube/cube root, real factorial,
inverse/reciprocal/hyperbolic functions, modulus, percent, absolute value,
floor/ceil, DMS/degrees and random are available through native capabilities.
Repeated equals is restored. Scientific memory now supports store, recall,
addition, subtraction, deletion and clear.

**Intentional differences:** Scientific memory is separate from the shared
Standard/Programmer collection, and exact-real results are preferred over legacy
finite-precision heuristics. Work and magnitude are bounded; mathematically
undecidable/unresolved boundary classifications report resource errors.

Capability checks remain native and are consumed by controls and command guards.
Scientific still rejects Programmer-only operations before input mutation.
The richer internal expression language does not automatically expand clipboard
syntax: paste retains character/command preflight and locale normalization.

## History, editing and snapshots

New history records carry a versioned Scientific state in addition to display
text. Loading a record restores its exact source and settings. Completed
Scientific expression operands/operators are selectable for native editing;
multi-digit replacement is supported. Delete/clear remain routed through the
existing history collection.

Snapshots serialize the live editor, including incomplete input and repeat state.
Native restore validates state into a temporary calculator before publication.
Managed snapshot/JSON aliases and WinRT wrappers preserve the same payload.
Memory is not part of this payload and is preserved during Scientific restore.

Old command-backed records retain their legacy replay path. Old display-only
records cannot recover exact provenance; they remain viewable and removable
but are not replayable. Display token positions are never parsed as source.
Standard/Programmer snapshot command validation is not bypassed by the presence
of a Scientific payload.

## Build and tests

Use Visual Studio 2026/v145, Windows SDK 10.0.26100.0, and the existing solution
configuration. Provision re2c before building:

```powershell
.\build\scripts\SetupRe2c.ps1
msbuild .\src\Calculator.slnx -restore -m -t:Build `
  -p:Configuration=Debug -p:Platform=x64 -p:RestorePackagesConfig=true
```

The setup script verifies and builds pinned re2c 4.6 locally. Both GitHub and ADO
build entry points invoke it. `Re2cExe` can point to a pre-provisioned executable.
Generated lexer output belongs to MSBuild's intermediate directory, not source.

Use the Visual Studio VSTest runner for native and managed UWP tests. On a
developer-mode host, the freshly built `.build.appxrecipe` can deploy a loose
test layout without a package-signing certificate:

```powershell
vstest.console.exe .\src\x64\Debug\CalculatorUnitTests\CalculatorUnitTests.build.appxrecipe /Platform:x64 /Logger:trx
vstest.console.exe .\src\Calculator.Tests\bin\x64\Debug\Calculator.Tests.build.appxrecipe /Platform:x64 /Logger:trx
```

Signed MSIX tests remain supported by the existing signing workflow. Do not
disable UAC, uninstall existing Calculator packages, clear user data, or silently
elevate to satisfy deployment prerequisites. Close the specific previous test
process before redeploying a locked layout.

Core tests cover syntax/spans, lifetime/statelessness, exactness counterexamples,
numeric domains, independent references and limits. Scientific interaction tests
exercise normalization, editing, memory and state round trips. Managed tests must
initialize the real WinRT wrapper and exercise its callbacks. Build/test outputs,
reference libraries and transient evidence remain outside source control.

`EpsilonValidation.md` records the refreshed engine's acceptance results separately
from the earlier prototype's historical evidence.
