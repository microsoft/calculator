// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "SettingsPage.xaml.h"
#include "CalcViewModel/Common/AppResourceProvider.h"
#include "CalcViewModel/Common/LocalizationService.h"
#include "CalcViewModel/Common/LocalizationStringUtil.h"
#include "CalcViewModel/Common/TraceLogger.h"

using namespace std;
using namespace CalculatorApp;
using namespace CalculatorApp::Common;
using namespace Platform;
using namespace Windows::ApplicationModel;
using namespace Windows::Foundation;
using namespace Windows::System;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;

#ifndef BUILD_YEAR
#define BUILD_YEAR 2020
#endif


SettingsPage::SettingsPage()
{
    auto locService = LocalizationService::GetInstance();
    auto resourceLoader = AppResourceProvider::GetInstance();

	InitializeComponent();

    Language = locService->GetLanguage();

    this->SetVersionString();
    this->SetCopyrightString();
}

void SettingsPage::SetVersionString()
{
    PackageVersion version = Package::Current->Id->Version;
    String ^ appName = AppResourceProvider::GetInstance()->GetResourceString(L"AppName");

    SettingsVersion->Text = appName + L" " + version.Major + L"." + version.Minor + L"." + version.Build + L"." + version.Revision;
}

void SettingsPage::SetCopyrightString()
{
    auto resourceLoader = AppResourceProvider::GetInstance();
    auto copyrightText = LocalizationStringUtil::GetLocalizedString(resourceLoader->GetResourceString("SettingsCopyright"), StringReference(to_wstring(BUILD_YEAR).c_str()));

    SettingsCopyright->Text = copyrightText;
}

void SettingsPage::ColorSettingsButtonClicked(_In_ Object ^ sender, _In_ RoutedEventArgs ^ e)
{
    Launcher::LaunchUriAsync(ref new Uri("ms-settings:colors"));
}

void SettingsPage::SettingsFeedbackButtonClick(_In_ Object ^ sender, _In_ RoutedEventArgs ^ e)
{
    PackageVersion version = Package::Current->Id->Version;
    String ^ versionNumber = ref new String(L"Version ");
    versionNumber = versionNumber + version.Major + version.Minor + version.Build + version.Revision;

    Launcher::LaunchUriAsync(ref new Uri("windows-feedback:?contextid=130&metadata=%7B%22Metadata%22:[%7B%22AppBuild%22:%22" + versionNumber + "%22%7D]%7D"));
}
