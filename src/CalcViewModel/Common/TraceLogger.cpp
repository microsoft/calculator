// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "pch.h"
#include "TraceLogger.h"
#include "NetworkManager.h"
#include "CalculatorButtonUser.h"

using namespace CalculatorApp;
using namespace CalculatorApp::Common;
using namespace Concurrency;
using namespace std;
using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Diagnostics;
using namespace winrt::Windows::Globalization;
using namespace winrt::Windows::Globalization::DateTimeFormatting;
using namespace winrt::Windows::System::UserProfile;

namespace CalculatorApp
{
    static multimap<int, vector<wstring>> s_memoryMap;

    static constexpr array<const wchar_t * const, 9> s_programmerType{
        L"N/A", L"QwordType", L"DwordType",
        L"WordType", L"ByteType", L"HexBase",
        L"DecBase", L"OctBase", L"BinBase" };
    static reader_writer_lock s_traceLoggerLock;

    // Telemetry events. Uploaded to asimov.
    constexpr auto EVENT_NAME_DEBUG                                                     = L"Debug";
    constexpr auto EVENT_NAME_ERROR                                                     = L"ErrorMessage";
    constexpr auto EVENT_NAME_APP_PRELAUNCHED_BY_SYSTEM                                 = L"AppPrelaunchedBySystem";
    constexpr auto EVENT_NAME_PRELAUNCHED_APP_ACTIVATED_BY_USER                         = L"PrelaunchedAppActivatedByUser";
    constexpr auto EVENT_NAME_APP_LAUNCH_BEGIN                                          = L"AppLaunchBegin";
    constexpr auto EVENT_NAME_APP_LAUNCH_END                                            = L"AppLaunchEnd";
    constexpr auto EVENT_NAME_APP_RESUME_END                                            = L"AppResumeEnd";
    constexpr auto EVENT_NAME_PREVIOUS_STATE_WINDOW_ON_CREATION                         = L"PreviousStateOnWindowCreation";
    constexpr auto EVENT_NAME_SIZE_ON_SUSPENSION                                        = L"CalculatorSizeOnSuspension";
    constexpr auto EVENT_NAME_CALCULATOR_VIEWED_IN_SESSION                              = L"CalculatorViewedInSession";
    constexpr auto EVENT_NAME_DATE_CALCULATOR_VIEWED_IN_SESSION                         = L"DateCalculatorViewedInSession";
    constexpr auto EVENT_NAME_CONVERTER_VIEWED_IN_SESSION                               = L"ConverterViewedInSession";
    constexpr auto EVENT_NAME_MODE_CHANGE_BEGIN                                         = L"ModeChangeBegin";
    constexpr auto EVENT_NAME_MODE_CHANGE_END                                           = L"ModeChangeEnd";
    constexpr auto EVENT_NAME_HISTORY_BODY_OPENED                                       = L"HistoryBodyOpened";
    constexpr auto EVENT_NAME_HISTORY_ITEM_LOAD_BEGIN                                   = L"HistoryItemLoadBegin";
    constexpr auto EVENT_NAME_HISTORY_ITEM_LOAD_END                                     = L"HistoryItemLoadEnd";
    constexpr auto EVENT_NAME_HISTORY_FLYOUT_OPEN_BEGIN                                 = L"HistoryFlyoutOpenBegin";
    constexpr auto EVENT_NAME_HISTORY_FLYOUT_OPEN_END                                   = L"HistoryFlyoutOpenEnd";
    constexpr auto EVENT_NAME_NEW_WINDOW_CREATION_BEGIN                                 = L"NewWindowCreationBegin";
    constexpr auto EVENT_NAME_NEW_WINDOW_CREATION_END                                   = L"NewWindowCreationEnd";
    constexpr auto EVENT_NAME_HISTORY_CLEAR                                             = L"HistoryClearBegin";
    constexpr auto EVENT_NAME_MULTIPLE_MEMORY_USED                                      = L"MultipleMemoryUsed";
    constexpr auto EVENT_NAME_SINGLE_MEMORY_USED                                        = L"SingleMemoryUsed";
    constexpr auto EVENT_NAME_SHARED_MEMORY_USED                                        = L"SharedMemoryUsed";
    constexpr auto EVENT_NAME_MEMORY_BODY_OPENED                                        = L"MemoryBodyOpened";
    constexpr auto EVENT_NAME_MEMORY_FLYOUT_OPEN_BEGIN                                  = L"MemoryFlyoutOpenBegin";
    constexpr auto EVENT_NAME_MEMORY_FLYOUT_OPEN_END                                    = L"MemoryFlyoutOpenEnd";
    constexpr auto EVENT_NAME_MEMORY_CLEAR_ALL                                          = L"MemoryClearAll";
    constexpr auto EVENT_NAME_INVALID_INPUT_PASTED                                      = L"InvalidInputPasted";
    constexpr auto EVENT_NAME_VALID_INPUT_PASTED                                        = L"ValidInputPasted";
    constexpr auto EVENT_NAME_BITFLIP_PANE_CLICKED                                      = L"BitFlipPaneClicked";
    constexpr auto EVENT_NAME_BITFLIP_BUTTONS_USED                                      = L"BitFlipToggleButtonUsed";
    constexpr auto EVENT_NAME_ANGLE_BUTTONS_USED                                        = L"AngleButtonUsedInSession";
    constexpr auto EVENT_NAME_HYP_BUTTON_USED                                           = L"HypButtonUsedInSession";
    constexpr auto EVENT_NAME_FUNCTION_USAGE                                            = L"FunctionUsageInSession";
    constexpr auto EVENT_NAME_BITLENGTH_BUTTON_USED                                     = L"BitLengthButtonUsed";
    constexpr auto EVENT_NAME_RADIX_BUTTON_USED                                         = L"RadixButtonUsed";
    constexpr auto EVENT_NAME_MAX_WINDOW_COUNT                                          = L"MaxWindowCountInSession";
    constexpr auto EVENT_NAME_WINDOW_LAUNCHED_PROTOCOL                                  = L"WindowActivatedThroughProtocol";
    constexpr auto EVENT_NAME_WINDOW_LAUNCHED_TILESEARCH                                = L"WindowLaunchedThroughTile";
    constexpr auto EVENT_NAME_DATE_DIFFERENCE_USED                                      = L"DateDifferenceModeUsed";
    constexpr auto EVENT_NAME_DATE_ADD_SUBTRACT_USED                                    = L"DateAddSubtractModeUsed";
    constexpr auto EVENT_NAME_DATE_DIFFERENCE_FOUND                                     = L"DateDifferenceFound";
    constexpr auto EVENT_NAME_HIDE_IF_SHOWN                                             = L"HideIfShown";
    constexpr auto EVENT_NAME_ABOUT_FLYOUT_OPENED                                       = L"AboutFlyoutOpened";
    constexpr auto EVENT_NAME_NAV_BAR_OPENED                                            = L"NavBarOpened";
    constexpr auto EVENT_NAME_CORE_WINDOW_WAS_NULL                                      = L"CoreWindowWasNull";

