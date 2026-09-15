# Epsilon Scientific prototype

See [EpsilonValidation.md](EpsilonValidation.md) for the executed test totals,
VM matrix, package provenance, evidence paths, and hosted-CI limitation.

This branch replaces the Scientific calculation path with an experimental
Epsilon adapter. It is not feature-equivalent to the original Scientific
calculator. Unsupported controls are disabled rather than delegated to RatPack.
Standard, Programmer, conversion, date, and graphing paths retain their existing
engines.

## Ownership and routing

```text
XAML
  |
  v
StandardCalculatorViewModel (C#, shared by all three calculator modes)
  |
  v
CalculatorManagerWrapper (C++/WinRT)
  |
  v
CalculatorManager
  |
  +-- Standard / Programmer --> CCalcEngine --> RatPack
  |
  +-- Scientific -----------> EpsilonEngine --> vendored Epsilon
                                |
                                +-- editable input
                                +-- small Pratt parser
                                +-- retained lazy real values
                                +-- formatted display / read-only history

Results:
  EpsilonEngine -> ICalcDisplay -> CalcDisplayBridge -> WinRT delegates
                -> CalculatorDisplay -> C# ViewModel -> XAML
```

The manager owns the Scientific adapter. Interop does not own another engine,
and there is no P/Invoke or C# expression evaluator. The legacy engine pointer is
not used while Scientific mode is active.

Epsilon's `epx::r<epx::default_container_type>` is the expression number type.
Its required integer internals remain part of the library; there is no separate
integer expression evaluator. Epsilon types are private to the adapter
implementation, not part of the WinRT contract or the public native header.

Editable numeric text, retained numeric values, and formatted display strings
have separate roles. Formatting a result does not replace its retained value,
and a subsequent calculation must not parse the rounded or localized display.

## Prototype capabilities

| Area | Scientific behavior |
| --- | --- |
| Input | Decimal and scientific notation, unary signs, parentheses, backspace, CE, C; existing auto-completion on first equals |
| Arithmetic | Addition, subtraction, multiplication, division; normal precedence; left-associative subtraction/division |
| Unary operations | Immediate square, square root, reciprocal, including closed groups |
| Constants/functions | Pi, e, sin, cos, tan, ln, base-10 log, exp |
| Angle units | DEG, RAD, GRAD |
| Results | Existing Scientific display precision and formatting; continuation retains the Epsilon value |
| Repeated equals | No-op after a completed result |
| History | Append/display only; no loading, replay, editing, removal, or clearing |
| Memory | Disabled in Scientific; existing Standard/Programmer memory is preserved |
| Deferred | Arbitrary powers/roots, factorial, inverse/hyperbolic functions, mod, percent, and other unimplemented operations |

The first equals preserves the existing completion policy: `(2+3` completes to
`5`, and `4+` completes to `8`. Further equals presses do not repeat the operation
or add another history entry.

The capability source of truth is the native manager, exposed by
[CalcManagerInterop.idl](../src/CalcManager.Interop/CalcManagerInterop.idl):

```text
IsCommandSupported(command) -> Boolean
IsMemorySupported           -> Boolean
IsHistoryReadOnly           -> Boolean
```

The ViewModel uses these capabilities for both controls and command guards.
Native code independently rejects unsupported operations before changing input.
Direct WinRT calls receive an explicit not-implemented error for unsupported
capabilities. Mathematical errors use the existing localized error/display
callbacks instead.

Scientific expression/history tokens are display-only, with command index `-1`
and no replay commands. Imported Scientific history is copied and stripped of
legacy replay metadata; the original records are not modified. Snapshot and
history consumers must not interpret those display tokens as Pratt input.
Scientific snapshot/Recall restoration restores only display history and resets
the live input to the native value `0`, preserving other modes' memory. A saved
formatted result is not restored as an active operand.

## Source and compiler boundary

