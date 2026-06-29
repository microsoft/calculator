# Copyright (c) Microsoft Corporation. All rights reserved.
<#
.SYNOPSIS
    Verifies that the C# source is free of unnecessary 'this.' qualifications
    (IDE0003), enforcing the '.editorconfig' single-source-of-truth policy.

.DESCRIPTION
    The Roslyn IDE-prefixed analyzers (IDE0003 et al.) ship with the .NET SDK
    but are not reliably surfaced by csc.exe at build time. The 'dotnet format
    style' host runs the same Roslyn analyzer and honors the '.editorconfig'
    severity ('dotnet_diagnostic.IDE0003.severity = warning'), so this script
    uses it as the build-time enforcement gate.

    'dotnet format' uses the SDK MSBuild workspace loader, which cannot load
    old-style UWP projects (the main Calculator app and its managed view
    models) or the native C++ test projects. Only SDK-style C# projects are
    listed in the allowlist below; all projects share the same '.editorconfig'
    rules. The allowlist is a positive list, so new projects are
    safe-by-default until explicitly added.

    Restore is performed explicitly per project (and its exit code checked)
    before formatting: the allowlisted projects carry PackageReference
    dependencies that must be restored to load in the workspace, and a restore
    failure should fail the gate loudly rather than letting 'dotnet format'
    proceed against a partially loaded workspace.

.PARAMETER Diagnostics
    Comma-separated diagnostic IDs to enforce. Defaults to 'IDE0003'.

.EXAMPLE
    .\build\scripts\VerifyCodeStyle.ps1
    Verifies the allowlisted SDK-style C# projects against IDE0003.

.EXAMPLE
    .\build\scripts\VerifyCodeStyle.ps1 -Diagnostics 'IDE0003,IDE0005'
    Also enforces IDE0005 (remove unnecessary usings).
#>
[CmdletBinding()]
param(
    [string] $Diagnostics = 'IDE0003'
)

$ErrorActionPreference = 'Stop'

# Resolve the repo root (this script lives in <root>\build\scripts).
$root = if ($env:BUILD_SOURCESDIRECTORY) { $env:BUILD_SOURCESDIRECTORY } else { (Resolve-Path (Join-Path $PSScriptRoot '..\..')).Path }

# SDK-style C# projects that 'dotnet format' can load. The old-style UWP
# projects (Calculator, Calculator.ManagedViewModels) and the native C++ test
# projects (CalculatorUnitTests) are excluded because the dotnet CLI workspace
# loader cannot evaluate them.
$sdkProjects = @(
    'src\CalculatorUITestFramework\CalculatorUITestFramework.csproj',
    'src\CalculatorUITests\CalculatorUITests.csproj'
)

Write-Host "Verifying code style for diagnostics: $Diagnostics"
Write-Host "Tooling: dotnet $(dotnet --version)"
Write-Host "Repo root: $root"
Write-Host ""
Write-Host "SDK-style projects to check:"
$sdkProjects | ForEach-Object { Write-Host "  $_" }
Write-Host ""

$failed = $false

foreach ($relPath in $sdkProjects)
{
    $projPath = Join-Path $root $relPath

    if (-not (Test-Path $projPath))
    {
        Write-Host "##[error]Allowlisted project not found: $relPath"
        $failed = $true
        continue
    }

    Write-Host "Checking: $relPath"

    # Restore explicitly first so a restore/feed failure fails the gate loudly,
    # rather than letting 'dotnet format' run against a partially loaded
    # workspace and report success without analyzing anything.
    & dotnet restore $projPath

    if ($LASTEXITCODE -ne 0)
    {
        Write-Host "##[error]Restore failed for: $relPath"
        $failed = $true
        continue
    }

    & dotnet format style $projPath `
        --no-restore `
        --verify-no-changes `
        --severity warn `
        --diagnostics $Diagnostics

    if ($LASTEXITCODE -ne 0)
    {
        Write-Host "##[error]Code style violation ($Diagnostics) in: $relPath"
        $failed = $true
    }
}

if ($failed)
{
    Write-Host ''
    Write-Host "##[error]Code style violations detected ($Diagnostics)."
    Write-Host '##[error]Run the following locally to fix:'
    Write-Host "##[error]  dotnet format style <project.csproj> --severity warn --diagnostics $Diagnostics"
    exit 1
}

Write-Host ''
Write-Host "##[section]Code style verification passed (no $Diagnostics violations)."
