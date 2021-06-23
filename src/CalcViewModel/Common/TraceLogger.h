// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "NavCategory.h"
#include "CalculatorButtonUser.h"

// A trace logging provider can only be instantiated and registered once per module.
// This class implements a singleton model ensure that only one instance is created.
namespace CalculatorApp::ViewModel::Common
{
    struct ButtonLog
    {
    public:
        int count;
        CalculatorApp::ViewModel::Common::NumbersAndOperatorsEnum button;
        CalculatorApp::ViewModel::Common::ViewMode mode;
        ButtonLog(CalculatorApp::ViewModel::Common::NumbersAndOperatorsEnum btn, CalculatorApp::ViewModel::Common::ViewMode vMode)
        {
            button = btn;
            mode = vMode;
            count = 1;
        }
    };

    public enum class GraphSettingsType
    {
        Grid,
        TrigUnits,
        Theme
    };

    public enum class GraphButton
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
    };

    public enum class GraphButtonValue
    {
        None,
        AutomaticBestFit,
        ManualAdjustment
    };

    public enum class LineStyleType
    {
        Color,
        Pattern
    };

    public ref class TraceLogger sealed
    {
    public:
        static TraceLogger ^ GetInstance();

        void LogModeChange(CalculatorApp::ViewModel::Common::ViewMode mode);
        void LogHistoryItemLoad(CalculatorApp::ViewModel::Common::ViewMode mode, int historyListSize, int loadedIndex);
        void LogMemoryItemLoad(CalculatorApp::ViewModel::Common::ViewMode mode, int memoryListSize, int loadedIndex);
        void UpdateButtonUsage(CalculatorApp::ViewModel::Common::NumbersAndOperatorsEnum button, CalculatorApp::ViewModel::Common::ViewMode mode);
        void LogButtonUsage();
        void LogDateCalculationModeUsed(bool AddSubtractMode);
        void UpdateWindowCount(uint64 windowCount);
        void DecreaseWindowCount();
        bool IsWindowIdInLog(int windowId);
        void LogVisualStateChanged(CalculatorApp::ViewModel::Common::ViewMode mode, Platform::String ^ state, bool isAlwaysOnTop);
        void LogWindowCreated(CalculatorApp::ViewModel::Common::ViewMode mode, int windowId);
        void LogConverterInputReceived(CalculatorApp::ViewModel::Common::ViewMode mode);
        void LogNavBarOpened();
        void LogError(CalculatorApp::ViewModel::Common::ViewMode mode, Platform::String ^ functionName, Platform::String ^ errorString);
        void LogShowHideButtonClicked(bool isHideButton);
        void LogGraphButtonClicked(GraphButton buttonName, GraphButtonValue buttonValue);
        void LogGraphLineStyleChanged(LineStyleType style);
        void LogVariableChanged(Platform::String ^ inputChangedType, Platform::String ^ variableName);
        void LogVariableSettingsChanged(Platform::String ^ setting);
        void LogGraphSettingsChanged(GraphSettingsType settingsType, Platform::String ^ settingValue);
        void LogGraphTheme(Platform::String ^ graphTheme);
        void LogInputPasted(CalculatorApp::ViewModel::Common::ViewMode mode);
        void LogPlatformExceptionInfo(CalculatorApp::ViewModel::Common::ViewMode mode, Platform::String ^ functionName, Platform::String ^ message, int hresult);

        internal:
        void LogPlatformException(CalculatorApp::ViewModel::Common::ViewMode mode, Platform::String ^ functionName, Platform::Exception ^ e);
        void LogStandardException(CalculatorApp::ViewModel::Common::ViewMode mode, std::wstring_view functionName, _In_ const std::exception& e);

    private:
        // Create an instance of TraceLogger
        TraceLogger();

        std::vector<ButtonLog> buttonLog;
        std::vector<int> windowIdLog;
        uint64 currentWindowCount = 0;
    };
}
