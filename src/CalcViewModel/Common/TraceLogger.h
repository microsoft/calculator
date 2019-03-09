// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "CalcManager/Command.h"
#include "TraceActivity.h"
#include "NavCategory.h"

static const int maxFunctionSize = (int)CalculationManager::Command::CommandBINEDITEND;

// A trace logging provider can only be instantiated and registered once per module. 
// This class implements a singleton model ensure that only one instance is created. 
namespace CalculatorApp
{
    struct FuncLog
    {
    public:
        int count;
        std::wstring funcName;
        FuncLog() { count = 0; }
        FuncLog(std::wstring fName)
        {
            funcName = fName;
            count = 0;
        }
    };

    class TraceLogger
    {
    public:
        TraceLogger(_In_ TraceLogger const&) = delete;
        TraceLogger const & operator= (_In_ TraceLogger const&) = delete;
        ~TraceLogger();
        static TraceLogger& GetInstance();
        bool GetTraceLoggingProviderEnabled() const;

        void LogAppLaunchStart();
        void LogAppLaunchComplete();
        void LogAppResumeComplete();
        void LogOnAppLaunch(std::wstring_view previousExecutionState) const;
        void LogMemoryClearAll(int);
        void LogBitFlipPaneClicked() const;
        void LogBitFlipUsed() const;
        void LogHistoryBodyOpened() const;
        void LogHistoryItemLoadBegin() const;
        void LogHistoryItemLoadEnd(unsigned int) const;
        void LogHistoryFlyoutOpenBegin(unsigned int) const;
        void LogHistoryFlyoutOpenEnd(int) const;
        void LogCalculatorModeViewed(CalculatorApp::Common::ViewMode, int);
        void LogDateCalculatorModeViewed(CalculatorApp::Common::ViewMode, int);
        void LogConverterModeViewed(CalculatorApp::Common::ViewMode, int);
        void LogModeChangeBegin(CalculatorApp::Common::ViewMode, CalculatorApp::Common::ViewMode, int);
        void LogModeChangeEnd(CalculatorApp::Common::ViewMode, int) const;
        void LogClearHistory() const;
        void InsertIntoMemoryMap(int, bool, bool, bool);
        void UpdateMemoryMap(int, int, bool, bool, bool);
        void DeleteFromMemoryMap(int, int);
        void LogMemoryUsed(int, unsigned int, bool, bool, bool, unsigned int) const;
        void LogMultipleMemoryUsed(unsigned int, unsigned int) const;
        void LogSingleMemoryUsed(unsigned int) const;
        void LogSharedMemoryUsed(std::wstring_view, std::wstring_view, unsigned int) const;
        void LogMemoryBodyOpened() const;
        void LogMemoryFlyoutOpenBegin(unsigned int) const;
        void LogDebug(std::wstring_view debugData);
        void LogMemoryFlyoutOpenEnd(unsigned int) const;
        void LogInvalidInputPasted(std::wstring_view reason, std::wstring_view pastedExpression, CalculatorApp::Common::ViewMode mode, int ProgrammerNumberBase, int bitLengthType);
        void LogValidInputPasted(CalculatorApp::Common::ViewMode mode) const;
        void UpdateFunctionUsage(int func);
        void LogFunctionUsage(int);
        void InitFunctionLogArray();
        void LogBitLengthButtonUsed(int windowId);
        void LogRadixButtonUsed(int windowId);
        void LogAngleButtonUsed(int windowId);
        void LogHypButtonUsed(int windowId);
        void LogNewWindowCreationBegin(int windowId);
        void LogNewWindowCreationEnd(int windowId);
        void LogError(std::wstring_view errorString);
        void LogPrelaunchedAppActivatedByUser();
        void LogAppPrelaunchedBySystem();
        void UpdateWindowCount(size_t windowCount);
        bool UpdateWindowIdLog(int windowId);
        void LogMaxWindowCount();
        void LogWindowActivated() const;
        void LogWindowLaunched() const;
        void LogUserRequestedRefreshFailed() const;
        void LogConversionResult(std::wstring_view fromValue, std::wstring_view fromUnit, std::wstring_view toValue, std::wstring_view toUnit) const;
        void LogAboutFlyoutOpened() const;
        void LogNavBarOpened() const;
        void LogViewClosingTelemetry(int);
        void LogCoreWindowWasNull() const;

