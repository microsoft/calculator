# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT License.

<#
.SYNOPSIS
    Updates the package identity in an AppxManifest file.

.PARAMETER AppxManifest
    The path to the AppxManifest file.

.PARAMETER Identity
    The package identity to write into the file.

.EXAMPLE
    Update-AppxManifestIdentity -AppxManifest "C:\App\Package.appxmanifest" -Identity "Microsoft.AppName"
#>
param(
    [ValidateScript({Test-Path -Path $_ -PathType 'Leaf'})]
    [Parameter(Mandatory)]
    [string] $AppxManifest,

    [Parameter(Mandatory)]
    [string] $Identity
)

$xmlDoc = [XML](Get-Content $AppxManifest -Encoding UTF8)
$xmlDoc.Package.Identity.setAttribute("Name", $Identity)
$xmlDoc.Save($AppxManifest)
