// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

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
    static constexpr array<const wchar_t* const, 9> s_programmerType{ L"N/A",     L"QwordType", L"DwordType", L"WordType", L"ByteType",
                                                                      L"HexBase", L"DecBase",   L"OctBase",   L"BinBase" };
    static reader_writer_lock s_traceLoggerLock;

    // Diagnostics events. Uploaded to asimov.
    constexpr auto EVENT_NAME_WINDOW_ON_CREATED = L"WindowCreated";
    constexpr auto EVENT_NAME_BUTTON_USAGE = L"ButtonUsageInSession";
    constexpr auto EVENT_NAME_NAV_BAR_OPENED = L"NavigationViewOpened";
    constexpr auto EVENT_NAME_MODE_CHANGED = L"ModeChanged";
    constexpr auto EVENT_NAME_DATE_CALCULATION_MODE_USED = L"DateCalculationModeUsed";
    constexpr auto EVENT_NAME_HISTORY_ITEM_LOAD = L"HistoryItemLoad";
    constexpr auto EVENT_NAME_MEMORY_ITEM_LOAD = L"MemoryItemLoad";
    constexpr auto EVENT_NAME_VISUAL_STATE_CHANGED = L"VisualStateChanged";
    constexpr auto EVENT_NAME_CONVERTER_INPUT_RECEIVED = L"ConverterInputReceived";
    constexpr auto EVENT_NAME_INPUT_PASTED = L"InputPasted";

    constexpr auto EVENT_NAME_EXCEPTION = L"Exception";

    constexpr auto PDT_PRIVACY_DATA_TAG = L"PartA_PrivTags";
    constexpr auto PDT_PRODUCT_AND_SERVICE_USAGE = 0x0000'0000'0200'0000u;

#ifdef SEND_DIAGNOSTICS
    // c.f. WINEVENT_KEYWORD_RESERVED_63-56 0xFF00000000000000 // Bits 63-56 - channel keywords
    // c.f. WINEVENT_KEYWORD_*              0x00FF000000000000 // Bits 55-48 - system-reserved keywords
    constexpr int64_t MICROSOFT_KEYWORD_LEVEL_1 = 0x0000800000000000; // Bit 47
    constexpr int64_t MICROSOFT_KEYWORD_LEVEL_2 = 0x0000400000000000; // Bit 46
    constexpr int64_t MICROSOFT_KEYWORD_LEVEL_3 = 0x0000200000000000; // Bit 45
#else
    // define all Keyword options as 0 when we do not want to upload app diagnostics
    constexpr int64_t MICROSOFT_KEYWORD_LEVEL_1 = 0;
    constexpr int64_t MICROSOFT_KEYWORD_LEVEL_2 = 0;
    constexpr int64_t MICROSOFT_KEYWORD_LEVEL_3 = 0;
#endif

