// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "AppLifecycleLogger.h"
#include <winmeta.h>

using namespace std;
using namespace winrt;
using namespace winrt::Windows::ApplicationModel;
using namespace winrt::Windows::ApplicationModel::Background;
using namespace winrt::Windows::ApplicationModel::Core;
using namespace winrt::Windows::Foundation::Diagnostics;
using namespace winrt::Windows::UI::ViewManagement;

namespace CalculatorApp
{
#ifdef SEND_DIAGNOSTICS
    // c.f. WINEVENT_KEYWORD_RESERVED_63-56 0xFF00000000000000 // Bits 63-56 - channel keywords
    // c.f. WINEVENT_KEYWORD_*              0x00FF000000000000 // Bits 55-48 - system-reserved keywords
    constexpr int64_t MICROSOFT_KEYWORD_LEVEL_1 = 0x0000800000000000;     // Bit 47
    constexpr int64_t MICROSOFT_KEYWORD_LEVEL_2 = 0x0000400000000000;     // Bit 46
    constexpr int64_t MICROSOFT_KEYWORD_LEVEL_3 = 0x0000200000000000;     // Bit 45
    constexpr int64_t MICROSOFT_KEYWORD_RESERVED_44 = 0x0000100000000000; // Bit 44 (reserved for future assignment)
#else
    // define all Keyword options as 0 when we do not want to upload app diagnostics
    constexpr int64_t MICROSOFT_KEYWORD_LEVEL_1 = 0;
    constexpr int64_t MICROSOFT_KEYWORD_LEVEL_2 = 0;
    constexpr int64_t MICROSOFT_KEYWORD_LEVEL_3 = 0;
    constexpr int64_t MICROSOFT_KEYWORD_RESERVED_44 = 0;
#endif

#pragma region TraceLogger setup and cleanup

    AppLifecycleLogger::AppLifecycleLogger()
        : m_appLifecycleProvider(
            L"Microsoft.Windows.AppLifeCycle",
            LoggingChannelOptions(GUID{ 0x4f50731a, 0x89cf, 0x4782, 0xb3, 0xe0, 0xdc, 0xe8, 0xc9, 0x4, 0x76, 0xba }),
            GUID{ 0xef00584a, 0x2655, 0x462c, 0xbc, 0x24, 0xe7, 0xde, 0x63, 0xe, 0x7f, 0xbf }) // Unique provider ID {EF00584A-2655-462C-BC24-E7DE630E7FBF}
    {
    }

    AppLifecycleLogger::~AppLifecycleLogger()
    {
    }

    AppLifecycleLogger& AppLifecycleLogger::GetInstance()
    {
        static AppLifecycleLogger s_selfInstance;
        return s_selfInstance;
    }

    bool AppLifecycleLogger::GetTraceLoggingProviderEnabled() const
    {
        return m_appLifecycleProvider.Enabled();
    }

#pragma region Tracing methods
    void AppLifecycleLogger::LogAppLifecycleEvent(hstring const& eventName, LoggingFields const& fields) const
    {
        m_appLifecycleProvider.LogEvent(
            eventName, fields, LoggingLevel::Information, LoggingOptions(MICROSOFT_KEYWORD_LEVEL_3 | WINEVENT_KEYWORD_RESPONSE_TIME));
    }
#pragma endregion

    void AppLifecycleLogger::LaunchUIResponsive() const
    {
        if (!GetTraceLoggingProviderEnabled())
            return;

        LoggingFields fields{};
        PopulateAppInfo(fields);
        LogAppLifecycleEvent(L"ModernAppLaunch_UIResponsive", fields);
    }

    void AppLifecycleLogger::LaunchVisibleComplete() const
    {
        if (!GetTraceLoggingProviderEnabled())
            return;

        LoggingFields fields{};
        PopulateAppInfo(fields);
        LogAppLifecycleEvent(L"ModernAppLaunch_VisibleComplete", fields);
    }

    void AppLifecycleLogger::ResumeUIResponsive() const
    {
        if (!GetTraceLoggingProviderEnabled())
            return;

        LoggingFields fields{};
        PopulateAppInfo(fields);
        LogAppLifecycleEvent(L"ModernAppResume_UIResponsive", fields);
    }

    void AppLifecycleLogger::ResumeVisibleComplete() const
    {
        if (!GetTraceLoggingProviderEnabled())
            return;

        LoggingFields fields{};
        PopulateAppInfo(fields);
        LogAppLifecycleEvent(L"ModernAppResume_VisibleComplete", fields);
    }

    void AppLifecycleLogger::ResizeUIResponsive() const
    {
        ResizeUIResponsive(ApplicationView::GetForCurrentView().Id());
    }

    void AppLifecycleLogger::ResizeUIResponsive(int32_t viewId) const
    {
        if (!GetTraceLoggingProviderEnabled())
            return;

        LoggingFields fields{};
        PopulateAppInfo(fields);
        fields.AddInt32(L"ViewId", viewId);
        LogAppLifecycleEvent(L"ModernAppResize_UIResponsive", fields);
    }

    void AppLifecycleLogger::ResizeVisibleComplete() const
    {
        ResizeVisibleComplete(ApplicationView::GetForCurrentView().Id());
    }

    void AppLifecycleLogger::ResizeVisibleComplete(int32_t viewId) const
    {
        if (!GetTraceLoggingProviderEnabled())
            return;

        LoggingFields fields{};
        PopulateAppInfo(fields);
        fields.AddInt32(L"ViewId", viewId);
        LogAppLifecycleEvent(L"ModernAppResize_VisibleComplete", fields);
    }

    void AppLifecycleLogger::PopulateAppInfo(LoggingFields& fields) const
    {
        auto appId = CoreApplication::Id();
        auto aumId = Package::Current().Id().FamilyName() + L"!" + appId;
        auto packageFullName = Package::Current().Id().FullName();
        auto psmKey = Package::Current().Id().FullName() + L"+" + appId;

        fields.AddString(L"AumId", aumId);
        fields.AddString(L"PackageFullName", packageFullName);
        fields.AddString(L"PsmKey", psmKey);
    }
}
