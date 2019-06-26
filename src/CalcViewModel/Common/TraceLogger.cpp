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

    // Telemetry events. Uploaded to asimov.
    constexpr auto EVENT_NAME_WINDOW_ON_CREATED = L"WindowCreated";
    constexpr auto EVENT_NAME_BUTTON_USAGE = L"ButtonUsageInSession";
    constexpr auto EVENT_NAME_NAV_BAR_OPENED = L"NavigationViewOpened";
    constexpr auto EVENT_NAME_MODE_CHANGED = L"ModeChanged";
    constexpr auto EVENT_NAME_DATE_CALCULATION_MODE_USED = L"DateCalculationModeUsed";
    constexpr auto EVENT_NAME_HISTORY_ITEM_LOAD = L"HistoryItemLoad";
    constexpr auto EVENT_NAME_MEMORY_ITEM_LOAD = L"MemoryItemLoad";
    constexpr auto EVENT_NAME_VISUAL_STATE_CHANGED = L"VisualStateChanged";

    constexpr auto EVENT_NAME_EXCEPTION = L"Exception";

    constexpr auto PDT_PRIVACY_DATA_TAG = L"PartA_PrivTags";
    constexpr auto PDT_PRODUCT_AND_SERVICE_USAGE = 0x0000'0000'0200'0000u;

#ifdef SEND_TELEMETRY
    // c.f. WINEVENT_KEYWORD_RESERVED_63-56 0xFF00000000000000 // Bits 63-56 - channel keywords
    // c.f. WINEVENT_KEYWORD_*              0x00FF000000000000 // Bits 55-48 - system-reserved keywords
    constexpr int64_t MICROSOFT_KEYWORD_LEVEL_1 = 0x0000800000000000; // Bit 47
    constexpr int64_t MICROSOFT_KEYWORD_LEVEL_2 = 0x0000400000000000; // Bit 46
    constexpr int64_t MICROSOFT_KEYWORD_LEVEL_3 = 0x0000200000000000; // Bit 45
#else
    // define all Keyword options as 0 when we do not want to upload app telemetry
    constexpr int64_t MICROSOFT_KEYWORD_LEVEL_1 = 0;
    constexpr int64_t MICROSOFT_KEYWORD_LEVEL_2 = 0;
    constexpr int64_t MICROSOFT_KEYWORD_LEVEL_3 = 0;
#endif

