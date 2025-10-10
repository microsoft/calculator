# Copyright (c) Microsoft Corporation. All rights reserved.
# Licensed under the MIT License.

<#
.SYNOPSIS
    Disables animations on the system. Equivalent to turning off the "Animation effects" setting in the Windows Settings app.
#>

Add-Type -AssemblyName System.Runtime.WindowsRuntime
$asTaskGeneric = ([System.WindowsRuntimeSystemExtensions].GetMethods() | ? { $_.Name -eq 'AsTask' -and $_.GetParameters().Count -eq 1 -and $_.GetParameters()[0].ParameterType.Name -eq 'IAsyncOperation`1' })[0]

Function WaitForAsyncAction($WinRtTask, $ResultType) {
    $asTask = $asTaskGeneric.MakeGenericMethod($ResultType)
    $task = $asTask.Invoke($null, @($WinRtTask))
    $task.GetAwaiter().GetResult()
}

[Windows.UI.ViewManagement.Core.UISettingsController,Windows.UI.ViewManagement.Core,ContentType=WindowsRuntime] | Out-Null

$controller = WaitForAsyncAction ([Windows.UI.ViewManagement.Core.UISettingsController]::RequestDefaultAsync())  ([Windows.UI.ViewManagement.Core.UISettingsController])

$controller.SetAnimationsEnabled($false)
