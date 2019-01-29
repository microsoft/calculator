// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

namespace CalculatorApp
{
    constexpr int64_t WINEVENT_KEYWORD_RESPONSE_TIME = 0x1000000000000;

    // RAII wrapper that automatically sends the Stop event when the class gets destructed.
    class TraceActivity
    {
    public:
        TraceActivity() :
            m_channel(nullptr),
            m_activity(nullptr),
            m_fields(nullptr)
        { }

        TraceActivity(
            winrt::Windows::Foundation::Diagnostics::LoggingChannel channel,
            std::wstring_view activityName,
            winrt::Windows::Foundation::Diagnostics::LoggingFields fields) :
            m_channel(channel),
            m_activityName(activityName),
            m_fields(fields),
            m_activity(nullptr)
        {
            // Write the activity's START event. Note that you must not specify keyword
            // or level for START and STOP events because they always use the activity's
            // keyword and level.
            m_activity = m_channel.StartActivity(
                m_activityName,
                m_fields,
                winrt::Windows::Foundation::Diagnostics::LoggingLevel::Verbose,
                winrt::Windows::Foundation::Diagnostics::LoggingOptions(WINEVENT_KEYWORD_RESPONSE_TIME)
                );
        }

        ~TraceActivity()
        {
            if (m_activity != nullptr)
            {
                // Write the activity's STOP event.
                m_activity.StopActivity(m_activityName, m_fields);
                m_activity = nullptr;
            }
        }

    private:
        std::wstring m_activityName;
        winrt::Windows::Foundation::Diagnostics::LoggingChannel m_channel;
        winrt::Windows::Foundation::Diagnostics::LoggingFields m_fields;
        winrt::Windows::Foundation::Diagnostics::LoggingActivity m_activity;
    };
}