#pragma region TraceLogger setup and cleanup

    TraceLogger::TraceLogger()
        : g_calculatorProvider(
              L"MicrosoftCalculator",
              LoggingChannelOptions(GUID{ 0x4f50731a, 0x89cf, 0x4782, 0xb3, 0xe0, 0xdc, 0xe8, 0xc9, 0x4, 0x76, 0xba }), // Microsoft Telemetry group
              GUID{ 0x905ca09, 0x610e, 0x401e, 0xb6, 0x50, 0x2f, 0x21, 0x29, 0x80, 0xb9, 0xe0 })
        , // Unique providerID {0905CA09-610E-401E-B650-2F212980B9E0}
        m_appLaunchActivity{ nullptr }
    {
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

    void TraceLogger::LogVisualStateChanged(ViewMode mode, wstring_view state) const
    {
        if (!GetTraceLoggingProviderEnabled())
        {
            return;
        }

        LoggingFields fields{};
        fields.AddString(L"CalcMode", NavCategory::GetFriendlyName(mode)->Data());
        fields.AddString(L"VisualState", state);
        fields.AddUInt64(PDT_PRIVACY_DATA_TAG, PDT_PRODUCT_AND_SERVICE_USAGE);
        LogLevel2Event(EVENT_NAME_VISUAL_STATE_CHANGED, fields);
    }

    void TraceLogger::LogWindowCreated(ViewMode mode) const
    {
        if (!GetTraceLoggingProviderEnabled())
            return;

        LoggingFields fields{};
        fields.AddInt32(L"CalcMode", NavCategory::Serialize(mode));
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
            // cast mode to an int for telemetry
            fields.AddInt32(L"CalcMode", NavCategory::Serialize(mode));
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
        // cast mode to an int for telemetry
        fields.AddInt32(L"CalcMode", NavCategory::Serialize(mode));
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
        // cast mode to an int for telemetry
        fields.AddInt32(L"CalcMode", NavCategory::Serialize(mode));
        fields.AddInt32(L"MemoryListSize", memoryListSize);
        fields.AddInt32(L"MemoryItemIndex", loadedIndex);
        fields.AddUInt64(PDT_PRIVACY_DATA_TAG, PDT_PRODUCT_AND_SERVICE_USAGE);
        LogLevel2Event(EVENT_NAME_MEMORY_ITEM_LOAD, fields);
    }
    void TraceLogger::LogError(ViewMode mode, wstring_view errorString)
    {
        if (!GetTraceLoggingProviderEnabled())
            return;

        LoggingFields fields{};
        fields.AddInt32(L"CalcMode", NavCategory::Serialize(mode));

        fields.AddString(L"ErrorString", errorString);
        fields.AddUInt64(PDT_PRIVACY_DATA_TAG, PDT_PRODUCT_AND_SERVICE_USAGE);
        LogLevel2Event(EVENT_NAME_EXCEPTION, fields);
    }
    void TraceLogger::LogStandardException(ViewMode mode, wstring_view functionName, const exception& e) const
    {
        if (!GetTraceLoggingProviderEnabled())
            return;

        LoggingFields fields{};
        fields.AddInt32(L"CalcMode", NavCategory::Serialize(mode));

        fields.AddString(L"FunctionName", functionName);
        wstringstream exceptionMessage;
        exceptionMessage << e.what();
        fields.AddString(L"ExceptionMessage", exceptionMessage.str());
        fields.AddUInt64(PDT_PRIVACY_DATA_TAG, PDT_PRODUCT_AND_SERVICE_USAGE);
        LogLevel2Event(EVENT_NAME_EXCEPTION, fields);
    }

    void TraceLogger::LogWinRTException(ViewMode mode, wstring_view functionName, hresult_error const& e) const
    {
        if (!GetTraceLoggingProviderEnabled())
            return;

        LoggingFields fields{};
        fields.AddInt32(L"CalcMode", NavCategory::Serialize(mode));
        fields.AddString(L"FunctionName", functionName);
        fields.AddInt32(L"HRESULT", e.code());
        fields.AddString(L"ExceptionMessage", e.message());
        fields.AddUInt64(PDT_PRIVACY_DATA_TAG, PDT_PRODUCT_AND_SERVICE_USAGE);
        LogLevel2Event(EVENT_NAME_EXCEPTION, fields);
    }

    void TraceLogger::LogPlatformException(ViewMode mode, wstring_view functionName, Platform::Exception ^ e) const
    {
        if (!GetTraceLoggingProviderEnabled())
            return;

        LoggingFields fields{};
        fields.AddInt32(L"CalcMode", NavCategory::Serialize(mode));
        fields.AddString(L"FunctionName", functionName);
        fields.AddInt32(L"HRESULT", e->HResult);
        fields.AddString(L"ExceptionMessage", e->Message->Data());
        fields.AddUInt64(PDT_PRIVACY_DATA_TAG, PDT_PRODUCT_AND_SERVICE_USAGE);
        LogLevel2Event(EVENT_NAME_EXCEPTION, fields);
    }

    void TraceLogger::UpdateButtonUsage(int buttonId, ViewMode mode)
    {
        // IsProgrammerMode, IsScientificMode, IsStandardMode and None are not actual buttons, so ignore them
        if (buttonId == (int)NumbersAndOperatorsEnum::IsProgrammerMode || buttonId == (int)NumbersAndOperatorsEnum::IsScientificMode ||
            buttonId == (int)NumbersAndOperatorsEnum::IsStandardMode || buttonId == (int)NumbersAndOperatorsEnum::None)
        {
            return;
        }

        // Writer lock for the static resources
        reader_writer_lock::scoped_lock lock(s_traceLoggerLock);
        vector<ButtonLog>::iterator it = std::find_if(
            buttonLog.begin(), buttonLog.end(), [buttonId, mode](const ButtonLog& bLog) -> bool { return bLog.buttonId == buttonId && bLog.mode == mode; });
        if (it != buttonLog.end())
        {
            it->count++;
        }
        else
        {
            NumbersAndOperatorsEnum button = safe_cast<NumbersAndOperatorsEnum>(buttonId);
            buttonLog.push_back(ButtonLog(buttonId, button.ToString()->Data(), mode));
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

        for (auto i : buttonLog)
        {
            LoggingFields fields{};
            fields.AddInt32(L"CalcMode", NavCategory::Serialize(i.mode));
            fields.AddUInt32(L"ButtonId", i.buttonId);
            fields.AddString(L"ButtonName", i.buttonName.data());
            fields.AddUInt32(L"ViewModeId", NavCategory::Serialize(i.mode));
            fields.AddUInt32(L"UsageCount", i.count);
            fields.AddUInt64(PDT_PRIVACY_DATA_TAG, PDT_PRODUCT_AND_SERVICE_USAGE);
            LogLevel2Event(EVENT_NAME_BUTTON_USAGE, fields);
        }

        buttonLog.clear();
    }

    void TraceLogger::LogDateCalculationModeUsed(bool AddSubtractMode)
    {
        LoggingFields fields{};
        fields.AddBoolean(L"CalculationType", AddSubtractMode);
        // cast mode to an int for telemetry
        fields.AddInt32(L"CalcMode", NavCategory::Serialize(ViewMode::Date));
        fields.AddUInt64(PDT_PRIVACY_DATA_TAG, PDT_PRODUCT_AND_SERVICE_USAGE);
        LogLevel2Event(EVENT_NAME_DATE_CALCULATION_MODE_USED, fields);
    }

    void TraceLogger::LogConverterInputReceived(ViewMode mode) const
    {
        if (!GetTraceLoggingProviderEnabled())
            return;

        LoggingFields fields{};
        fields.AddInt32(L"CalcMode", NavCategory::Serialize(mode));
        fields.AddUInt64(PDT_PRIVACY_DATA_TAG, PDT_PRODUCT_AND_SERVICE_USAGE);
        LogLevel2Event(L"ConverterInputReceived", fields);
    }

    void TraceLogger::LogNavBarOpened() const
    {
        if (!GetTraceLoggingProviderEnabled())
            return;

        LoggingFields fields{};
        fields.AddUInt64(PDT_PRIVACY_DATA_TAG, PDT_PRODUCT_AND_SERVICE_USAGE);
        LogLevel2Event(EVENT_NAME_NAV_BAR_OPENED, fields);
    }
}
