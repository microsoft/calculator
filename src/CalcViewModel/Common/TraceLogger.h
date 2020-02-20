// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "TraceActivity.h"
#include "NavCategory.h"
#include "CalculatorButtonUser.h"

// A trace logging provider can only be instantiated and registered once per module.
// This class implements a singleton model ensure that only one instance is created.
namespace CalculatorApp
{
    struct ButtonLog
    {
    public:
        int count;
        CalculatorApp::NumbersAndOperatorsEnum button;
        CalculatorApp::Common::ViewMode mode;
        ButtonLog(CalculatorApp::NumbersAndOperatorsEnum btn, CalculatorApp::Common::ViewMode vMode)
        {
            button = btn;
            mode = vMode;
            count = 1;
        }
    };

    public enum class GraphSettingsType
    {
        Grid,
        TrigUnits
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
        ZoomReset
    };

    public enum class VisibilityButtonState
    {
        Show,
        Hide
    };

    public enum class LineStyleType
    {
        Color
    };

public ref class TraceLogger sealed
    {
    public:
        static TraceLogger ^ GetInstance();
        bool GetTraceLoggingProviderEnabled();
        void LogModeChange(CalculatorApp::Common::ViewMode mode);
        void LogHistoryItemLoad(CalculatorApp::Common::ViewMode mode, int historyListSize, int loadedIndex);
        void LogMemoryItemLoad(CalculatorApp::Common::ViewMode mode, int memoryListSize, int loadedIndex);
        void UpdateButtonUsage(CalculatorApp::NumbersAndOperatorsEnum button, CalculatorApp::Common::ViewMode mode);
        void LogButtonUsage();
        void LogDateCalculationModeUsed(bool AddSubtractMode);
        void UpdateWindowCount(uint64 windowCount);
        void DecreaseWindowCount();
        bool IsWindowIdInLog(int windowId);
        void LogVisualStateChanged(CalculatorApp::Common::ViewMode mode, Platform::String ^ state, bool isAlwaysOnTop);
        void LogWindowCreated(CalculatorApp::Common::ViewMode mode, int windowId);
        void LogConverterInputReceived(CalculatorApp::Common::ViewMode mode);
        void LogNavBarOpened();
        void LogError(CalculatorApp::Common::ViewMode mode, Platform::String ^ functionName, Platform::String ^ errorString);
        void LogShowHideButtonClicked(VisibilityButtonState buttonState);
        void LogGraphButtonClicked(GraphButton buttonName);
        void LogGraphLineStyleChanged(LineStyleType style);
        void LogVariableChanged(Platform::String ^ inputChangedType);
        void LogVariableSettingsChanged(Platform::String ^ setting);
        void LogGraphSettingsChanged(GraphSettingsType settingsType);
        internal : void LogStandardException(CalculatorApp::Common::ViewMode mode, std::wstring_view functionName, _In_ const std::exception& e);
        void LogWinRTException(CalculatorApp::Common::ViewMode mode, std::wstring_view functionName, _In_ winrt::hresult_error const& e);
        void LogPlatformException(CalculatorApp::Common::ViewMode mode, std::wstring_view functionName, _In_ Platform::Exception ^ e);
        void LogInputPasted(CalculatorApp::Common::ViewMode mode);

    private:
        // Create an instance of TraceLogger
        TraceLogger();

        // As mentioned in Microsoft's Privacy Statement(https://privacy.microsoft.com/en-US/privacystatement#maindiagnosticsmodule),
        // sampling is involved in Microsoft's diagnostic data collection process.
        // These keywords provide additional input into how frequently an event might be sampled.
        // The lower the level of the keyword, the higher the possibility that the corresponding event may be sampled.
        void LogLevel1Event(std::wstring_view eventName, winrt::Windows::Foundation::Diagnostics::LoggingFields fields);
        void LogLevel2Event(std::wstring_view eventName, winrt::Windows::Foundation::Diagnostics::LoggingFields fields);
        void LogLevel3Event(std::wstring_view eventName, winrt::Windows::Foundation::Diagnostics::LoggingFields fields);

        std::unique_ptr<TraceActivity> CreateTraceActivity(std::wstring_view activityName, winrt::Windows::Foundation::Diagnostics::LoggingFields fields);

        winrt::Windows::Foundation::Diagnostics::LoggingChannel g_calculatorProvider;

        std::vector<ButtonLog> buttonLog;
        std::vector<int> windowIdLog;

        GUID sessionGuid;
        uint64 currentWindowCount = 0;

        winrt::Windows::Foundation::Diagnostics::LoggingActivity m_appLaunchActivity;
    };
}
