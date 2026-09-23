// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using Windows.Foundation.Diagnostics;

namespace TraceLogging
{
    public sealed class TraceLoggingCommon
    {
#if SEND_DIAGNOSTICS
        // c.f. WINEVENT_KEYWORD_RESERVED_63-56 0xFF00000000000000 // Bits 63-56 - channel keywords
        // c.f. WINEVENT_KEYWORD_*              0x00FF000000000000 // Bits 55-48 - system-reserved keywords
        private const long MICROSOFT_KEYWORD_LEVEL_1 = 0x0000800000000000; // Bit 47
        private const long MICROSOFT_KEYWORD_LEVEL_2 = 0x0000400000000000; // Bit 46
        private const long MICROSOFT_KEYWORD_LEVEL_3 = 0x0000200000000000; // Bit 45
#else
        // Define all keyword options as 0 when we do not want to upload app diagnostics.
        private const long MICROSOFT_KEYWORD_LEVEL_1 = 0;
        private const long MICROSOFT_KEYWORD_LEVEL_2 = 0;
        private const long MICROSOFT_KEYWORD_LEVEL_3 = 0;
#endif

        private const string SESSION_GUID = "SessionGuid";
        private const string PDT_PRIVACY_DATA_TAG = "PartA_PrivTags";
        private const ulong PDT_PRODUCT_AND_SERVICE_USAGE = 0x0000000002000000u;

        private static readonly Lazy<TraceLoggingCommon> s_selfInstance =
            new Lazy<TraceLoggingCommon>(() => new TraceLoggingCommon());

        private readonly LoggingChannel g_calculatorProvider;
        private readonly Guid sessionGuid;

        private TraceLoggingCommon()
        {
            g_calculatorProvider = new LoggingChannel(
                "MicrosoftCalculator",
                new LoggingChannelOptions(new Guid(0x4f50731a, 0x89cf, 0x4782, 0xb3, 0xe0, 0xdc, 0xe8, 0xc9, 0x04, 0x76, 0xba)),
                new Guid(0x0905ca09, 0x610e, 0x401e, 0xb6, 0x50, 0x2f, 0x21, 0x29, 0x80, 0xb9, 0xe0));
                // Unique providerID {0905CA09-610E-401E-B650-2F212980B9E0}

            sessionGuid = Guid.NewGuid();
        }

        public static TraceLoggingCommon GetInstance()
        {
            return s_selfInstance.Value;
        }

        public bool GetTraceLoggingProviderEnabled()
        {
            return g_calculatorProvider.Enabled;
        }

        // As mentioned in Microsoft's Privacy Statement(https://privacy.microsoft.com/en-US/privacystatement#maindiagnosticsmodule),
        // sampling is involved in Microsoft's diagnostic data collection process.
        // These keywords provide additional input into how frequently an event might be sampled.
        // The lower the level of the keyword, the higher the possibility that the corresponding event may be sampled.
        public void LogLevel1Event(string eventName, LoggingFields fields)
        {
            if (!GetTraceLoggingProviderEnabled())
            {
                return;
            }

            fields.AddGuid(SESSION_GUID, sessionGuid);
            fields.AddUInt64(PDT_PRIVACY_DATA_TAG, PDT_PRODUCT_AND_SERVICE_USAGE);
            g_calculatorProvider.LogEvent(eventName, fields, LoggingLevel.Verbose, new LoggingOptions(MICROSOFT_KEYWORD_LEVEL_1));
        }

        public void LogLevel2Event(string eventName, LoggingFields fields)
        {
            if (!GetTraceLoggingProviderEnabled())
            {
                return;
            }

            fields.AddGuid(SESSION_GUID, sessionGuid);
            fields.AddUInt64(PDT_PRIVACY_DATA_TAG, PDT_PRODUCT_AND_SERVICE_USAGE);
            g_calculatorProvider.LogEvent(eventName, fields, LoggingLevel.Verbose, new LoggingOptions(MICROSOFT_KEYWORD_LEVEL_2));
        }

        public void LogLevel3Event(string eventName, LoggingFields fields)
        {
            if (!GetTraceLoggingProviderEnabled())
            {
                return;
            }

            fields.AddGuid(SESSION_GUID, sessionGuid);
            fields.AddUInt64(PDT_PRIVACY_DATA_TAG, PDT_PRODUCT_AND_SERVICE_USAGE);
            g_calculatorProvider.LogEvent(eventName, fields, LoggingLevel.Verbose, new LoggingOptions(MICROSOFT_KEYWORD_LEVEL_3));
        }
    }
}