    constexpr auto EVENT_NAME_EXCEPTION                                                 = L"Exception";

#ifdef SEND_TELEMETRY
    // c.f. WINEVENT_KEYWORD_RESERVED_63-56 0xFF00000000000000 // Bits 63-56 - channel keywords
    // c.f. WINEVENT_KEYWORD_*              0x00FF000000000000 // Bits 55-48 - system-reserved keywords
    constexpr int64_t MICROSOFT_KEYWORD_CRITICAL_DATA = 0x0000800000000000; // Bit 47
    constexpr int64_t MICROSOFT_KEYWORD_MEASURES = 0x0000400000000000; // Bit 46
    constexpr int64_t MICROSOFT_KEYWORD_TELEMETRY = 0x0000200000000000; // Bit 45
    constexpr int64_t MICROSOFT_KEYWORD_RESERVED_44 = 0x0000100000000000; // Bit 44 (reserved for future assignment)
#else
    // define all Keyword options as 0 when we do not want to upload app telemetry
    constexpr int64_t MICROSOFT_KEYWORD_CRITICAL_DATA = 0;
    constexpr int64_t MICROSOFT_KEYWORD_MEASURES = 0;
    constexpr int64_t MICROSOFT_KEYWORD_TELEMETRY = 0;
    constexpr int64_t MICROSOFT_KEYWORD_RESERVED_44 = 0;
#endif

#pragma region TraceLogger setup and cleanup

    TraceLogger::TraceLogger() :
        g_calculatorProvider(
            L"MicrosoftCalculator",
            LoggingChannelOptions(GUID{ 0x4f50731a, 0x89cf, 0x4782, 0xb3, 0xe0, 0xdc, 0xe8, 0xc9, 0x4, 0x76, 0xba }), // Microsoft Telemetry group
            GUID{ 0x905ca09, 0x610e, 0x401e, 0xb6, 0x50, 0x2f, 0x21, 0x29, 0x80, 0xb9, 0xe0 }), //Unique providerID {0905CA09-610E-401E-B650-2F212980B9E0}
        m_appLaunchActivity{ nullptr }
    {
        // initialize the function array
        InitFunctionLogArray();
    }

    TraceLogger::~TraceLogger()
    {
    }

    TraceLogger& TraceLogger::GetInstance()
    {
        static TraceLogger s_selfInstance;
        return s_selfInstance;
    }

    bool TraceLogger::GetTraceLoggingProviderEnabled() const
    {
        return g_calculatorProvider.Enabled();
    }

#pragma region Tracing methods
    void TraceLogger::LogTelemetryEvent(wstring_view eventName, LoggingFields fields) const
    {
        g_calculatorProvider.LogEvent(eventName, fields, LoggingLevel::Verbose, LoggingOptions(MICROSOFT_KEYWORD_TELEMETRY));
    }

    void TraceLogger::LogMeasureEvent(wstring_view eventName, LoggingFields fields) const
    {
        g_calculatorProvider.LogEvent(eventName, fields, LoggingLevel::Verbose, LoggingOptions(MICROSOFT_KEYWORD_MEASURES));
    }

    void TraceLogger::LogCriticalDataEvent(wstring_view eventName, LoggingFields fields) const
    {
        g_calculatorProvider.LogEvent(eventName, fields, LoggingLevel::Verbose, LoggingOptions(MICROSOFT_KEYWORD_CRITICAL_DATA));
    }

