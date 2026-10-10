// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// Linux replacement for TraceLogger (Windows ETW / TraceLogging).
// Upstream telemetry leaves the machine; per SPEC the Linux port collects
// nothing. This class keeps every call site compiling (same singleton shape
// and method names the ViewModels use) with no-op bodies. A file sink can be
// added later behind an explicit opt-in; there is deliberately no network
// sink. Behavior delta is recorded in PORTING-NOTES.md (Task 10).
using Calculator.Linux.Common;

namespace Calculator.Linux.Services;

public enum GraphSettingsType
{
    Grid,
    TrigUnits,
    Theme,
}

public enum GraphButton
{
    StylePicker,
    RemoveFunction,
    ActiveTracingChecked,
    ActiveTracingUnchecked,
    GraphSettings,
    Share,
    ZoomIn,
    ZoomOut,
    GraphView,
}

public enum GraphButtonValue
{
    None,
    AutomaticBestFit,
    ManualAdjustment,
}

public enum LineStyleType
{
    Color,
    Pattern,
}

public sealed class LinuxTraceLogger
{
    private static readonly Lazy<LinuxTraceLogger> s_instance = new(() => new LinuxTraceLogger());
    public static LinuxTraceLogger Instance => s_instance.Value;

    private LinuxTraceLogger()
    {
    }

    public void LogVisualStateChanged(ViewMode mode, string state, bool isAlwaysOnTop)
    {
    }

    public void LogWindowCreated(ViewMode mode, int windowId)
    {
    }

    public void LogModeChange(ViewMode mode)
    {
    }

    public void LogHistoryItemLoad(ViewMode mode, int historyListSize, int loadedIndex)
    {
    }

    public void LogMemoryItemLoad(ViewMode mode, int memoryListSize, int loadedIndex)
    {
    }

    public void LogError(ViewMode mode, string functionName, string errorString)
    {
    }

    public void LogPlatformExceptionInfo(ViewMode mode, string functionName, string message, int hresult)
    {
    }

    public void LogPlatformException(ViewMode mode, string functionName, Exception e) =>
        LogPlatformExceptionInfo(mode, functionName, e.Message, e.HResult);

    public void UpdateButtonUsage(NumbersAndOperatorsEnum button, ViewMode mode)
    {
    }

    public void LogButtonUsage()
    {
    }

    public void LogDateCalculationModeUsed(bool addSubtractMode)
    {
    }

    public void LogConverterInputReceived(ViewMode mode)
    {
    }

    public void LogNavBarOpened()
    {
    }

    public void LogInputPasted(ViewMode mode)
    {
    }

    public void LogShowHideButtonClicked(bool isHideButton)
    {
    }

    public void LogGraphButtonClicked(GraphButton buttonName, GraphButtonValue buttonValue)
    {
    }

    public void LogGraphLineStyleChanged(LineStyleType style)
    {
    }

    public void LogVariableChanged(string inputChangedType, string variableName)
    {
    }

    public void LogVariableSettingsChanged(string setting)
    {
    }

    public void LogGraphSettingsChanged(GraphSettingsType settingType, string settingValue)
    {
    }

    public void LogGraphTheme(string graphTheme)
    {
    }

    public void LogRecallSnapshot(ViewMode mode)
    {
    }

    public void LogRecallRestore(ViewMode mode)
    {
    }

    public void LogRecallError(string message)
    {
    }
}
