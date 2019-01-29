# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT License.

<#
.SYNOPSIS
    Updates the version number in an AppxManifest file.

.PARAMETER AppxManifest
    The path to the AppxManifest file.

.PARAMETER Version
    The version number to write into the file.

.EXAMPLE
    Update-AppxManifestVersion -AppxManifest "C:\App\Package.appxmanifest" -Version "3.2.1.0"
#>
param(
    [ValidateScript({Test-Path $_ -PathType 'Leaf'})]
    [Parameter(Mandatory)]
    [string]
    $AppxManifest,

    [ValidateScript({[version]$_})]
    [Parameter(Mandatory)]
    [string]
    $Version
)

$xmlDoc = [XML](Get-Content $AppxManifest)
$xmlDoc.Package.Identity.setAttribute("Version", $Version);
$xmlDoc.Save($AppxManifest)