Vendored source comes from
[tian-lt-personal/epsilon](https://github.com/tian-lt-personal/epsilon).
Only its mathematical library is
vendored, not its expression engine. The source repository is not modified.
Upstream structure and notices are retained in
[Epsilon](../src/CalcManager/Epsilon). The application's packaged
[NOTICE.txt](../NOTICE.txt) includes Epsilon's MIT attribution and license.

The Pratt approach is informed by the small token and parser patterns in
[tian-lt-personal/luca](https://github.com/tian-lt-personal/luca).
It does not import Luca's integer
conversion, type system, optimizer, arena, modules, or lexer generator.

Use Visual Studio 2026, v145, and Windows SDK 10.0.26100.0. Keep the existing
C++20 setting for legacy CalcManager translation units and C++17 for Interop
and native tests. Epsilon translation units require C++23 and must not consume
a C++20 PCH. In the installed v145 tools, MSBuild's `stdcpp23` setting selects
`/std:c++23preview`. Do not leave a conflicting `/std:c++20` in additional
compiler options.

## Build and validation

Build only Debug x64 for this prototype:

```powershell
msbuild .\src\Calculator.slnx -restore -m -t:Publish `
  -p:Configuration=Debug -p:Platform=x64 -p:RestorePackagesConfig=true `
  -p:UseReleaseAppxManifest=false -p:IsStoreBuild=false -p:AppxBundlePlatforms=x64 `
  -p:GenerateProjectSpecificOutputFolder=true `
  -p:OutDir=C:\code\calc\output\epsilon-debug-x64\ `
  -p:PublishDir=C:\code\calc\output\epsilon-debug-x64\publish\ `
  -bl:C:\code\calc\output\epsilon-debug-x64\Calculator.binlog
```

Regenerate WinRT metadata and projections through the build; do not edit
generated files. If the normal NuGet v3 endpoint fails TLS negotiation, an
environment-specific restore can use the official HTTPS v2 source without
changing project dependency versions or the compiler toolchain:

```text
-p:RestoreSources=https://www.nuget.org/api/v2/
```

Run the freshly built native and managed MSIX test packages with the Visual
Studio VSTest runner, not `dotnet test`. Follow the CI signing/trust procedure
in [SignTestApp.ps1](../build/scripts/SignTestApp.ps1). That script requires an
already elevated shell; do not trigger unattended elevation.

### Recommended reusable certificate for local testing

For repeated local validation, configure a persistent local test code-signing
certificate instead of creating a new temporary certificate for every build:

1. Keep its private key securely in the current user's certificate store
   (`CurrentUser\My`), with signing access limited to the intended user.
   The certificate subject must match the test package's publisher.
2. Use administrator approval once to import only the public certificate into
   the host's `LocalMachine\TrustedPeople` store.
3. Update the local validation scripts to reuse that certificate for signing,
   keeping certificate provisioning separate from the normal test workflow.
   Rebuilt packages still need signing, but the trusted certificate need not
   be recreated or imported again for each package.

After this setup, the normal build -> sign -> test cycle should usually run
without repeated UAC approvals. Keep UAC enabled and run VS Code without
administrator privileges. Renewing or replacing the certificate may require
another administrator-approved trust update.

This is a recommended future workflow, not a change already implemented here.
The current `SignTestApp.ps1` still creates a new one-hour certificate and
imports it into machine-level trust on every invocation, requiring elevation.
No certificate or signing-script configuration is changed by this guidance.

### Running packaged tests

```powershell
vstest.console.exe $nativeTestPackage /Platform:x64 /Logger:trx
vstest.console.exe $managedTestPackage /Platform:x64 /Logger:trx `
  -- RunConfiguration.TreatNoTestsAsError=true
```

Managed integration tests must assert that the real WinRT wrapper initialized,
then exercise ViewModel commands and result/error callbacks. A constructed
ViewModel or a visible window is not proof that the native engine loaded.

For Hyperloop validation, use the existing designated VM, acquire ownership,
deploy the fresh loose AppX layout, and launch the installed
`Microsoft.WindowsCalculator.Dev` AUMID. Verify both package identity and build
provenance. Do not launch the ambiguous `Calculator` name or shared URI. Use
UI Automation identifiers, button and keyboard input, assertions, screenshots,
and scoped health/event-log checks. Release ownership even on failure.

Deployment must preserve per-user state. Do not recreate the VM, uninstall the
inbox Calculator, or use destructive reinstall/provisioning options. The Dev
app remains **Calculator [Dev]**, separate from the inbox application.

For this managed UWP application, use the layout extracted from the freshly
built x64 MSIX and include its generated `Dependencies\x64` directory.
The build root's AppX recipe alone may omit the .NET CoreFramework and
CoreRuntime framework packages. The root executable is the UWP bootstrapper;
`entrypoint\CalculatorApp.exe` is the managed application assembly. Record
hashes for both, the native and managed implementation DLLs, and the manifest.
Use the manifest and installed Dev AUMID rather than folder names to verify
the deployed application.
The development manifest is version `0.0.2.0`, allowing an in-place upgrade
from the baseline `0.0.1.0`. Re-registering the same identity and version from
a different loose-layout directory can leave Windows using the old directory
even when deployment reports success. Verify the installed location and binary
hashes, not only the deployment tool's success flag.

When a previous test package with the same identity and version is installed,
Windows rejects changed package contents. For local validation, make a copy
using the SDK's MakeAppx unpack/pack commands and increment only the test
package manifest version with
[UpdateAppxManifestVersion.ps1](../build/scripts/UpdateAppxManifestVersion.ps1).
Sign and run that upgrade copy, retaining the original binary hashes. This
avoids uninstalling the baseline test app or deleting its data. Do not change
production manifests or dependency versions to bypass deployment conflicts.

Retain baseline and integration logs separately. Report each exercised case
with its input/actions, expected and actual result, status, and evidence path.
Keep build outputs, screenshots, certificates, and transient validation logs
out of source commits. The validation report must distinguish actual passes
from failures, prerequisites, and unexecuted cases.
