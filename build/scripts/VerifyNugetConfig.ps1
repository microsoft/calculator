# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT License.

<#
.SYNOPSIS
  Verify the specified nuget.config. Throw to fail the pipeline if the config is ill-formed.
.PARAMETER FilePath
  The path pointing to the nuget.config file to be verified by this script.
.EXAMPLE
  VerifyNugetConfig -FilePath .\nuget.config
#>

param([Parameter(Mandatory)][string]$FilePath)

$doc = [XML](Get-Content $FilePath)
$sources = (Select-Xml $doc -XPath "//configuration/packageSources").Node
$count = 0
foreach($src in $sources.ChildNodes){
  if ($src.Name -eq "add"){
    ++$count
  }
  elseif ($src.Name -eq "clear"){
    $count = 0
  }
}
if ($count -gt 1){
  throw "Adding multiple package sources is not allowed in nuget.config."
}