        // Trace methods for Date Calculator usage
        void LogDateDifferenceModeUsed(int windowId);
        void LogDateAddSubtractModeUsed(int windowId, bool isAddMode);
        void LogDateClippedTimeDifferenceFound(winrt::Windows::Globalization::Calendar const& today, winrt::Windows::Foundation::DateTime const& clippedTime) const;

        void LogStandardException(std::wstring_view functionName, _In_ const std::exception& e) const;
        void LogWinRTException(std::wstring_view functionName, _In_ winrt::hresult_error const& e) const;
        void LogPlatformException(std::wstring_view functionName, _In_ Platform::Exception^ e) const;

    private:
        // Create an instance of TraceLogger
        TraceLogger();

        // Any new Log method should
            // a) decide the level of logging. This will help us in limiting recording of events only up to a certain level. See this link for guidance https://msdn.microsoft.com/en-us/library/windows/desktop/aa363742(v=vs.85).aspx
            // We're using Verbose level for events that are called frequently and needed only for debugging or capturing perf for specific scenarios 
            // b) should decide whether or not to log to  telemetry and pass TraceLoggingKeyword(MICROSOFT_KEYWORD_TELEMETRY) accordingly
            // c) Should accept a variable number of additional data arguments if needed
        void LogTelemetryEvent(std::wstring_view eventName, winrt::Windows::Foundation::Diagnostics::LoggingFields fields) const;
        void LogMeasureEvent(std::wstring_view eventName, winrt::Windows::Foundation::Diagnostics::LoggingFields fields) const;
        void LogCriticalDataEvent(std::wstring_view eventName, winrt::Windows::Foundation::Diagnostics::LoggingFields fields) const;
        void LogPerformanceEvent(std::wstring_view eventName, winrt::Windows::Foundation::Diagnostics::LoggingFields fields) const;
        void LogInfoEvent(std::wstring_view eventName, winrt::Windows::Foundation::Diagnostics::LoggingFields fields) const;

        std::unique_ptr<TraceActivity> CreateTraceActivity(std::wstring_view activityName, winrt::Windows::Foundation::Diagnostics::LoggingFields fields) const;

        winrt::Windows::Foundation::Diagnostics::LoggingChannel g_calculatorProvider;

        bool isSizeChangeLogged = false;
        bool isHideIfShownLogged = false;
        bool isSizeChangedFirstTime = true; // to track the first size changed event which is fired on the launch of app
        bool isAutoConversionBeginLoggedInSession = false;
        bool isAutoConversionEndLoggedInSession = false;
        bool angleButtonLoggedInSession = false;
        bool radixButtonLoggedInSession = false;
        bool bitLengthButtonLoggedInSession = false;
        GUID sessionGuid;
        CalculatorApp::Common::ViewMode currentMode = CalculatorApp::Common::ViewMode::None;
        std::vector<FuncLog> funcLog;
        int functionCount = 0;
        bool isHypButtonLogged = false;
        bool isAngleButtonInitialized = false;
        unsigned int findIndex[maxFunctionSize] = { 0 };
        bool GetIndex(int &index);
        std::wstring GetProgrammerType(int index);
        size_t maxWindowCount = 0;
        bool isAppLaunchBeginLogged = false;
        bool isAppLaunchEndLogged = false;
        std::map<int, int> bitLengthButtonUsage;
        std::map<int, int> angleButtonUsage;
        std::map<int, int> radixButtonUsage;
        std::map<int, bool> windowIdLog;

        // Private variables for Date Calculator usage
        bool m_dateDiffUsageLoggedInSession = false;
        bool m_dateAddUsageLoggedInSession = false;
        bool m_dateSubtractUsageLoggedInSession = false;
        std::map<int, int> m_dateAddModeUsage;
        std::map<int, int> m_dateSubtractModeUsage;

        size_t windowLaunchCount = 0;

        winrt::Windows::Foundation::Diagnostics::LoggingActivity m_appLaunchActivity;
    };
}
