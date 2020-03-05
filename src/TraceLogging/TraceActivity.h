// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once
#include <winmeta.h>

namespace CalculatorApp
{
    // RAII wrapper that automatically sends the Stop event when the class gets destructed.
public
    ref class TraceActivity sealed
    {
    public:
        TraceActivity()
            : m_channel(nullptr)
            , m_activity(nullptr)
            , m_fields(nullptr)
        {
        }

        TraceActivity(
            Windows::Foundation::Diagnostics::LoggingChannel ^ channel,
            Platform::String ^ activityName,
            Windows::Foundation::Diagnostics::LoggingFields ^ fields)
            : m_channel(channel)
            , m_activityName(activityName)
            , m_fields(fields)
            , m_activity(nullptr)
        {
            // Write the activity's START event. Note that you must not specify keyword
            // or level for START and STOP events because they always use the activity's
            // keyword and level.
            m_activity = m_channel->StartActivity(
                m_activityName,
                m_fields,
                Windows::Foundation::Diagnostics::LoggingLevel::Verbose,
                ref new Windows::Foundation::Diagnostics::LoggingOptions(WINEVENT_KEYWORD_RESPONSE_TIME));
        }

    private:
        ~TraceActivity()
        {
            if (m_activity != nullptr)
            {
                // Write the activity's STOP event.
                m_activity->StopActivity(m_activityName, m_fields);
                m_activity = nullptr;
            }
        }

        Platform::String ^ m_activityName;
        Windows::Foundation::Diagnostics::LoggingChannel ^ m_channel;
        Windows::Foundation::Diagnostics::LoggingFields ^ m_fields;
        Windows::Foundation::Diagnostics::LoggingActivity ^ m_activity;
    };
}
