// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "TraceActivity.h"
#include "Common.h"


// A trace logging provider can only be instantiated and registered once per module.
// This class implements a singleton model ensure that only one instance is created.
namespace GraphControl
{

public
    ref class TraceLogger sealed
    {
    internal:
        static TraceLogger ^ GetInstance();
        bool GetTraceLoggingProviderEnabled();

        void LogEquationCountChanged(int currentValidEquations, int currentInvalidEquations);
        void LogFunctionAnalysisPerformed(int analysisErrorType, uint32 tooComplexFlag);
        void LogVariableAdded(int variablesCount);

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

        GUID sessionGuid;

        winrt::Windows::Foundation::Diagnostics::LoggingActivity m_appLaunchActivity;
    };
}
