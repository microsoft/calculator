// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "TraceLogger.h"

using namespace TraceLogging;
using namespace Concurrency;
using namespace std;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Diagnostics;
using namespace Windows::Globalization;

namespace GraphControl
{
    static reader_writer_lock s_traceLoggerLock;

    constexpr auto GRAPHING_MODE = L"Graphing";
    constexpr auto CALC_MODE = L"CalcMode";

    // Diagnostics events. Uploaded to asimov.
    constexpr auto EVENT_NAME_EQUATION_COUNT_CHANGED = L"EquationCountChanged";
    constexpr auto EVENT_NAME_FUNCTION_ANALYSIS_PERFORMED = L"FunctionAnalysisPerformed";
    constexpr auto EVENT_NAME_VARIABLES_COUNT_CHANGED = L"VariablesCountChanged";
    constexpr auto EVENT_NAME_LINE_WIDTH_CHANGED = L"LineWidthChanged";

    TraceLogger ^ TraceLogger::GetInstance()
    {
        static TraceLogger ^ s_selfInstance = ref new TraceLogger();
        return s_selfInstance;
    }

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

        auto fields = ref new LoggingFields();
        fields->AddString(StringReference(CALC_MODE), StringReference(GRAPHING_MODE));
        fields->AddUInt64(StringReference(L"ConcurrentValidFunctions"), currentValidEquations);
        fields->AddUInt64(StringReference(L"ConcurrentInvalidFunctions"), currentInvalidEquations);
        fields->AddUInt64(StringReference(L"TotalValidFunctions"), TotalValidEquations);
        fields->AddUInt64(StringReference(L"TotalInvalidFunctions"), TotalInvalidEquations);
        TraceLoggingCommon::GetInstance()->LogLevel2Event(StringReference(EVENT_NAME_EQUATION_COUNT_CHANGED), fields);
    }

    void TraceLogger::LogFunctionAnalysisPerformed(int analysisErrorType, uint32 tooComplexFlag)
    {
        auto fields = ref new LoggingFields();
        fields->AddString(StringReference(CALC_MODE), StringReference(GRAPHING_MODE));
        fields->AddInt32(StringReference(L"AnalysisErrorType"), analysisErrorType);
        fields->AddUInt32(StringReference(L"TooComplexFeatures"), tooComplexFlag);
        TraceLoggingCommon::GetInstance()->LogLevel2Event(StringReference(EVENT_NAME_FUNCTION_ANALYSIS_PERFORMED), fields);
    }

    void TraceLogger::LogVariableCountChanged(int variablesCount)
    {
        auto fields = ref new LoggingFields();
        fields->AddString(StringReference(CALC_MODE), StringReference(GRAPHING_MODE));
        fields->AddInt64(StringReference(L"VariableCount"), variablesCount);
        TraceLoggingCommon::GetInstance()->LogLevel2Event(StringReference(EVENT_NAME_VARIABLES_COUNT_CHANGED), fields);
    }

    void TraceLogger::LogLineWidthChanged()
    {
        auto fields = ref new LoggingFields();
        fields->AddString(StringReference(CALC_MODE), StringReference(GRAPHING_MODE));
        TraceLoggingCommon::GetInstance()->LogLevel2Event(StringReference(EVENT_NAME_LINE_WIDTH_CHANGED), fields);
    }
}
