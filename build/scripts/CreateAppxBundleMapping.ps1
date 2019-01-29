# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT License.

<#
.SYNOPSIS
    Generates a mapping file to be used with the MakeAppx tool. It generates the file based on
    a folder structure grouped by architecture then by project, like this example:

    drop\
        ARM\
            Project\
                AppPackages\
                    Project_ARM.appx
                    Project_scale-100.appx
        x64\
            Project\
                AppPackages\
                    Project_x64.appx
                    Project_scale-100.appx

.PARAMETER InputPath
    The path where appx packages to bundle are located.

.PARAMETER ProjectName
    The folder name within each architecture to search recursively for appx packages. The appx files
    must also have the ProjectName in their file names.

.PARAMETER OutputFile
    The path to write the generated mapping file.

.EXAMPLE
    Create-AppxBundleMapping -InputPath "C:\drop" -ProjectName "CalculatorApp" -OutputFile "C:\Temp\AppxBundleMapping.txt"
#>
param(
    [Parameter(Mandatory)]
    [string]
    $InputPath,

    [Parameter(Mandatory)]
    [string]
    $ProjectName,

    [Parameter(Mandatory)]
    [string]
    $OutputFile
)

# List all appx packages by architecture
$architectures = @(Get-ChildItem -Path $InputPath -Directory | Foreach-Object Name | Foreach-Object ToLower)
if ($architectures.Count -lt 1)
{
    throw "No architecture-specific folders found in $InputPath"
}

$defaultArchitecture = $architectures[0]
$packages = @{}
foreach ($architecture in $architectures)
{
    $projectPath = [IO.Path]::Combine($InputPath, $architecture, $ProjectName)
    $packages[$architecture] = Get-ChildItem -Path $projectPath -Recurse -Filter *$ProjectName*.appx

    if ($packages[$architecture].Count -lt 1)
    {
        throw "No .appx files found for architecture $architecture in $projectPath"
    }
}

# List appx packages which are common to all architectures
$commonPackages = $packages[$defaultArchitecture]
foreach ($architecture in $architectures)
{
    $commonPackages = $packages[$architecture] | Where {$commonPackages.Name -Contains $_.Name}
}

# List appx packages which are architecture-specific and verify that there is exactly one per
# architecture.
$architectureSpecificPackages = @()
if ($architectures.Count -gt 1)
{
    foreach ($architecture in $architectures)
    {
        $uniquePackages = $packages[$architecture] | Where {$commonPackages.Name -NotContains $_.Name}
        if ($uniquePackages.Count -ne 1)
        {
            throw "Found multiple architecture-specific packages for architecture $($architecture): $($uniquePackages.Name)"
        }
        $architectureSpecificPackages += $uniquePackages[0]
    }
}

# Write the mapping file
Set-Content $OutputFile "[Files]"
foreach ($package in ($architectureSpecificPackages + $commonPackages))
{
    $mapping = "`"$($package.FullName)`" `"$($package.Name)`""
    Write-Host $mapping
    Add-Content $OutputFile $mapping
}