    void TraceLogger::LogPerformanceEvent(wstring_view eventName, LoggingFields fields) const
    {
        g_calculatorProvider.LogEvent(eventName, fields, LoggingLevel::Verbose, LoggingOptions(WINEVENT_KEYWORD_RESPONSE_TIME));
    }

    void TraceLogger::LogInfoEvent(wstring_view eventName, LoggingFields fields) const
    {
        g_calculatorProvider.LogEvent(eventName, fields, LoggingLevel::Information);
    }

    unique_ptr<TraceActivity> TraceLogger::CreateTraceActivity(wstring_view eventName, LoggingFields fields) const
    {
        return make_unique<TraceActivity>(g_calculatorProvider, eventName, fields);
    }
#pragma endregion

    void TraceLogger::InsertIntoMemoryMap(int windowId, bool isStandard, bool isScientific, bool isProgrammer)
    {
        // Writer lock for the static resources
        reader_writer_lock::scoped_lock lock(s_traceLoggerLock);

        auto iterMap = s_memoryMap.find(windowId);
        if (iterMap == s_memoryMap.end())
        {
            s_memoryMap.insert(std::make_pair(windowId, vector<wstring>()));
            iterMap = s_memoryMap.find(windowId);
        }

        if (isScientific)
        {
            iterMap->second.insert(iterMap->second.begin(), L"Scientific");
        }
        else if (isProgrammer)
        {
            iterMap->second.insert(iterMap->second.begin(), L"Programmer");
        }
        else if (isStandard)
        {
            iterMap->second.insert(iterMap->second.begin(), L"Standard");
        }
    }

    void TraceLogger::UpdateMemoryMap(int windowId, int memoryPosition, bool isStandard, bool isScientific, bool isProgrammer)
    {
        // Writer lock for the static resources
        reader_writer_lock::scoped_lock lock(s_traceLoggerLock);

        auto iterMap = s_memoryMap.find(windowId);
        assert(iterMap != s_memoryMap.end());
        assert(iterMap->second.size() >= (unsigned int)memoryPosition);

        if (isScientific)
        {
            iterMap->second[memoryPosition] = L"Scientific";
        }
        else if (isProgrammer)
        {
            iterMap->second[memoryPosition] = L"Programmer";
        }
        else if (isStandard)
        {
            iterMap->second[memoryPosition] = L"Standard";
        }
    }

    void TraceLogger::DeleteFromMemoryMap(int windowId, int memoryPosition)
    {
        // Writer lock for the static resources
        reader_writer_lock::scoped_lock lock(s_traceLoggerLock);
        auto iterMap = s_memoryMap.find(windowId);
        assert(iterMap != s_memoryMap.end());

        iterMap->second.erase(iterMap->second.begin() + memoryPosition);
    }

    // return true if windowId is logged once else return false
    bool TraceLogger::UpdateWindowIdLog(int windowId)
    {
        // Writer lock for the static resources
        reader_writer_lock::scoped_lock lock(s_traceLoggerLock);

        if (windowIdLog.find(windowId) == windowIdLog.end())
        {
            return false;
        }
        if (windowIdLog[windowId] == false)
        {
            windowIdLog[windowId] = true;
            return true;
        }
        else
        {
            return false;
        }
    }

    // call comes here at the time of ApplicationViewModel initialisation
    void TraceLogger::LogCalculatorModeViewed(ViewMode mode, int windowId)
    {
        // Writer lock for the static resources
        reader_writer_lock::scoped_lock lock(s_traceLoggerLock);

        // store windowId in windowIdLog which says we have logged mode for the present windowId.
        if (windowIdLog.find(windowId) == windowIdLog.end())
        {
            windowIdLog.insert(pair<int, bool>(windowId, false));
        }
        // if the event is not logged already for the present mode 
        if (currentMode != mode)
        {
            currentMode = mode;

            LoggingFields fields{};
            fields.AddString(L"CalculatorMode", NavCategory::GetFriendlyName(mode)->Data());
            fields.AddUInt32(L"WindowId", windowId);
            LogTelemetryEvent(EVENT_NAME_CALCULATOR_VIEWED_IN_SESSION, fields);
        }
    }

    // call comes here at the time of ApplicationViewModel initialization
    void TraceLogger::LogDateCalculatorModeViewed(ViewMode mode, int windowId)
    {
        // Writer lock for the static resources
        reader_writer_lock::scoped_lock lock(s_traceLoggerLock);

        // store windowId in windowIdLog which says we have logged mode for the present windowId.
        if (windowIdLog.find(windowId) == windowIdLog.end())
        {
            windowIdLog.insert(pair<int, bool>(windowId, false));
        }
        // if the event is not logged already for the present mode 
        if (currentMode != mode)
        {
            currentMode = mode;

            LoggingFields fields{};
            fields.AddString(L"DateCalculatorMode", NavCategory::GetFriendlyName(mode)->Data());
            fields.AddUInt32(L"WindowId", windowId);
            LogTelemetryEvent(EVENT_NAME_DATE_CALCULATOR_VIEWED_IN_SESSION, fields);
        }
    }

