// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using CalculatorApp.ViewModel.Common;

using System;

using Windows.ApplicationModel;
using Windows.ApplicationModel.Core;
using Windows.Foundation.Diagnostics;
using Windows.UI.ViewManagement;

namespace CalculatorApp
{
    public static class Globals
    {
#if SEND_DIAGNOSTICS
        // c.f. WINEVENT_KEYWORD_RESERVED_63-56 0xFF00000000000000 // Bits 63-56 - channel keywords
        // c.f. WINEVENT_KEYWORD_*              0x00FF000000000000 // Bits 55-48 - system-reserved keywords
        public const long MICROSOFT_KEYWORD_LEVEL_1 = 0x0000800000000000; // Bit 47
        public const long MICROSOFT_KEYWORD_LEVEL_2 = 0x0000400000000000;      // Bit 46
        public const long MICROSOFT_KEYWORD_LEVEL_3 = 0x0000200000000000;     // Bit 45
        public const long MICROSOFT_KEYWORD_RESERVED_44 = 0x0000100000000000;   // Bit 44 (reserved for future assignment)
#else
        // define all Keyword options as 0 when we do not want to upload app diagnostics
        public const long MICROSOFT_KEYWORD_LEVEL_1 = 0;
        public const long MICROSOFT_KEYWORD_LEVEL_2 = 0;
        public const long MICROSOFT_KEYWORD_LEVEL_3 = 0;
        public const long MICROSOFT_KEYWORD_RESERVED_44 = 0;
#endif
    }

    internal class AppLifecycleLogger
    {
        public static AppLifecycleLogger GetInstance()
        {
            return s_selfInstance.Value;
        }

        public bool GetTraceLoggingProviderEnabled()
        {
            return m_appLifecycleProvider.Enabled;
        }

        public void LaunchUIResponsive()
        {
            if (!GetTraceLoggingProviderEnabled())
                return;

            LoggingFields fields = new LoggingFields();
            PopulateAppInfo(fields);
            LogAppLifecycleEvent("ModernAppLaunch_UIResponsive", fields);
        }

        public void LaunchVisibleComplete()
        {
            if (!GetTraceLoggingProviderEnabled())
                return;

            LoggingFields fields = new LoggingFields();
            PopulateAppInfo(fields);
            LogAppLifecycleEvent("ModernAppLaunch_VisibleComplete", fields);
        }

        public void ResumeUIResponsive()
        {
            if (!GetTraceLoggingProviderEnabled())
                return;

            LoggingFields fields = new LoggingFields();
            PopulateAppInfo(fields);
            LogAppLifecycleEvent("ModernAppResume_UIResponsive", fields);
        }

        public void ResumeVisibleComplete()
        {
            if (!GetTraceLoggingProviderEnabled())
                return;

            LoggingFields fields = new LoggingFields();
            PopulateAppInfo(fields);
            LogAppLifecycleEvent("ModernAppResume_VisibleComplete", fields);
        }

        public void ResizeUIResponsive()
        {
            ResizeUIResponsive(ApplicationView.GetForCurrentView().Id);
        }

        public void ResizeVisibleComplete()
        {
            ResizeVisibleComplete(ApplicationView.GetForCurrentView().Id);
        }

        public void ResizeUIResponsive(int viewId)
        {
            if (!GetTraceLoggingProviderEnabled())
                return;

            LoggingFields fields = new LoggingFields();
            PopulateAppInfo(fields);
            fields.AddInt32("ViewId", viewId);
            LogAppLifecycleEvent("ModernAppResize_UIResponsive", fields);
        }

        public void ResizeVisibleComplete(int viewId)
        {
            if (!GetTraceLoggingProviderEnabled())
                return;

            LoggingFields fields = new LoggingFields();
            PopulateAppInfo(fields);
            fields.AddInt32("ViewId", viewId);
            LogAppLifecycleEvent("ModernAppResize_VisibleComplete", fields);
        }

        // Make the object construction private to allow singleton access to this class
        private AppLifecycleLogger()
        {
            m_appLifecycleProvider = new LoggingChannel(
                        "Microsoft.Windows.AppLifeCycle",
                        new LoggingChannelOptions(new Guid(0x4f50731a, 0x89cf, 0x4782, 0xb3, 0xe0, 0xdc, 0xe8, 0xc9, 0x4, 0x76, 0xba)),
                        new Guid(0xef00584a, 0x2655, 0x462c, 0xbc, 0x24, 0xe7, 0xde, 0x63, 0xe, 0x7f, 0xbf));
        }

        // Any new Log method should
        // a) Decide the level of logging. This will help us in limiting recording of events only up to a certain level. See this link for guidance
        // https://msdn.microsoft.com/en-us/library/windows/desktop/aa363742(v=vs.85).aspx We're using Verbose level for events that are called frequently and
        // needed only for debugging or capturing perf for specific scenarios b) Should decide whether or not to log to diagnostics and pass
        // TraceLoggingKeyword(MICROSOFT_KEYWORD_LEVEL_3) accordingly c) Should accept a variable number of additional data arguments if needed
        private void LogAppLifecycleEvent(string eventName, LoggingFields fields)
        {
            m_appLifecycleProvider.LogEvent(
                eventName, fields, LoggingLevel.Information, new LoggingOptions(Globals.MICROSOFT_KEYWORD_LEVEL_3 | Utilities.GetConst_WINEVENT_KEYWORD_RESPONSE_TIME()));
        }

        private void PopulateAppInfo(LoggingFields fields)
        {
            var appId = CoreApplication.Id;
            var aumId = Package.Current.Id.FamilyName + "!" + appId;
            var packageFullName = Package.Current.Id.FullName;
            var psmKey = Package.Current.Id.FullName + "+" + appId;

            fields.AddString("AumId", aumId);
            fields.AddString("PackageFullName", packageFullName);
            fields.AddString("PsmKey", psmKey);
        }

        private readonly LoggingChannel m_appLifecycleProvider;
        private static readonly Lazy<AppLifecycleLogger> s_selfInstance = new Lazy<AppLifecycleLogger>(() => new AppLifecycleLogger(), true);
    }
}

