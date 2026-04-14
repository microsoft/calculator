// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// A trace logging provider can only be instantiated and registered once per module.
// This class implements a singleton model to ensure that only one instance is created.

using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Windows.Foundation.Diagnostics;

namespace CalculatorApp.ViewModel.Common
{
    public enum GraphSettingsType
    {
        Grid,
        TrigUnits,
        Theme
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
        GraphView
    }

    public enum GraphButtonValue
    {
        None,
        AutomaticBestFit,
        ManualAdjustment
    }

    public enum LineStyleType
    {
        Color,
        Pattern
    }

    internal struct ButtonLog
    {
        public int Count;
        public NumbersAndOperatorsEnum Button;
        public ViewMode Mode;

        public ButtonLog(NumbersAndOperatorsEnum button, ViewMode mode)
        {
            Button = button;
            Mode = mode;
            Count = 1;
        }
    }

    public sealed class TraceLogger
    {
        private static readonly Lazy<TraceLogger> s_instance = new Lazy<TraceLogger>(() => new TraceLogger());

        private const string CalcMode = "CalcMode";
        private const string GraphingMode = "Graphing";

        private const string EventNameWindowOnCreated = "WindowCreated";
        private const string EventNameButtonUsage = "ButtonUsageInSession";
        private const string EventNameNavBarOpened = "NavigationViewOpened";
        private const string EventNameModeChanged = "ModeChanged";
        private const string EventNameDateCalculationModeUsed = "DateCalculationModeUsed";
        private const string EventNameHistoryItemLoad = "HistoryItemLoad";
        private const string EventNameMemoryItemLoad = "MemoryItemLoad";
        private const string EventNameVisualStateChanged = "VisualStateChanged";
        private const string EventNameConverterInputReceived = "ConverterInputReceived";
        private const string EventNameInputPasted = "InputPasted";
        private const string EventNameShowHideButtonClicked = "ShowHideButtonClicked";
        private const string EventNameGraphButtonClicked = "GraphButtonClicked";
        private const string EventNameGraphLineStyleChanged = "GraphLineStyleChanged";
        private const string EventNameVariableChanged = "VariableChanged";
        private const string EventNameVariableSettingChanged = "VariableSettingChanged";
        private const string EventNameGraphSettingsChanged = "GraphSettingsChanged";
        private const string EventNameGraphTheme = "GraphTheme";
        private const string EventNameRecallSnapshot = "RecallSnapshot";
        private const string EventNameRecallRestore = "RecallRestore";
        private const string EventNameException = "Exception";

        private readonly List<ButtonLog> _buttonLog = new List<ButtonLog>();
        private readonly List<int> _windowIdLog = new List<int>();
        private readonly object _lock = new object();
        private ulong _currentWindowCount;

        private TraceLogger()
        {
        }

        public static TraceLogger GetInstance() => s_instance.Value;

        public bool IsWindowIdInLog(int windowId)
        {
            lock (_lock)
            {
                return _windowIdLog.Contains(windowId);
            }
        }

        public void LogVisualStateChanged(ViewMode mode, string state, bool isAlwaysOnTop)
        {
            var fields = new LoggingFields();
            fields.AddString(CalcMode, NavCategoryStates.GetFriendlyName(mode));
            fields.AddString("VisualState", state);
            fields.AddBoolean("IsAlwaysOnTop", isAlwaysOnTop);
            LogLevel2Event(EventNameVisualStateChanged, fields);
        }

        public void LogWindowCreated(ViewMode mode, int windowId)
        {
            if (!IsWindowIdInLog(windowId))
            {
                lock (_lock)
                {
                    _windowIdLog.Add(windowId);
                }
            }

            var fields = new LoggingFields();
            fields.AddString(CalcMode, NavCategoryStates.GetFriendlyName(mode));
            fields.AddUInt64("NumOfOpenWindows", _currentWindowCount);
            LogLevel2Event(EventNameWindowOnCreated, fields);
        }

        public void LogModeChange(ViewMode mode)
        {
            if (NavCategoryStates.IsValidViewMode(mode))
            {
                var fields = new LoggingFields();
                fields.AddString(CalcMode, NavCategoryStates.GetFriendlyName(mode));
                LogLevel2Event(EventNameModeChanged, fields);
            }
        }

        public void LogHistoryItemLoad(ViewMode mode, int historyListSize, int loadedIndex)
        {
            var fields = new LoggingFields();
            fields.AddString(CalcMode, NavCategoryStates.GetFriendlyName(mode));
            fields.AddInt32("HistoryListSize", historyListSize);
            fields.AddInt32("HistoryItemIndex", loadedIndex);
            LogLevel2Event(EventNameHistoryItemLoad, fields);
        }