    // call comes here at the time of ApplicationViewModel initialization
    void TraceLogger::LogConverterModeViewed(ViewMode mode, int windowId)
    {
        // Writer lock for the static resources
        reader_writer_lock::scoped_lock lock(s_traceLoggerLock);

        if (windowIdLog.find(windowId) == windowIdLog.end())
        {
            windowIdLog.insert(pair<int, bool>(windowId, false));
        }
        // if the event is not logged already for the present mode 
        if (currentMode != mode)
        {
            currentMode = mode;

            LoggingFields fields{};
            fields.AddString(L"ConverterMode", NavCategory::GetFriendlyName(mode)->Data());
            fields.AddUInt32(L"WindowId", windowId);
            LogTelemetryEvent(EVENT_NAME_CONVERTER_VIEWED_IN_SESSION, fields);
        }
    }

    void TraceLogger::LogSharedMemoryUsed(wstring_view fromMode, wstring_view toMode, unsigned int memorySize) const
    {
        if (!GetTraceLoggingProviderEnabled()) return;

        LoggingFields fields{};
        fields.AddString(L"FromMode", fromMode);
        fields.AddString(L"ToMode", toMode);
        fields.AddUInt32(L"MemoryListSize", memorySize);
        LogTelemetryEvent(EVENT_NAME_SHARED_MEMORY_USED, fields);
    }

    void TraceLogger::LogBitFlipPaneClicked() const
    {
        if (!GetTraceLoggingProviderEnabled()) return;

        LoggingFields fields{};
        LogTelemetryEvent(EVENT_NAME_BITFLIP_PANE_CLICKED, fields);
    }

    void TraceLogger::LogBitFlipUsed() const
    {
        if (!GetTraceLoggingProviderEnabled()) return;

        LoggingFields fields{};
        LogTelemetryEvent(EVENT_NAME_BITFLIP_BUTTONS_USED, fields);
    }

    void TraceLogger::LogAppLaunchStart()
    {
        // Writer lock for the static resources
        reader_writer_lock::scoped_lock lock(s_traceLoggerLock);

        if (!isAppLaunchBeginLogged)
        {
            m_appLaunchActivity = g_calculatorProvider.StartActivity(
                EVENT_NAME_APP_LAUNCH_BEGIN,
                nullptr,
                LoggingLevel::Verbose,
                LoggingOptions(MICROSOFT_KEYWORD_TELEMETRY));

            isAppLaunchBeginLogged = true;
        }
    }

    void TraceLogger::LogAppLaunchComplete(/*Windows::ApplicationModel::Activation::ActivationKind activationKind, Windows::ApplicationModel::Activation::ApplicationExecutionState executionState*/)
    {
        // Writer lock for the static resources
        reader_writer_lock::scoped_lock lock(s_traceLoggerLock);

        if ((m_appLaunchActivity != nullptr) && (!isAppLaunchEndLogged))
        {
            m_appLaunchActivity.StopActivity(EVENT_NAME_APP_LAUNCH_END);

            isAppLaunchEndLogged = true;
        }

        m_appLaunchActivity = nullptr;
    }

    void TraceLogger::LogAppResumeComplete()
    {
        if (m_appLaunchActivity != nullptr)
        {
            m_appLaunchActivity.StopActivity(EVENT_NAME_APP_RESUME_END);
        }

        m_appLaunchActivity = nullptr;
    }

    void TraceLogger::LogDebug(wstring_view debugData)
    {
        if (!GetTraceLoggingProviderEnabled()) return;

        LoggingFields fields{};
        fields.AddString(L"DebugData", debugData);
        LogTelemetryEvent(EVENT_NAME_DEBUG, fields);
    }

    void TraceLogger::LogOnAppLaunch(wstring_view previousExecutionState) const
    {
        if (!GetTraceLoggingProviderEnabled()) return;

        LoggingFields fields{};
        fields.AddString(L"PreviousExecutionState", previousExecutionState);
        LogTelemetryEvent(EVENT_NAME_PREVIOUS_STATE_WINDOW_ON_CREATION, fields);
    }

    void TraceLogger::LogAboutFlyoutOpened() const
    {
        if (!GetTraceLoggingProviderEnabled()) return;

        LoggingFields fields{};
        LogTelemetryEvent(EVENT_NAME_ABOUT_FLYOUT_OPENED, fields);
    }

    void TraceLogger::LogNavBarOpened() const
    {
        if (!GetTraceLoggingProviderEnabled()) return;

        LoggingFields fields{};
        LogTelemetryEvent(EVENT_NAME_NAV_BAR_OPENED, fields);
    }

    void TraceLogger::LogClearHistory() const
    {
        if (!GetTraceLoggingProviderEnabled()) return;

        LoggingFields fields{};
        LogTelemetryEvent(EVENT_NAME_HISTORY_CLEAR, fields);
    }

    void TraceLogger::LogHistoryFlyoutOpenBegin(unsigned int historyItemCount) const
    {
        if (!GetTraceLoggingProviderEnabled()) return;

        // we want to record the event only when history item count is atleast 20
        if (historyItemCount >= 20)
        {
            LoggingFields fields{};
            fields.AddUInt32(L"HistoryItemCount", historyItemCount);
            LogTelemetryEvent(EVENT_NAME_HISTORY_FLYOUT_OPEN_BEGIN, fields);
        }
    }

