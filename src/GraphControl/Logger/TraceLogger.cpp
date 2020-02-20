// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "TraceLogger.h"

using namespace Concurrency;
using namespace std;
using namespace Platform;
using namespace winrt;
using namespace winrt::Windows::Foundation;
using namespace winrt::Windows::Foundation::Diagnostics;
using namespace winrt::Windows::Globalization;

namespace GraphControl
{
    static reader_writer_lock s_traceLoggerLock;

    constexpr auto GraphingMode = L"Graphing";
    constexpr auto SessionGuid = L"SessionGuid";
    constexpr auto CalcMode = L"CalcMode";

    // Diagnostics events. Uploaded to asimov.
    constexpr auto EVENT_NAME_EQUATION_COUNT_CHANGED = L"EquationCountChanged";
    constexpr auto EVENT_NAME_FUNCTION_ANALYSIS_PERFORMED = L"FunctionAnalysisPerformed";
    constexpr auto EVENT_NAME_VARIABLES_COUNT_CHANGED = L"VariblesCountChanged";

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

    TraceLogger ^ TraceLogger::GetInstance()
    {
        static TraceLogger ^ s_selfInstance = ref new TraceLogger();
        return s_selfInstance;
    }

    bool TraceLogger::GetTraceLoggingProviderEnabled()
    {
        return g_calculatorProvider.Enabled();
    }

#pragma region Tracing methods
    void TraceLogger::LogLevel1Event(wstring_view eventName, LoggingFields fields)
    {
        g_calculatorProvider.LogEvent(eventName, fields, LoggingLevel::Verbose, LoggingOptions(MICROSOFT_KEYWORD_LEVEL_1));
    }

    void TraceLogger::LogLevel2Event(wstring_view eventName, LoggingFields fields)
    {
        g_calculatorProvider.LogEvent(eventName, fields, LoggingLevel::Verbose, LoggingOptions(MICROSOFT_KEYWORD_LEVEL_2));
    }

    void TraceLogger::LogLevel3Event(wstring_view eventName, LoggingFields fields)
    {
        g_calculatorProvider.LogEvent(eventName, fields, LoggingLevel::Verbose, LoggingOptions(MICROSOFT_KEYWORD_LEVEL_3));
    }

    unique_ptr<TraceActivity> TraceLogger::CreateTraceActivity(wstring_view eventName, LoggingFields fields)
    {
        return make_unique<TraceActivity>(g_calculatorProvider, eventName, fields);
    }
#pragma endregion

    void TraceLogger::LogEquationCountChanged(int currentValidEquations, int currentInvalidEquations)
    {
        static bool firstRun = true;
        if (firstRun)
        {
            firstRun = false;
            return;
        }

        // Update the total valid/invalid equations to record the max value.
        // Equations are added/removed/updated one at a time, so we know either
        // currentValidEquations or currentInvalidEquations increased,
        // but they cannot both increase at the same time
        // If an equation was removed, do not decrement the total count.
        static uint64 TotalValidEquations = 0;
        static uint64 TotalInvalidEquations = 0;
        static uint64 PreviousValidEquations = 0;
        static uint64 PreviousInvalidEquations = 0;

        if (currentValidEquations > PreviousValidEquations)
        {
            TotalValidEquations++;
        }
        else if (currentInvalidEquations > PreviousInvalidEquations)
        {
            TotalInvalidEquations++;
        }

        PreviousValidEquations = currentValidEquations;
        PreviousInvalidEquations = currentInvalidEquations;

        LoggingFields fields{};
        fields.AddGuid(SessionGuid, sessionGuid);
        fields.AddString(CalcMode, GraphingMode);
        fields.AddUInt64(L"ConcurrentValidFunctions", currentValidEquations);
        fields.AddUInt64(L"ConcurrentInvalidFunctions", currentInvalidEquations);
        fields.AddUInt64(L"TotalValidFunctions", TotalValidEquations);
        fields.AddUInt64(L"TotalInvalidFunctions", TotalInvalidEquations);
        fields.AddUInt64(PDT_PRIVACY_DATA_TAG, PDT_PRODUCT_AND_SERVICE_USAGE);
        LogLevel2Event(EVENT_NAME_EQUATION_COUNT_CHANGED, fields);
    }

    void TraceLogger::LogFunctionAnalysisPerformed(String ^ errorMessage)
    {
        LoggingFields fields{};
        fields.AddGuid(SessionGuid, sessionGuid);
        fields.AddString(CalcMode, GraphingMode);
        fields.AddString(L"ErrorMessage", errorMessage->Data());
        fields.AddUInt64(PDT_PRIVACY_DATA_TAG, PDT_PRODUCT_AND_SERVICE_USAGE);
        LogLevel2Event(EVENT_NAME_FUNCTION_ANALYSIS_PERFORMED, fields);
    }

    void TraceLogger::LogVariableCountChanged(uint64 numVariables)
    {
        if (!GetTraceLoggingProviderEnabled())
            return;

        static uint64 maxVariableCount = 0;

        if (numVariables > maxVariableCount)
        {
        LoggingFields fields{};
        fields.AddGuid(SessionGuid, sessionGuid);
        fields.AddString(CalcMode, GraphingMode);
        fields.AddUInt64(L"NumberVariables", numVariables);
        fields.AddUInt64(PDT_PRIVACY_DATA_TAG, PDT_PRODUCT_AND_SERVICE_USAGE);
        LogLevel2Event(EVENT_NAME_VARIABLES_COUNT_CHANGED, fields);
        maxVariableCount = numVariables;
        }


    }
}
