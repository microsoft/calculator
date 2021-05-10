// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "TraceLoggingCommon.h"

using namespace TraceLogging;
using namespace std;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Diagnostics;

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

constexpr auto SESSION_GUID = L"SessionGuid";
constexpr auto PDT_PRIVACY_DATA_TAG = L"PartA_PrivTags";
constexpr auto PDT_PRODUCT_AND_SERVICE_USAGE = 0x0000'0000'0200'0000u;

TraceLoggingCommon::TraceLoggingCommon()
    : g_calculatorProvider(
        ref new LoggingChannel(
        L"MicrosoftCalculator",
        ref new LoggingChannelOptions(GUID{ 0x4f50731a, 0x89cf, 0x4782, 0xb3, 0xe0, 0xdc, 0xe8, 0xc9, 0x4, 0x76, 0xba }),
        GUID{ 0x905ca09, 0x610e, 0x401e, 0xb6, 0x50, 0x2f, 0x21, 0x29, 0x80, 0xb9, 0xe0 }))
    , // Unique providerID {0905CA09-610E-401E-B650-2F212980B9E0}
    m_appLaunchActivity{ nullptr }
{
    CoCreateGuid(&sessionGuid);
}

TraceLoggingCommon ^ TraceLoggingCommon::GetInstance()
{
    static TraceLoggingCommon ^ s_selfInstance = ref new TraceLoggingCommon();
    return s_selfInstance;
}

bool TraceLoggingCommon::GetTraceLoggingProviderEnabled()
{
    return g_calculatorProvider->Enabled;
}

void TraceLoggingCommon::LogLevel1Event(String ^ eventName, LoggingFields ^ fields)
{
    if (!GetTraceLoggingProviderEnabled())
    {
        return;
    }

    fields->AddGuid(ref new String(SESSION_GUID), sessionGuid);
    fields->AddUInt64(ref new String(PDT_PRIVACY_DATA_TAG), PDT_PRODUCT_AND_SERVICE_USAGE);

    g_calculatorProvider->LogEvent(eventName, fields, LoggingLevel::Verbose, ref new LoggingOptions(MICROSOFT_KEYWORD_LEVEL_1));
}

void TraceLoggingCommon::LogLevel2Event(String ^ eventName, LoggingFields ^ fields)
{
    if (!GetTraceLoggingProviderEnabled())
    {
        return;
    }

    fields->AddGuid(ref new String(SESSION_GUID), sessionGuid);
    fields->AddUInt64(ref new String(PDT_PRIVACY_DATA_TAG), PDT_PRODUCT_AND_SERVICE_USAGE);
    g_calculatorProvider->LogEvent(eventName, fields, LoggingLevel::Verbose, ref new LoggingOptions(MICROSOFT_KEYWORD_LEVEL_2));
}

void TraceLoggingCommon::LogLevel3Event(String ^ eventName, LoggingFields ^ fields)
{
    if (!GetTraceLoggingProviderEnabled())
    {
        return;
    }

    fields->AddGuid(ref new String(SESSION_GUID), sessionGuid);
    fields->AddUInt64(ref new String(PDT_PRIVACY_DATA_TAG), PDT_PRODUCT_AND_SERVICE_USAGE);
    g_calculatorProvider->LogEvent(eventName, fields, LoggingLevel::Verbose, ref new LoggingOptions(MICROSOFT_KEYWORD_LEVEL_3));
}