    void TraceLogger::LogHistoryFlyoutOpenEnd(int historyItemCount) const
    {
        if (!GetTraceLoggingProviderEnabled()) return;

        if (historyItemCount >= 20)
        {
            LoggingFields fields{};
            fields.AddUInt32(L"HistoryItemCount", historyItemCount);
            LogTelemetryEvent(EVENT_NAME_HISTORY_FLYOUT_OPEN_END, fields);
        }
    }

    void TraceLogger::LogHistoryBodyOpened() const
    {
        if (!GetTraceLoggingProviderEnabled()) return;

        LoggingFields fields{};
        LogTelemetryEvent(EVENT_NAME_HISTORY_BODY_OPENED, fields);
    }

    void TraceLogger::LogMemoryFlyoutOpenBegin(unsigned int memoryItemCount) const
    {
        if (!GetTraceLoggingProviderEnabled()) return;

        // we want to record the event only when memory item count is atleast 4
        if (memoryItemCount >= 4)
        {
            LoggingFields fields{};
            fields.AddUInt32(L"MemoryItemCount", memoryItemCount);
            LogTelemetryEvent(EVENT_NAME_MEMORY_FLYOUT_OPEN_BEGIN, fields);
        }
    }

    void TraceLogger::LogMemoryFlyoutOpenEnd(unsigned int memoryItemCount) const
    {
        if (!GetTraceLoggingProviderEnabled()) return;

        if (memoryItemCount >= 4)
        {
            LoggingFields fields{};
            fields.AddUInt32(L"MemoryItemCount", memoryItemCount);
            LogTelemetryEvent(EVENT_NAME_MEMORY_FLYOUT_OPEN_END, fields);
        }
    }

    void TraceLogger::LogMemoryBodyOpened() const
    {
        if (!GetTraceLoggingProviderEnabled()) return;

        LoggingFields fields{};
        LogTelemetryEvent(EVENT_NAME_MEMORY_BODY_OPENED, fields);
    }

    //If calculator is launched in any mode other than standard then this call will come which is not intended. But there is no way to avoid it.
    //So don't use this function to analyze the count of mode change in session instead use CalculatorViewedInSession and ConverterViewedInSession to do that.
    //Use of this function is to analyze perf of mode change.
    void TraceLogger::LogModeChangeBegin(ViewMode fromMode, ViewMode toMode, int windowId)
    {
        if (!GetTraceLoggingProviderEnabled()) return;

        if (NavCategory::IsValidViewMode(fromMode) && NavCategory::IsValidViewMode(toMode))
        {
            LoggingFields fields{};
            fields.AddString(L"FromMode", NavCategory::GetFriendlyName(fromMode)->Data());
            fields.AddString(L"ToMode", NavCategory::GetFriendlyName(toMode)->Data());
            fields.AddInt32(L"WindowId", windowId);
            LogMeasureEvent(EVENT_NAME_MODE_CHANGE_BEGIN, fields);
        }
    }

    //comment: same as LogModeChangeBegin
    void TraceLogger::LogModeChangeEnd(ViewMode toMode, int windowId) const
    {
        if (!GetTraceLoggingProviderEnabled()) return;

        if (NavCategory::IsValidViewMode(toMode))
        {
            LoggingFields fields{};
            fields.AddString(L"ToMode", NavCategory::GetFriendlyName(toMode)->Data());
            fields.AddInt32(L"WindowId", windowId);
            LogMeasureEvent(EVENT_NAME_MODE_CHANGE_END, fields);
        }
    }

    void TraceLogger::LogHistoryItemLoadBegin() const
    {
        if (!GetTraceLoggingProviderEnabled()) return;

        LoggingFields fields{};
        LogTelemetryEvent(EVENT_NAME_HISTORY_ITEM_LOAD_BEGIN, fields);
    }

    void TraceLogger::LogHistoryItemLoadEnd(unsigned int historyTokenCount) const
    {
        if (!GetTraceLoggingProviderEnabled()) return;

        LoggingFields fields{};
        fields.AddUInt32(L"HistoryTokenCount", historyTokenCount);
        LogTelemetryEvent(EVENT_NAME_HISTORY_ITEM_LOAD_END, fields);
    }

    void TraceLogger::LogNewWindowCreationBegin(int windowId)
    {
        if (!GetTraceLoggingProviderEnabled()) return;

        LoggingFields fields{};
        fields.AddUInt32(L"WindowId", windowId);
        LogTelemetryEvent(EVENT_NAME_NEW_WINDOW_CREATION_BEGIN, fields);
    }

    void TraceLogger::LogNewWindowCreationEnd(int windowId)
    {
        if (!GetTraceLoggingProviderEnabled()) return;

        LoggingFields fields{};
        fields.AddUInt32(L"WindowId", windowId);
        LogTelemetryEvent(EVENT_NAME_NEW_WINDOW_CREATION_END, fields);
    }

    void TraceLogger::LogError(wstring_view errorString)
    {
        if (!GetTraceLoggingProviderEnabled()) return;

        LoggingFields fields{};
        fields.AddString(L"ErrorString", errorString);
        LogTelemetryEvent(EVENT_NAME_ERROR, fields);
    }