#pragma region TraceLogger setup and cleanup

    TraceLogger::TraceLogger()
        : g_calculatorProvider(
              L"MicrosoftCalculator",
              LoggingChannelOptions(GUID{ 0x4f50731a, 0x89cf, 0x4782, 0xb3, 0xe0, 0xdc, 0xe8, 0xc9, 0x4, 0x76, 0xba }),
              GUID{ 0x905ca09, 0x610e, 0x401e, 0xb6, 0x50, 0x2f, 0x21, 0x29, 0x80, 0xb9, 0xe0 })
        , // Unique providerID {0905CA09-610E-401E-B650-2F212980B9E0}
        m_appLaunchActivity{ nullptr }
    {
        CoCreateGuid(&sessionGuid);
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
    void TraceLogger::LogLevel1Event(wstring_view eventName, LoggingFields fields) const
    {
        g_calculatorProvider.LogEvent(eventName, fields, LoggingLevel::Verbose, LoggingOptions(MICROSOFT_KEYWORD_LEVEL_1));
    }

    void TraceLogger::LogLevel2Event(wstring_view eventName, LoggingFields fields) const
    {
        g_calculatorProvider.LogEvent(eventName, fields, LoggingLevel::Verbose, LoggingOptions(MICROSOFT_KEYWORD_LEVEL_2));
    }

    void TraceLogger::LogLevel3Event(wstring_view eventName, LoggingFields fields) const
    {
        g_calculatorProvider.LogEvent(eventName, fields, LoggingLevel::Verbose, LoggingOptions(MICROSOFT_KEYWORD_LEVEL_3));
    }

    unique_ptr<TraceActivity> TraceLogger::CreateTraceActivity(wstring_view eventName, LoggingFields fields) const
    {
        return make_unique<TraceActivity>(g_calculatorProvider, eventName, fields);
    }
#pragma endregion

    // return true if windowId is logged once else return false
    bool TraceLogger::IsWindowIdInLog(int windowId)
    {
        // Writer lock for the windowIdLog resource
        reader_writer_lock::scoped_lock lock(s_traceLoggerLock);

        if (find(windowIdLog.begin(), windowIdLog.end(), windowId) == windowIdLog.end())
        {
            return false;
        }

        return true;
    }

    void TraceLogger::LogVisualStateChanged(ViewMode mode, wstring_view state, bool isAlwaysOnTop) const
    {
        if (!GetTraceLoggingProviderEnabled())
        {
            return;
        }

        LoggingFields fields{};
        fields.AddGuid(L"SessionGuid", sessionGuid);
        fields.AddString(L"CalcMode", NavCategory::GetFriendlyName(mode)->Data());
        fields.AddString(L"VisualState", state);
        fields.AddBoolean(L"IsAlwaysOnTop", isAlwaysOnTop);
        fields.AddUInt64(PDT_PRIVACY_DATA_TAG, PDT_PRODUCT_AND_SERVICE_USAGE);
        LogLevel2Event(EVENT_NAME_VISUAL_STATE_CHANGED, fields);
    }

    void TraceLogger::LogWindowCreated(ViewMode mode, int windowId)
    {
        // store windowId in windowIdLog which says we have logged mode for the present windowId.
        if (!IsWindowIdInLog(windowId))
        {
            windowIdLog.push_back(windowId);
        }

        if (!GetTraceLoggingProviderEnabled())
            return;

        LoggingFields fields{};
        fields.AddGuid(L"SessionGuid", sessionGuid);
        fields.AddString(L"CalcMode", NavCategory::GetFriendlyName(mode)->Data());
        fields.AddUInt64(L"NumOfOpenWindows", currentWindowCount);
        fields.AddUInt64(PDT_PRIVACY_DATA_TAG, PDT_PRODUCT_AND_SERVICE_USAGE);
        LogLevel2Event(EVENT_NAME_WINDOW_ON_CREATED, fields);
    }

    void TraceLogger::LogModeChange(ViewMode mode) const
    {
        if (!GetTraceLoggingProviderEnabled())
            return;

        if (NavCategory::IsValidViewMode(mode))
        {
            LoggingFields fields{};
            fields.AddGuid(L"SessionGuid", sessionGuid);
            fields.AddString(L"CalcMode", NavCategory::GetFriendlyName(mode)->Data());
            fields.AddUInt64(PDT_PRIVACY_DATA_TAG, PDT_PRODUCT_AND_SERVICE_USAGE);
            LogLevel2Event(EVENT_NAME_MODE_CHANGED, fields);
        }
    }

    void TraceLogger::LogHistoryItemLoad(ViewMode mode, int historyListSize, int loadedIndex) const
    {
        if (!GetTraceLoggingProviderEnabled())
        {
            return;
        }

        LoggingFields fields{};
        fields.AddGuid(L"SessionGuid", sessionGuid);
        fields.AddString(L"CalcMode", NavCategory::GetFriendlyName(mode)->Data());
        fields.AddInt32(L"HistoryListSize", historyListSize);
        fields.AddInt32(L"HistoryItemIndex", loadedIndex);
        fields.AddUInt64(PDT_PRIVACY_DATA_TAG, PDT_PRODUCT_AND_SERVICE_USAGE);
        LogLevel2Event(EVENT_NAME_HISTORY_ITEM_LOAD, fields);
    }

    void TraceLogger::LogMemoryItemLoad(ViewMode mode, int memoryListSize, int loadedIndex) const
    {
        if (!GetTraceLoggingProviderEnabled())
        {
            return;
        }

        LoggingFields fields{};
        fields.AddGuid(L"SessionGuid", sessionGuid);
        fields.AddString(L"CalcMode", NavCategory::GetFriendlyName(mode)->Data());
        fields.AddInt32(L"MemoryListSize", memoryListSize);
        fields.AddInt32(L"MemoryItemIndex", loadedIndex);
        fields.AddUInt64(PDT_PRIVACY_DATA_TAG, PDT_PRODUCT_AND_SERVICE_USAGE);
        LogLevel2Event(EVENT_NAME_MEMORY_ITEM_LOAD, fields);
    }

    void TraceLogger::LogError(ViewMode mode, wstring_view functionName, wstring_view errorString)
    {
        if (!GetTraceLoggingProviderEnabled())
            return;

        LoggingFields fields{};
        fields.AddGuid(L"SessionGuid", sessionGuid);
        fields.AddString(L"CalcMode", NavCategory::GetFriendlyName(mode)->Data());
        fields.AddString(L"FunctionName", functionName);
        fields.AddString(L"Message", errorString);
        fields.AddUInt64(PDT_PRIVACY_DATA_TAG, PDT_PRODUCT_AND_SERVICE_USAGE);
        LogLevel2Event(EVENT_NAME_EXCEPTION, fields);
    }

    void TraceLogger::LogStandardException(ViewMode mode, wstring_view functionName, const exception& e) const
    {
        if (!GetTraceLoggingProviderEnabled())
            return;

        LoggingFields fields{};
        fields.AddGuid(L"SessionGuid", sessionGuid);
        fields.AddString(L"CalcMode", NavCategory::GetFriendlyName(mode)->Data());
        fields.AddString(L"FunctionName", functionName);
        wstringstream exceptionMessage;
        exceptionMessage << e.what();
        fields.AddString(L"Message", exceptionMessage.str());
        fields.AddUInt64(PDT_PRIVACY_DATA_TAG, PDT_PRODUCT_AND_SERVICE_USAGE);
        LogLevel2Event(EVENT_NAME_EXCEPTION, fields);
    }

    void TraceLogger::LogWinRTException(ViewMode mode, wstring_view functionName, hresult_error const& e) const
    {
        if (!GetTraceLoggingProviderEnabled())
            return;

        LoggingFields fields{};
        fields.AddGuid(L"SessionGuid", sessionGuid);
        fields.AddString(L"CalcMode", NavCategory::GetFriendlyName(mode)->Data());
        fields.AddString(L"FunctionName", functionName);
        fields.AddString(L"Message", e.message());
        fields.AddInt32(L"HRESULT", e.code());
        fields.AddUInt64(PDT_PRIVACY_DATA_TAG, PDT_PRODUCT_AND_SERVICE_USAGE);
        LogLevel2Event(EVENT_NAME_EXCEPTION, fields);
    }

    void TraceLogger::LogPlatformException(ViewMode mode, wstring_view functionName, Platform::Exception ^ e) const
    {
        if (!GetTraceLoggingProviderEnabled())
            return;

        LoggingFields fields{};
        fields.AddGuid(L"SessionGuid", sessionGuid);
        fields.AddString(L"CalcMode", NavCategory::GetFriendlyName(mode)->Data());
        fields.AddString(L"FunctionName", functionName);
        fields.AddString(L"Message", e->Message->Data());
        fields.AddInt32(L"HRESULT", e->HResult);
        fields.AddUInt64(PDT_PRIVACY_DATA_TAG, PDT_PRODUCT_AND_SERVICE_USAGE);
        LogLevel2Event(EVENT_NAME_EXCEPTION, fields);
    }

    void TraceLogger::UpdateButtonUsage(NumbersAndOperatorsEnum button, ViewMode mode)
    {
        // IsProgrammerMode, IsScientificMode, IsStandardMode and None are not actual buttons, so ignore them
        if (button == NumbersAndOperatorsEnum::IsProgrammerMode || button == NumbersAndOperatorsEnum::IsScientificMode
            || button == NumbersAndOperatorsEnum::IsStandardMode || button == NumbersAndOperatorsEnum::None)
        {
            return;
        }

        {
            // Writer lock for the buttonLog resource
            reader_writer_lock::scoped_lock lock(s_traceLoggerLock);

            vector<ButtonLog>::iterator it = std::find_if(
                buttonLog.begin(), buttonLog.end(), [button, mode](const ButtonLog& bLog) -> bool { return bLog.button == button && bLog.mode == mode; });
            if (it != buttonLog.end())
            {
                it->count++;
            }
            else
            {
                buttonLog.push_back(ButtonLog(button, mode));
            }
        }

        // Periodically log the button usage so that we do not lose all button data if the app is foricibly closed or crashes
        if (buttonLog.size() >= 10)
        {
            LogButtonUsage();
        }
    }

    void TraceLogger::UpdateWindowCount(size_t windowCount)
    {
        if (windowCount == 0)
        {
            currentWindowCount--;
            return;
        }
        currentWindowCount = windowCount;
    }

    void TraceLogger::LogButtonUsage()
    {
        if (!GetTraceLoggingProviderEnabled())
            return;

        // Writer lock for the buttonLog resource
        reader_writer_lock::scoped_lock lock(s_traceLoggerLock);

        if (buttonLog.size() == 0)
        {
            return;
        }

        Platform::String ^ buttonUsageString;
        for (size_t i = 0; i < buttonLog.size(); i++)
        {
            buttonUsageString += NavCategory::GetFriendlyName(buttonLog[i].mode);
            buttonUsageString += "|";
            buttonUsageString += buttonLog[i].button.ToString();
            buttonUsageString += "|";
            buttonUsageString += buttonLog[i].count;
            if (i != buttonLog.size() - 1)
            {
                buttonUsageString += ",";
            }
        }

        LoggingFields fields{};
        fields.AddGuid(L"SessionGuid", sessionGuid);
        fields.AddString(L"ButtonUsage", buttonUsageString->Data());
        fields.AddUInt64(PDT_PRIVACY_DATA_TAG, PDT_PRODUCT_AND_SERVICE_USAGE);
        LogLevel2Event(EVENT_NAME_BUTTON_USAGE, fields);

        buttonLog.clear();
    }

    void TraceLogger::LogDateCalculationModeUsed(bool AddSubtractMode)
    {
        const wchar_t* calculationType = AddSubtractMode ? L"AddSubtractMode" : L"DateDifferenceMode";
        LoggingFields fields{};
        fields.AddGuid(L"SessionGuid", sessionGuid);
        fields.AddString(L"CalcMode", NavCategory::GetFriendlyName(ViewMode::Date)->Data());
        fields.AddString(L"CalculationType", calculationType);
        fields.AddUInt64(PDT_PRIVACY_DATA_TAG, PDT_PRODUCT_AND_SERVICE_USAGE);
        LogLevel2Event(EVENT_NAME_DATE_CALCULATION_MODE_USED, fields);
    }

    void TraceLogger::LogConverterInputReceived(ViewMode mode) const
    {
        if (!GetTraceLoggingProviderEnabled())
            return;

        LoggingFields fields{};
        fields.AddGuid(L"SessionGuid", sessionGuid);
        fields.AddString(L"CalcMode", NavCategory::GetFriendlyName(mode)->Data());
        fields.AddUInt64(PDT_PRIVACY_DATA_TAG, PDT_PRODUCT_AND_SERVICE_USAGE);
        LogLevel2Event(EVENT_NAME_CONVERTER_INPUT_RECEIVED, fields);
    }

    void TraceLogger::LogNavBarOpened() const
    {
        if (!GetTraceLoggingProviderEnabled())
            return;

        LoggingFields fields{};
        fields.AddGuid(L"SessionGuid", sessionGuid);
        fields.AddUInt64(PDT_PRIVACY_DATA_TAG, PDT_PRODUCT_AND_SERVICE_USAGE);
        LogLevel2Event(EVENT_NAME_NAV_BAR_OPENED, fields);
    }

    void TraceLogger::LogInputPasted(ViewMode mode) const
    {
        if (!GetTraceLoggingProviderEnabled())
            return;

        LoggingFields fields{};
        fields.AddGuid(L"SessionGuid", sessionGuid);
        fields.AddString(L"Mode", NavCategory::GetFriendlyName(mode)->Data());
        fields.AddUInt64(PDT_PRIVACY_DATA_TAG, PDT_PRODUCT_AND_SERVICE_USAGE);
        LogLevel2Event(EVENT_NAME_INPUT_PASTED, fields);
    }
}
