// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "TraceLogger.h"
#include "NetworkManager.h"
#include "CalculatorButtonUser.h"

using namespace CalculatorApp;
using namespace CalculatorApp::ViewModel::Common;
using namespace CalculatorApp::ViewModel;
using namespace TraceLogging;
using namespace Concurrency;
using namespace std;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Diagnostics;
using namespace Windows::Globalization;
using namespace Windows::Globalization::DateTimeFormatting;
using namespace Windows::System::UserProfile;

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
    constexpr auto EVENT_NAME_GRAPH_THEME = L"GraphTheme";
    constexpr auto EVENT_NAME_RECALL_SNAPSHOT = L"RecallSnapshot";
    constexpr auto EVENT_NAME_RECALL_RESTORE = L"RecallRestore";

    constexpr auto EVENT_NAME_EXCEPTION = L"Exception";

    constexpr auto CALC_MODE = L"CalcMode";
    constexpr auto GRAPHING_MODE = L"Graphing";

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
        auto fields = ref new LoggingFields();

        fields->AddString(StringReference(CALC_MODE), NavCategoryStates::GetFriendlyName(mode));
        fields->AddString(StringReference(L"VisualState"), state);
        fields->AddBoolean(StringReference(L"IsAlwaysOnTop"), isAlwaysOnTop);
        TraceLoggingCommon::GetInstance()->LogLevel2Event(StringReference(EVENT_NAME_VISUAL_STATE_CHANGED), fields);
    }

    void TraceLogger::LogWindowCreated(ViewMode mode, int windowId)
    {
        // store windowId in windowIdLog which says we have logged mode for the present windowId.
        if (!IsWindowIdInLog(windowId))
        {
            windowIdLog.push_back(windowId);
        }

        auto fields = ref new LoggingFields();
        fields->AddString(StringReference(CALC_MODE), NavCategoryStates::GetFriendlyName(mode));
        fields->AddUInt64(StringReference(L"NumOfOpenWindows"), currentWindowCount);
        TraceLoggingCommon::GetInstance()->LogLevel2Event(StringReference(EVENT_NAME_WINDOW_ON_CREATED), fields);
    }

    void TraceLogger::LogModeChange(ViewMode mode)
    {
        if (NavCategoryStates::IsValidViewMode(mode))
        {
            auto fields = ref new LoggingFields();
            fields->AddString(StringReference(CALC_MODE), NavCategoryStates::GetFriendlyName(mode));
            TraceLoggingCommon::GetInstance()->LogLevel2Event(StringReference(EVENT_NAME_MODE_CHANGED), fields);
        }
    }

    void TraceLogger::LogHistoryItemLoad(ViewMode mode, int historyListSize, int loadedIndex)
    {
        auto fields = ref new LoggingFields();
        fields->AddString(StringReference(CALC_MODE), NavCategoryStates::GetFriendlyName(mode));
        fields->AddInt32(StringReference(L"HistoryListSize"), historyListSize);
        fields->AddInt32(StringReference(L"HistoryItemIndex"), loadedIndex);
        TraceLoggingCommon::GetInstance()->LogLevel2Event(StringReference(EVENT_NAME_HISTORY_ITEM_LOAD), fields);
    }

    void TraceLogger::LogMemoryItemLoad(ViewMode mode, int memoryListSize, int loadedIndex)
    {
        auto fields = ref new LoggingFields();
        fields->AddString(StringReference(CALC_MODE), NavCategoryStates::GetFriendlyName(mode));
        fields->AddInt32(StringReference(L"MemoryListSize"), memoryListSize);
        fields->AddInt32(StringReference(L"MemoryItemIndex"), loadedIndex);
        TraceLoggingCommon::GetInstance()->LogLevel2Event(StringReference(EVENT_NAME_MEMORY_ITEM_LOAD), fields);
    }

    void TraceLogger::LogError(ViewMode mode, Platform::String ^ functionName, Platform::String ^ errorString)
    {
        auto fields = ref new LoggingFields();
        fields->AddString(StringReference(CALC_MODE), NavCategoryStates::GetFriendlyName(mode));
        fields->AddString(StringReference(L"FunctionName"), functionName);
        fields->AddString(StringReference(L"Message"), errorString);
        TraceLoggingCommon::GetInstance()->LogLevel2Event(StringReference(EVENT_NAME_EXCEPTION), fields);
    }

    void TraceLogger::LogStandardException(ViewMode mode, wstring_view functionName, const exception& e)
    {
        auto fields = ref new LoggingFields();
        fields->AddString(StringReference(CALC_MODE), NavCategoryStates::GetFriendlyName(mode));
        fields->AddString(StringReference(L"FunctionName"), StringReference(functionName.data()));
        wstringstream exceptionMessage;
        exceptionMessage << e.what();
        fields->AddString(StringReference(L"Message"), StringReference(exceptionMessage.str().data()));
        TraceLoggingCommon::GetInstance()->LogLevel2Event(StringReference(EVENT_NAME_EXCEPTION), fields);
    }

    void TraceLogger::LogPlatformExceptionInfo(CalculatorApp::ViewModel::Common::ViewMode mode, Platform::String ^ functionName, Platform::String^ message, int hresult)
    {
        auto fields = ref new LoggingFields();
        fields->AddString(StringReference(CALC_MODE), NavCategoryStates::GetFriendlyName(mode));
        fields->AddString(StringReference(L"FunctionName"), functionName);
        fields->AddString(StringReference(L"Message"), message);
        fields->AddInt32(StringReference(L"HRESULT"), hresult);
        TraceLoggingCommon::GetInstance()->LogLevel2Event(StringReference(EVENT_NAME_EXCEPTION), fields);
    }

    void TraceLogger::LogPlatformException(ViewMode mode, Platform::String ^ functionName, Platform::Exception ^ e)
    {
        LogPlatformExceptionInfo(mode, functionName, e->Message, e->HResult);
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
            buttonUsageString += NavCategoryStates::GetFriendlyName(buttonLog[i].mode);
            buttonUsageString += "|";
            buttonUsageString += buttonLog[i].button.ToString();
            buttonUsageString += "|";
            buttonUsageString += buttonLog[i].count;
            if (i != buttonLog.size() - 1)
            {
                buttonUsageString += ",";
            }
        }

        auto fields = ref new LoggingFields();
        fields->AddString(StringReference(L"ButtonUsage"), buttonUsageString);
        TraceLoggingCommon::GetInstance()->LogLevel2Event(StringReference(EVENT_NAME_BUTTON_USAGE), fields);

        buttonLog.clear();
    }

    void TraceLogger::LogDateCalculationModeUsed(bool AddSubtractMode)
    {
        const wchar_t* calculationType = AddSubtractMode ? L"AddSubtractMode" : L"DateDifferenceMode";
        auto fields = ref new LoggingFields();
        fields->AddString(StringReference(CALC_MODE), NavCategoryStates::GetFriendlyName(ViewMode::Date));
        fields->AddString(StringReference(L"CalculationType"), StringReference(calculationType));
        TraceLoggingCommon::GetInstance()->LogLevel2Event(StringReference(EVENT_NAME_DATE_CALCULATION_MODE_USED), fields);
    }

    void TraceLogger::LogConverterInputReceived(ViewMode mode)
    {
        auto fields = ref new LoggingFields();
        fields->AddString(StringReference(CALC_MODE), NavCategoryStates::GetFriendlyName(mode));
        TraceLoggingCommon::GetInstance()->LogLevel2Event(StringReference(EVENT_NAME_CONVERTER_INPUT_RECEIVED), fields);
    }

    void TraceLogger::LogNavBarOpened()
    {
        auto fields = ref new LoggingFields();
        TraceLoggingCommon::GetInstance()->LogLevel2Event(StringReference(EVENT_NAME_NAV_BAR_OPENED), fields);
    }

    void TraceLogger::LogInputPasted(ViewMode mode)
    {
        auto fields = ref new LoggingFields();
        fields->AddString(StringReference(CALC_MODE), NavCategoryStates::GetFriendlyName(mode));
        TraceLoggingCommon::GetInstance()->LogLevel2Event(StringReference(EVENT_NAME_INPUT_PASTED), fields);
    }

    void TraceLogger::LogShowHideButtonClicked(bool isHideButton)
    {
        auto fields = ref new LoggingFields();
        fields->AddString(StringReference(CALC_MODE), StringReference(GRAPHING_MODE));
        fields->AddBoolean(StringReference(L"IsHideButton"), isHideButton);
        TraceLoggingCommon::GetInstance()->LogLevel2Event(StringReference(EVENT_NAME_SHOW_HIDE_BUTTON_CLICKED), fields);
    }

    void TraceLogger::LogGraphButtonClicked(GraphButton buttonName, GraphButtonValue buttonValue)
    {
        auto fields = ref new LoggingFields();
        fields->AddString(StringReference(CALC_MODE), StringReference(GRAPHING_MODE));
        fields->AddInt16(StringReference(L"ButtonName"), static_cast<int16>(buttonName));
        fields->AddInt16(StringReference(L"ButtonValue"), static_cast<int16>(buttonValue));
        TraceLoggingCommon::GetInstance()->LogLevel2Event(StringReference(EVENT_NAME_GRAPH_BUTTON_CLICKED), fields);
    }

    void TraceLogger::LogGraphLineStyleChanged(LineStyleType style)
    {
        auto fields = ref new LoggingFields();
        fields->AddString(StringReference(CALC_MODE), StringReference(GRAPHING_MODE));
        fields->AddInt16(StringReference(L"StyleType"), static_cast<int16>(style));
        TraceLoggingCommon::GetInstance()->LogLevel2Event(StringReference(EVENT_NAME_GRAPH_LINE_STYLE_CHANGED), fields);
    }

    void TraceLogger::LogVariableChanged(String ^ inputChangedType, String ^ variableName)
    {
        auto fields = ref new LoggingFields();
        fields->AddString(StringReference(CALC_MODE), StringReference(GRAPHING_MODE));
        fields->AddString(StringReference(L"InputChangedType"), inputChangedType);
        fields->AddString(StringReference(L"VariableName"), variableName);
        TraceLoggingCommon::GetInstance()->LogLevel2Event(StringReference(EVENT_NAME_VARIABLE_CHANGED), fields);
    }
    void TraceLogger::LogVariableSettingsChanged(String ^ setting)
    {
        auto fields = ref new LoggingFields();
        fields->AddString(StringReference(CALC_MODE), StringReference(GRAPHING_MODE));
        fields->AddString(StringReference(L"SettingChanged"), setting);
        TraceLoggingCommon::GetInstance()->LogLevel2Event(StringReference(EVENT_NAME_VARIABLE_SETTING_CHANGED), fields);
    }

    void TraceLogger::LogGraphSettingsChanged(GraphSettingsType settingType, String ^ settingValue)
    {
        auto fields = ref new LoggingFields();
        fields->AddString(StringReference(CALC_MODE), StringReference(GRAPHING_MODE));
        fields->AddInt16(L"SettingType", static_cast<int16>(settingType));
        fields->AddString(L"SettingValue", settingValue);

        TraceLoggingCommon::GetInstance()->LogLevel2Event(StringReference(EVENT_NAME_GRAPH_SETTINGS_CHANGED), fields);
    }

    void TraceLogger::LogGraphTheme(String ^ graphTheme)
    {
        auto fields = ref new LoggingFields();
        fields->AddString(StringReference(CALC_MODE), StringReference(GRAPHING_MODE));
        fields->AddString(L"GraphTheme", graphTheme);

        TraceLoggingCommon::GetInstance()->LogLevel2Event(StringReference(EVENT_NAME_GRAPH_THEME), fields);
    }

    void TraceLogger::LogRecallSnapshot(ViewMode mode)
    {
        auto fields = ref new LoggingFields();
        fields->AddString(StringReference(CALC_MODE), NavCategoryStates::GetFriendlyName(mode));
        TraceLoggingCommon::GetInstance()->LogLevel2Event(StringReference(EVENT_NAME_RECALL_SNAPSHOT), fields);
    }

    void TraceLogger::LogRecallRestore(ViewMode mode)
    {
        auto fields = ref new LoggingFields();
        fields->AddString(StringReference(CALC_MODE), NavCategoryStates::GetFriendlyName(mode));
        TraceLoggingCommon::GetInstance()->LogLevel2Event(StringReference(EVENT_NAME_RECALL_RESTORE), fields);
    }

    void TraceLogger::LogRecallError(Platform::String^ message)
    {
        auto fields = ref new LoggingFields();
        fields->AddString(StringReference(L"FunctionName"), L"Recall");
        fields->AddString(StringReference(L"Message"), message);
        TraceLoggingCommon::GetInstance()->LogLevel2Event(StringReference(EVENT_NAME_EXCEPTION), fields);
    }
}