    void TraceLogger::LogPrelaunchedAppActivatedByUser()
    {
        if (!GetTraceLoggingProviderEnabled()) return;

        LoggingFields fields{};
        LogTelemetryEvent(EVENT_NAME_PRELAUNCHED_APP_ACTIVATED_BY_USER, fields);
    }

    void TraceLogger::LogAppPrelaunchedBySystem()
    {
        if (!GetTraceLoggingProviderEnabled()) return;

        LoggingFields fields{};
        LogTelemetryEvent(EVENT_NAME_APP_PRELAUNCHED_BY_SYSTEM, fields);
    }

    void TraceLogger::LogMemoryClearAll(int windowId)
    {
        // Writer lock for the static resources
        reader_writer_lock::scoped_lock lock(s_traceLoggerLock);
        auto iterMap = s_memoryMap.find(windowId);
        
        LoggingFields fields{};
        LogTelemetryEvent(EVENT_NAME_MEMORY_CLEAR_ALL, fields);

        if (iterMap != s_memoryMap.end())
        {
            iterMap->second.clear();
        }
    }

    void TraceLogger::LogMemoryUsed(int windowId, unsigned int slotPosition, bool isStandard, bool isScientific, bool isProgrammer, unsigned int memorySize) const
    {
        if (!GetTraceLoggingProviderEnabled()) return;

        // Reader lock for the static resources
        reader_writer_lock::scoped_lock_read lock(s_traceLoggerLock);
        auto iterMap = s_memoryMap.find(windowId);

        if (slotPosition == 0)
        {
            LogSingleMemoryUsed(memorySize);
        }
        else
        {
            LogMultipleMemoryUsed(slotPosition, memorySize);
        }

        if (iterMap != s_memoryMap.end())
        {
            // if current mode is not equal to mode of memoryMap[slotPosition] then LogSharedMemoryUsed
            if (isStandard && (iterMap->second[slotPosition] != L"Standard"))
            {
                LogSharedMemoryUsed(iterMap->second[slotPosition], L"Standard", memorySize);
            }
            else if (isScientific && (iterMap->second[slotPosition] != L"Scientific"))
            {
                LogSharedMemoryUsed(iterMap->second[slotPosition], L"Scientific", memorySize);
            }
            else if (isProgrammer && (iterMap->second[slotPosition] != L"Programmer"))
            {
                LogSharedMemoryUsed(iterMap->second[slotPosition], L"Programmer", memorySize);
            }
        }
    }

    void TraceLogger::LogMultipleMemoryUsed(unsigned int slotPosition, unsigned int memorySize) const
    {
        if (!GetTraceLoggingProviderEnabled()) return;

        LoggingFields fields{};
        fields.AddUInt32(L"MemoryIndex", slotPosition);
        fields.AddUInt32(L"MemoryListSize", memorySize);
        LogTelemetryEvent(EVENT_NAME_MULTIPLE_MEMORY_USED, fields);
    }

    void TraceLogger::LogSingleMemoryUsed(unsigned int memorySize) const
    {
        if (!GetTraceLoggingProviderEnabled()) return;

        LoggingFields fields{};
        fields.AddUInt32(L"MemoryListSize", memorySize);
        LogTelemetryEvent(EVENT_NAME_SINGLE_MEMORY_USED, fields);
    }

    void TraceLogger::LogInvalidInputPasted(wstring_view reason, wstring_view pastedExpression, ViewMode mode, int programmerNumberBase, int bitLengthType)
    {
        if (!GetTraceLoggingProviderEnabled()) return;

        LoggingFields fields{};
        fields.AddString(L"Mode", NavCategory::GetFriendlyName(mode)->Data());
        fields.AddString(L"Reason", reason);
        fields.AddString(L"PastedExpression", pastedExpression);
        fields.AddString(L"ProgrammerNumberBase", GetProgrammerType(programmerNumberBase).c_str());
        fields.AddString(L"BitLengthType", GetProgrammerType(bitLengthType).c_str());
        LogTelemetryEvent(EVENT_NAME_INVALID_INPUT_PASTED, fields);
    }

    void TraceLogger::LogValidInputPasted(ViewMode mode) const
    {
        if (!GetTraceLoggingProviderEnabled()) return;

        LoggingFields fields{};
        fields.AddString(L"Mode", NavCategory::GetFriendlyName(mode)->Data());
        LogTelemetryEvent(EVENT_NAME_VALID_INPUT_PASTED, fields);
    }

    void TraceLogger::LogStandardException(wstring_view functionName, const exception& e) const
    {
        if (!GetTraceLoggingProviderEnabled()) return;

        LoggingFields fields{};
        fields.AddString(L"FunctionName", functionName);
        wstringstream exceptionMessage;
        exceptionMessage << e.what();
        fields.AddString(L"ExceptionMessage", exceptionMessage.str());
        LogMeasureEvent(EVENT_NAME_EXCEPTION, fields);
    }

    void TraceLogger::LogWinRTException(wstring_view functionName, hresult_error const& e) const
    {
        if (!GetTraceLoggingProviderEnabled()) return;

        LoggingFields fields{};
        fields.AddString(L"FunctionName", functionName);
        fields.AddInt32(L"HRESULT", e.code());
        fields.AddString(L"ExceptionMessage", e.message());
        LogMeasureEvent(EVENT_NAME_EXCEPTION, fields);
    }

