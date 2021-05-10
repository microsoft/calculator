// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

namespace TraceLogging
{
    public ref class TraceLoggingCommon sealed
    {
    public:
        static TraceLoggingCommon ^ GetInstance();

        // As mentioned in Microsoft's Privacy Statement(https://privacy.microsoft.com/en-US/privacystatement#maindiagnosticsmodule),
        // sampling is involved in Microsoft's diagnostic data collection process.
        // These keywords provide additional input into how frequently an event might be sampled.
        // The lower the level of the keyword, the higher the possibility that the corresponding event may be sampled.
        void LogLevel1Event(Platform::String ^ eventName, Windows::Foundation::Diagnostics::LoggingFields ^ fields);
        void LogLevel2Event(Platform::String ^ eventName, Windows::Foundation::Diagnostics::LoggingFields ^ fields);
        void LogLevel3Event(Platform::String ^ eventName, Windows::Foundation::Diagnostics::LoggingFields ^ fields);

        bool GetTraceLoggingProviderEnabled();

    private:
        TraceLoggingCommon();

        Windows::Foundation::Diagnostics::LoggingChannel ^ g_calculatorProvider;
        Windows::Foundation::Diagnostics::LoggingActivity ^ m_appLaunchActivity;
        GUID sessionGuid;
    };
}