        public void LogMemoryItemLoad(ViewMode mode, int memoryListSize, int loadedIndex)
        {
            var fields = new LoggingFields();
            fields.AddString(CalcMode, NavCategoryStates.GetFriendlyName(mode));
            fields.AddInt32("MemoryListSize", memoryListSize);
            fields.AddInt32("MemoryItemIndex", loadedIndex);
            LogLevel2Event(EventNameMemoryItemLoad, fields);
        }

        public void LogError(ViewMode mode, string functionName, string errorString)
        {
            var fields = new LoggingFields();
            fields.AddString(CalcMode, NavCategoryStates.GetFriendlyName(mode));
            fields.AddString("FunctionName", functionName);
            fields.AddString("Message", errorString);
            LogLevel2Event(EventNameException, fields);
        }

        public void LogPlatformExceptionInfo(ViewMode mode, string functionName, string message, int hresult)
        {
            var fields = new LoggingFields();
            fields.AddString(CalcMode, NavCategoryStates.GetFriendlyName(mode));
            fields.AddString("FunctionName", functionName);
            fields.AddString("Message", message);
            fields.AddInt32("HRESULT", hresult);
            LogLevel2Event(EventNameException, fields);
        }

        internal void LogPlatformException(ViewMode mode, string functionName, Exception e)
        {
            LogPlatformExceptionInfo(mode, functionName, e.Message, e.HResult);
        }

        internal void LogStandardException(ViewMode mode, string functionName, Exception e)
        {
            var fields = new LoggingFields();
            fields.AddString(CalcMode, NavCategoryStates.GetFriendlyName(mode));
            fields.AddString("FunctionName", functionName);
            fields.AddString("Message", e.Message);
            LogLevel2Event(EventNameException, fields);
        }

        public void UpdateButtonUsage(NumbersAndOperatorsEnum button, ViewMode mode)
        {
            // IsProgrammerMode, IsScientificMode, IsStandardMode and None are not actual buttons, so ignore them
            if (button == NumbersAndOperatorsEnum.IsProgrammerMode
                || button == NumbersAndOperatorsEnum.IsScientificMode
                || button == NumbersAndOperatorsEnum.IsStandardMode
                || button == NumbersAndOperatorsEnum.None)
            {
                return;
            }

            lock (_lock)
            {
                int existingIndex = _buttonLog.FindIndex(b => b.Button == button && b.Mode == mode);
                if (existingIndex >= 0)
                {
                    var entry = _buttonLog[existingIndex];
                    entry.Count++;
                    _buttonLog[existingIndex] = entry;
                }
                else
                {
                    _buttonLog.Add(new ButtonLog(button, mode));
                }
            }

            // Periodically log the button usage so that we do not lose all button data if the app is forcibly closed or crashes
            if (_buttonLog.Count >= 10)
            {
                LogButtonUsage();
            }
        }

        public void LogButtonUsage()
        {
            lock (_lock)
            {
                if (_buttonLog.Count == 0)
                {
                    return;
                }

                var sb = new StringBuilder();
                for (int i = 0; i < _buttonLog.Count; i++)
                {
                    sb.Append(NavCategoryStates.GetFriendlyName(_buttonLog[i].Mode));
                    sb.Append('|');
                    sb.Append(_buttonLog[i].Button.ToString());
                    sb.Append('|');
                    sb.Append(_buttonLog[i].Count);
                    if (i != _buttonLog.Count - 1)
                    {
                        sb.Append(',');
                    }
                }

                var fields = new LoggingFields();
                fields.AddString("ButtonUsage", sb.ToString());
                LogLevel2Event(EventNameButtonUsage, fields);

                _buttonLog.Clear();
            }
        }

        public void UpdateWindowCount(ulong windowCount)
        {
            if (windowCount == 0)
            {
                _currentWindowCount--;
                return;
            }
            _currentWindowCount = windowCount;
        }

        public void DecreaseWindowCount()
        {
            _currentWindowCount = 0;
        }

        public void LogDateCalculationModeUsed(bool addSubtractMode)
        {
            string calculationType = addSubtractMode ? "AddSubtractMode" : "DateDifferenceMode";
            var fields = new LoggingFields();
            fields.AddString(CalcMode, NavCategoryStates.GetFriendlyName(ViewMode.Date));
            fields.AddString("CalculationType", calculationType);
            LogLevel2Event(EventNameDateCalculationModeUsed, fields);
        }

        public void LogConverterInputReceived(ViewMode mode)
        {
            var fields = new LoggingFields();
            fields.AddString(CalcMode, NavCategoryStates.GetFriendlyName(mode));
            LogLevel2Event(EventNameConverterInputReceived, fields);
        }