    void TraceLogger::LogPlatformException(wstring_view functionName, Platform::Exception^ e) const
    {
        if (!GetTraceLoggingProviderEnabled()) return;

        LoggingFields fields{};
        fields.AddString(L"FunctionName", functionName);
        fields.AddInt32(L"HRESULT", e->HResult);
        fields.AddString(L"ExceptionMessage", e->Message->Data());
        LogMeasureEvent(EVENT_NAME_EXCEPTION, fields);
    }

    void TraceLogger::UpdateFunctionUsage(int funcIndex)
    {
        // Writer lock for the static resources
        reader_writer_lock::scoped_lock lock(s_traceLoggerLock);

        if (GetIndex(funcIndex))
        {
            // funcIndex is passed by reference and will be having the returned index
            funcLog[funcIndex].count++;
        }
    }

    void TraceLogger::InitFunctionLogArray()
    {
        int i = -1;
        for (int funcIndex = 0; funcIndex != maxFunctionSize; funcIndex++)
        {
            FunctionLogEnum func = safe_cast<FunctionLogEnum>(funcIndex);
            wstring functionName = func.ToString()->Data();
            if (functionName.compare(L"CalculatorApp.FunctionLogEnum") != 0)
            {
                findIndex[funcIndex] = ++i;
                funcLog.push_back(FuncLog(functionName));
            }
        }
        // update the functionCount with total function count which we are tracking through tracelog.
        functionCount = i;
    }

    wstring TraceLogger::GetProgrammerType(int index)
    {
        if (index >= 0)
        {
            return s_programmerType[index];
        }
        //return "N/A"
        return s_programmerType[0];
    }

    bool TraceLogger::GetIndex(int &index)
    {
        if (findIndex[index] > 0)
        {
            index = findIndex[index];
            return true;
        }
        return false;
    }

    void TraceLogger::UpdateWindowCount(size_t windowCount)
    {
        maxWindowCount = (maxWindowCount > windowCount) ? maxWindowCount : windowCount;
        windowLaunchCount++;
    }

    void TraceLogger::LogMaxWindowCount()
    {
        if (!GetTraceLoggingProviderEnabled()) return;

        LoggingFields fields{};
        fields.AddUInt32(L"WindowCount", (unsigned int)maxWindowCount);
        LogTelemetryEvent(EVENT_NAME_MAX_WINDOW_COUNT, fields);
    }

    void TraceLogger::LogWindowActivated() const
    {
        if (!GetTraceLoggingProviderEnabled()) return;

        LoggingFields fields{};
        LogTelemetryEvent(EVENT_NAME_WINDOW_LAUNCHED_PROTOCOL, fields);
    }

    void TraceLogger::LogWindowLaunched() const
    {
        if (!GetTraceLoggingProviderEnabled()) return;

        LoggingFields fields{};
        LogTelemetryEvent(EVENT_NAME_WINDOW_LAUNCHED_TILESEARCH, fields);
    }

    void TraceLogger::LogBitLengthButtonUsed(int windowId)
    {
        if (bitLengthButtonUsage.find(windowId) == bitLengthButtonUsage.end())
        {
            bitLengthButtonUsage.insert(pair<int, int>(windowId, 1));
        }
        else
        {
            bitLengthButtonUsage[windowId]++;
        }
        if ((bitLengthButtonUsage[windowId] == 5) && !bitLengthButtonLoggedInSession)
        {
            LoggingFields fields{};
            fields.AddUInt32(L"WindowId", windowId);
            LogTelemetryEvent(EVENT_NAME_BITLENGTH_BUTTON_USED, fields);

            bitLengthButtonLoggedInSession = true;
        }
    }

    void TraceLogger::LogRadixButtonUsed(int windowId)
    {
        if (radixButtonUsage.find(windowId) == radixButtonUsage.end())
        {
            radixButtonUsage.insert(pair<int, int>(windowId, 1));
        }
        else
        {
            radixButtonUsage[windowId]++;
        }
        if ((radixButtonUsage[windowId] == 2) && !radixButtonLoggedInSession)
        {
            LoggingFields fields{};
            fields.AddUInt32(L"WindowId", windowId);
            LogTelemetryEvent(EVENT_NAME_RADIX_BUTTON_USED, fields);

            radixButtonLoggedInSession = true;
        }
    }

    void TraceLogger::LogAngleButtonUsed(int windowId)
    {
        if (angleButtonUsage.find(windowId) == angleButtonUsage.end())
        {
            angleButtonUsage.insert(pair<int, int>(windowId, 1));
        }
        else
        {
            angleButtonUsage[windowId]++;
        }
        if ((angleButtonUsage[windowId] == 2) && !angleButtonLoggedInSession)
        {
            LoggingFields fields{};
            fields.AddUInt32(L"WindowId", windowId);
            LogTelemetryEvent(EVENT_NAME_ANGLE_BUTTONS_USED, fields);

            angleButtonLoggedInSession = true;
        }
    }

