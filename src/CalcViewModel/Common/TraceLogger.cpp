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
using namespace Platform;
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
    constexpr auto EVENT_NAME_SHOW_HIDE_BUTTON_CLICKED = L"ShowHideButtonClicked";
    constexpr auto EVENT_NAME_GRAPH_BUTTON_CLICKED = L"GraphButtonClicked";
    constexpr auto EVENT_NAME_GRAPH_LINE_STYLE_CHANGED = L"GraphLineStyleChanged";
    constexpr auto EVENT_NAME_VARIABLE_CHANGED = L"VariableChanged";
    constexpr auto EVENT_NAME_VARIABLE_SETTING_CHANGED = L"VariableSettingChanged";
    constexpr auto EVENT_NAME_GRAPH_SETTINGS_CHANGED = L"GraphSettingsChanged";

    constexpr auto EVENT_NAME_EXCEPTION = L"Exception";

    constexpr auto CALC_MODE = L"CalcMode";
    constexpr auto GRPAHING_MODE = L"Graphing";

#pragma region TraceLogger setup and cleanup

    TraceLogger::TraceLogger()
    {
        
    }

    TraceLogger ^ TraceLogger::GetInstance()
    {
        static TraceLogger ^ s_selfInstance = ref new TraceLogger();
        return s_selfInstance;
    }

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

    void TraceLogger::LogVisualStateChanged(ViewMode mode, String ^ state, bool isAlwaysOnTop)
    {
        LoggingFields fields;
       
        fields.AddString(CALC_MODE, NavCategory::GetFriendlyName(mode)->Data());
        fields.AddString(L"VisualState", state->Data());
        fields.AddBoolean(L"IsAlwaysOnTop", isAlwaysOnTop);
        TraceLogging::GetInstance()->LogLevel2Event(ref new String(EVENT_NAME_VISUAL_STATE_CHANGED), fields);
    }

    void TraceLogger::LogWindowCreated(ViewMode mode, int windowId)
    {
        // store windowId in windowIdLog which says we have logged mode for the present windowId.
        if (!IsWindowIdInLog(windowId))
        {
            windowIdLog.push_back(windowId);
        }

        LoggingFields fields{};
        fields.AddString(CALC_MODE, NavCategory::GetFriendlyName(mode)->Data());
        fields.AddUInt64(L"NumOfOpenWindows", currentWindowCount);
        TraceLogging::GetInstance()->LogLevel2Event(EVENT_NAME_WINDOW_ON_CREATED, fields);
    }

    void TraceLogger::LogModeChange(ViewMode mode)
    {
        if (NavCategory::IsValidViewMode(mode))
        {
            LoggingFields fields{};;
            fields.AddString(CALC_MODE, NavCategory::GetFriendlyName(mode)->Data());
            TraceLogging::GetInstance()->LogLevel2Event(EVENT_NAME_MODE_CHANGED, fields);
        }
    }

    void TraceLogger::LogHistoryItemLoad(ViewMode mode, int historyListSize, int loadedIndex)
    {
        LoggingFields fields{};
        fields.AddString(CALC_MODE, NavCategory::GetFriendlyName(mode)->Data());
        fields.AddInt32(L"HistoryListSize", historyListSize);
        fields.AddInt32(L"HistoryItemIndex", loadedIndex);
        TraceLogging::GetInstance()->LogLevel2Event(EVENT_NAME_HISTORY_ITEM_LOAD, fields);
    }

    void TraceLogger::LogMemoryItemLoad(ViewMode mode, int memoryListSize, int loadedIndex)
    {
        LoggingFields fields{};
        fields.AddString(CALC_MODE, NavCategory::GetFriendlyName(mode)->Data());
        fields.AddInt32(L"MemoryListSize", memoryListSize);
        fields.AddInt32(L"MemoryItemIndex", loadedIndex);
        TraceLogging::GetInstance()->LogLevel2Event(EVENT_NAME_MEMORY_ITEM_LOAD, fields);
    }

    void TraceLogger::LogError(ViewMode mode, Platform::String ^ functionName, Platform::String ^ errorString)
    {
        LoggingFields fields{};
        fields.AddString(CALC_MODE, NavCategory::GetFriendlyName(mode)->Data());
        fields.AddString(L"FunctionName", functionName->Data());
        fields.AddString(L"Message", errorString->Data());
        TraceLogging::GetInstance()->LogLevel2Event(EVENT_NAME_EXCEPTION, fields);
    }

    void TraceLogger::LogStandardException(ViewMode mode, wstring_view functionName, const exception& e)
    {
        LoggingFields fields{};
        fields.AddString(CALC_MODE, NavCategory::GetFriendlyName(mode)->Data());
        fields.AddString(L"FunctionName", functionName);
        wstringstream exceptionMessage;
        exceptionMessage << e.what();
        fields.AddString(L"Message", exceptionMessage.str());
        TraceLogging::GetInstance()->LogLevel2Event(EVENT_NAME_EXCEPTION, fields);
    }

    void TraceLogger::LogWinRTException(ViewMode mode, wstring_view functionName, hresult_error const& e)
    {
        LoggingFields fields{};
        fields.AddString(CALC_MODE, NavCategory::GetFriendlyName(mode)->Data());
        fields.AddString(L"FunctionName", functionName);
        fields.AddString(L"Message", e.message());
        fields.AddInt32(L"HRESULT", e.code());
        TraceLogging::GetInstance()->LogLevel2Event(EVENT_NAME_EXCEPTION, fields);
    }

    void TraceLogger::LogPlatformException(ViewMode mode, wstring_view functionName, Platform::Exception ^ e)
    {
        LoggingFields fields{};
        fields.AddString(CALC_MODE, NavCategory::GetFriendlyName(mode)->Data());
        fields.AddString(L"FunctionName", functionName);
        fields.AddString(L"Message", e->Message->Data());
        fields.AddInt32(L"HRESULT", e->HResult);
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

    void TraceLogger::UpdateWindowCount(uint64 windowCount)
    {
        if (windowCount == 0)
        {
            currentWindowCount--;
            return;
        }
        currentWindowCount = windowCount;
    }

    void TraceLogger::DecreaseWindowCount()
    {
        currentWindowCount = 0;
    }

    void TraceLogger::LogButtonUsage()
    {
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
        fields.AddString(L"ButtonUsage", buttonUsageString->Data());
        TraceLogging::GetInstance()->LogLevel2Event(EVENT_NAME_BUTTON_USAGE, fields);

        buttonLog.clear();
    }

    void TraceLogger::LogDateCalculationModeUsed(bool AddSubtractMode)
    {
        const wchar_t* calculationType = AddSubtractMode ? L"AddSubtractMode" : L"DateDifferenceMode";
        LoggingFields fields{};
        fields.AddString(CALC_MODE, NavCategory::GetFriendlyName(ViewMode::Date)->Data());
        fields.AddString(L"CalculationType", calculationType);
        TraceLogging::GetInstance()->LogLevel2Event(EVENT_NAME_DATE_CALCULATION_MODE_USED, fields);
    }

    void TraceLogger::LogConverterInputReceived(ViewMode mode)
    {
        LoggingFields fields{};
        fields.AddString(CALC_MODE, NavCategory::GetFriendlyName(mode)->Data());
        TraceLogging::GetInstance()->LogLevel2Event(EVENT_NAME_CONVERTER_INPUT_RECEIVED, fields);
    }

    void TraceLogger::LogNavBarOpened()
    {
        LoggingFields fields{};
        TraceLogging::GetInstance()->LogLevel2Event(EVENT_NAME_NAV_BAR_OPENED, fields);
    }

    void TraceLogger::LogInputPasted(ViewMode mode)
    {
        LoggingFields fields{};
        fields.AddString(CALC_MODE, NavCategory::GetFriendlyName(mode)->Data());
        TraceLogging::GetInstance()->LogLevel2Event(EVENT_NAME_INPUT_PASTED, fields);
    }

    void TraceLogger::LogShowHideButtonClicked(bool isHideButton)
    {
        LoggingFields fields{};
        fields.AddString(CALC_MODE, GRPAHING_MODE);
        fields.AddBoolean(L"IsHideButton", isHideButton);
        TraceLogging::GetInstance()->LogLevel2Event(EVENT_NAME_SHOW_HIDE_BUTTON_CLICKED, fields);
    }

    void TraceLogger::LogGraphButtonClicked(GraphButton buttonName)
    {
        LoggingFields fields{};
        fields.AddString(CALC_MODE, GRPAHING_MODE);
        fields.AddInt16(L"ButtonName", static_cast<int16>(buttonName));
        TraceLogging::GetInstance()->LogLevel2Event(EVENT_NAME_GRAPH_BUTTON_CLICKED, fields);
    }

    void TraceLogger::LogGraphLineStyleChanged(LineStyleType style)
    {
        LoggingFields fields{};
        fields.AddString(CALC_MODE, GRPAHING_MODE);
        fields.AddInt16(L"StyleType", static_cast<int16>(style));
        TraceLogging::GetInstance()->LogLevel2Event(EVENT_NAME_GRAPH_LINE_STYLE_CHANGED, fields);
    }

    void TraceLogger::LogVariableChanged(String ^ inputChangedType)
    {
        LoggingFields fields{};
        fields.AddString(CALC_MODE, GRPAHING_MODE);
        fields.AddString(L"InputChangedType", inputChangedType->Data());
        TraceLogging::GetInstance()->LogLevel2Event(EVENT_NAME_VARIABLE_CHANGED, fields);
    }
    void TraceLogger::LogVariableSettingsChanged(String ^ setting)
    {
        LoggingFields fields{};
        fields.AddString(CALC_MODE, GRPAHING_MODE);
        fields.AddString(L"SettingChanged", setting->Data());
        TraceLogging::GetInstance()->LogLevel2Event(EVENT_NAME_VARIABLE_SETTING_CHANGED, fields);
    }

    void TraceLogger::LogGraphSettingsChanged(GraphSettingsType settingType)
    {
        LoggingFields fields{};

        fields.AddString(CALC_MODE, GRPAHING_MODE);
        fields.AddInt16(L"SettingType", static_cast<int16>(settingType));

        TraceLogging::GetInstance()->LogLevel2Event(EVENT_NAME_GRAPH_SETTINGS_CHANGED, fields);
    }
}