        public void LogNavBarOpened()
        {
            var fields = new LoggingFields();
            LogLevel2Event(EventNameNavBarOpened, fields);
        }

        public void LogInputPasted(ViewMode mode)
        {
            var fields = new LoggingFields();
            fields.AddString(CalcMode, NavCategoryStates.GetFriendlyName(mode));
            LogLevel2Event(EventNameInputPasted, fields);
        }

        public void LogShowHideButtonClicked(bool isHideButton)
        {
            var fields = new LoggingFields();
            fields.AddString(CalcMode, GraphingMode);
            fields.AddBoolean("IsHideButton", isHideButton);
            LogLevel2Event(EventNameShowHideButtonClicked, fields);
        }

        public void LogGraphButtonClicked(GraphButton buttonName, GraphButtonValue buttonValue)
        {
            var fields = new LoggingFields();
            fields.AddString(CalcMode, GraphingMode);
            fields.AddInt16("ButtonName", (short)buttonName);
            fields.AddInt16("ButtonValue", (short)buttonValue);
            LogLevel2Event(EventNameGraphButtonClicked, fields);
        }

        public void LogGraphLineStyleChanged(LineStyleType style)
        {
            var fields = new LoggingFields();
            fields.AddString(CalcMode, GraphingMode);
            fields.AddInt16("StyleType", (short)style);
            LogLevel2Event(EventNameGraphLineStyleChanged, fields);
        }

        public void LogVariableChanged(string inputChangedType, string variableName)
        {
            var fields = new LoggingFields();
            fields.AddString(CalcMode, GraphingMode);
            fields.AddString("InputChangedType", inputChangedType);
            fields.AddString("VariableName", variableName);
            LogLevel2Event(EventNameVariableChanged, fields);
        }

        public void LogVariableSettingsChanged(string setting)
        {
            var fields = new LoggingFields();
            fields.AddString(CalcMode, GraphingMode);
            fields.AddString("SettingChanged", setting);
            LogLevel2Event(EventNameVariableSettingChanged, fields);
        }

        public void LogGraphSettingsChanged(GraphSettingsType settingType, string settingValue)
        {
            var fields = new LoggingFields();
            fields.AddString(CalcMode, GraphingMode);
            fields.AddInt16("SettingType", (short)settingType);
            fields.AddString("SettingValue", settingValue);
            LogLevel2Event(EventNameGraphSettingsChanged, fields);
        }

        public void LogGraphTheme(string graphTheme)
        {
            var fields = new LoggingFields();
            fields.AddString(CalcMode, GraphingMode);
            fields.AddString("GraphTheme", graphTheme);
            LogLevel2Event(EventNameGraphTheme, fields);
        }

        public void LogRecallSnapshot(ViewMode mode)
        {
            var fields = new LoggingFields();
            fields.AddString(CalcMode, NavCategoryStates.GetFriendlyName(mode));
            LogLevel2Event(EventNameRecallSnapshot, fields);
        }

        public void LogRecallRestore(ViewMode mode)
        {
            var fields = new LoggingFields();
            fields.AddString(CalcMode, NavCategoryStates.GetFriendlyName(mode));
            LogLevel2Event(EventNameRecallRestore, fields);
        }

        public void LogRecallError(string message)
        {
            var fields = new LoggingFields();
            fields.AddString("FunctionName", "Recall");
            fields.AddString("Message", message);
            LogLevel2Event(EventNameException, fields);
        }

        private void LogLevel2Event(string eventName, LoggingFields fields)
        {
            // Use TraceLoggingCommon if available, otherwise use LoggingChannel
            try
            {
                TraceLoggingCommon.GetInstance()?.LogLevel2Event(eventName, fields);
            }
            catch
            {
                // Silently ignore trace logging failures
            }
        }
    }

    /// <summary>
    /// Wrapper for TraceLogging infrastructure. In the managed port, this uses the Windows.Foundation.Diagnostics
    /// logging channel.
    /// </summary>
    internal sealed class TraceLoggingCommon
    {
        private static TraceLoggingCommon s_instance;
        private readonly LoggingChannel _channel;

        private TraceLoggingCommon()
        {
            _channel = new LoggingChannel("CalculatorApp", null, new Guid("1d9dda64-be84-57c4-8154-e01299f79f4a"));
        }

        public static TraceLoggingCommon GetInstance()
        {
            if (s_instance == null)
            {
                s_instance = new TraceLoggingCommon();
            }
            return s_instance;
        }

        public void LogLevel2Event(string eventName, LoggingFields fields)
        {
            _channel.LogEvent(eventName, fields, LoggingLevel.Verbose);
        }
    }
}