    void TraceLogger::LogFunctionUsage(int windowId)
    {
        if (!GetTraceLoggingProviderEnabled()) return;

        for (int i = 0; i < functionCount; i++)
        {
            // log only those functions which are used
            if (funcLog[i].count > 0)
            {
                LoggingFields fields{};
                fields.AddString(L"FunctionName", funcLog[i].funcName.data());
                fields.AddUInt32(L"UsageCount", funcLog[i].count);
                fields.AddUInt32(L"WindowId", windowId);
                LogTelemetryEvent(EVENT_NAME_FUNCTION_USAGE, fields);
            }
        }
    }

    void TraceLogger::LogHypButtonUsed(int windowId)
    {
        if (!isHypButtonLogged)
        {
            LoggingFields fields{};
            fields.AddUInt32(L"WindowId", windowId);
            LogTelemetryEvent(EVENT_NAME_HYP_BUTTON_USED, fields);

            isHypButtonLogged = true;
        }
    }

    void TraceLogger::LogDateDifferenceModeUsed(int windowId)
    {
        if (!m_dateDiffUsageLoggedInSession)
        {
            LoggingFields fields{};
            fields.AddUInt32(L"WindowId", windowId);
            LogTelemetryEvent(EVENT_NAME_DATE_DIFFERENCE_USED, fields);

            m_dateDiffUsageLoggedInSession = true;
        }
    }

    void TraceLogger::LogDateAddSubtractModeUsed(int windowId, bool isAddMode)
    {
        auto usageMap = isAddMode ? &m_dateAddModeUsage : &m_dateSubtractModeUsage;
        auto isLoggedInSession = isAddMode ? &m_dateAddUsageLoggedInSession : &m_dateSubtractUsageLoggedInSession;

        if (usageMap->find(windowId) == usageMap->end())
        {
            usageMap->insert(pair<int, int>(windowId, 1));
        }
        else
        {
            (*usageMap)[windowId]++;
        }

        // Ignore first 3 calls during the initialization of the combo box selected items for Add mode
        int firstChangeEventCount = isAddMode ? 4 : 1;

        if (((*usageMap)[windowId] == firstChangeEventCount)
            && (!(*isLoggedInSession)))
        {
            LoggingFields fields{};
            fields.AddString(L"AddSubtractMode", isAddMode ? L"Add" : L"Subtract");
            LogTelemetryEvent(EVENT_NAME_DATE_ADD_SUBTRACT_USED, fields);

            *isLoggedInSession = true;
        }
    }

    void TraceLogger::LogDateClippedTimeDifferenceFound(Calendar const& today, DateTime const& clippedTime) const
    {
        if (!GetTraceLoggingProviderEnabled()) return;

        auto calendarSystem = today.GetCalendarSystem();
        auto clock = today.GetClock();
        DateTimeFormatter dtFormatter{
            L"longdate shorttime",
            { L"en-US" },
            GlobalizationPreferences::HomeGeographicRegion(),
            calendarSystem,
            clock };

        LoggingFields fields{};
        fields.AddString(L"ResolvedCalendarLanguage", today.ResolvedLanguage());
        fields.AddString(L"Timezone", today.GetTimeZone());
        fields.AddString(L"CalendarSystem", calendarSystem);
        fields.AddString(L"Clock", clock);
        fields.AddBoolean(L"IsDaylightSavingTime", today.IsDaylightSavingTime());
        fields.AddString(L"TodayDate", dtFormatter.Format(today.GetDateTime()));
        fields.AddString(L"ClippedDate", dtFormatter.Format(clippedTime));
        LogTelemetryEvent(EVENT_NAME_DATE_DIFFERENCE_FOUND, fields);
    }

    void TraceLogger::LogUserRequestedRefreshFailed() const
    {
        if (!GetTraceLoggingProviderEnabled()) return;

        LoggingFields fields{};
        LogTelemetryEvent(L"UserRequestedRefreshFailed", fields);
    }

    void TraceLogger::LogConversionResult(wstring_view fromValue, wstring_view fromUnit, wstring_view toValue, wstring_view toUnit) const
    {
        if (!GetTraceLoggingProviderEnabled()) return;

        wstring behaviorString{};
        NetworkAccessBehavior behavior = NetworkManager::GetNetworkAccessBehavior();
        switch (behavior)
        {
        case NetworkAccessBehavior::Offline:
            behaviorString = L"Offline";
            break;

        case NetworkAccessBehavior::OptIn:
            behaviorString = L"Metered";
            break;

        case NetworkAccessBehavior::Normal:
        default:
            behaviorString = L"Online";
            break;
        }

        LoggingFields fields{};
        fields.AddString(L"NetworkAccess", behaviorString);
        fields.AddString(L"FromValue", fromValue);
        fields.AddString(L"FromUnit", fromUnit);
        fields.AddString(L"ToValue", toValue);
        fields.AddString(L"ToUnit", toUnit);
        LogTelemetryEvent(L"CurrencyConverterInputReceived", fields);
    }

    void TraceLogger::LogViewClosingTelemetry(int windowId)
    {
        LogFunctionUsage(windowId);
        LogMaxWindowCount();
    }

    void TraceLogger::LogCoreWindowWasNull() const
    {
        if (!GetTraceLoggingProviderEnabled()) return;

        LoggingFields fields{};
        LogTelemetryEvent(EVENT_NAME_CORE_WINDOW_WAS_NULL, fields);
    }
}

