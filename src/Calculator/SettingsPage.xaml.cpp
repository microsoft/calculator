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

auto resourceLoader = AppResourceProvider::GetInstance();

SettingsPage::SettingsPage()
{
    auto locService = LocalizationService::GetInstance();

	InitializeComponent();

    SettingsRestartApp->Visibility = Windows::UI::Xaml::Visibility::Collapsed;

    Language = locService->GetLanguage();

    this->SetVersionString();
    this->SetCopyrightString();

    auto value = Windows::Storage::ApplicationData::Current->LocalSettings->Values->Lookup(L"themeSetting");
    if (value != nullptr)
    {
        String ^ colorSetting = safe_cast<String ^>(value);
        if (colorSetting == L"Dark")
        {
            SettingsDarkTheme->IsChecked = true;
        }
        else if (colorSetting == L"Light")
        {
            SettingsLightTheme->IsChecked = true;
        }
        else if (colorSetting == L"System")
        {
            SettingsSystemTheme->IsChecked = true;
        }
    }
}

void SettingsPage::SetVersionString()
{
    PackageVersion version = Package::Current->Id->Version;
    String ^ appName = AppResourceProvider::GetInstance()->GetResourceString(L"AppName");

    SettingsVersion->Text = appName + L" " + version.Major + L"." + version.Minor + L"." + version.Build + L"." + version.Revision;
}

void SettingsPage::SetCopyrightString()
{
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

void SettingsPage::LightChecked(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e)
{
    SettingsRestartApp->Visibility = Windows::UI::Xaml::Visibility::Visible;
    SettingsRestartApp->Text = LocalizationStringUtil::GetLocalizedString(resourceLoader->GetResourceString("SettingsRestartNotice"));
    Windows::Storage::ApplicationData::Current->LocalSettings->Values->Insert(L"themeSetting", ApplicationTheme::Light.ToString());
}

void SettingsPage::DarkChecked(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e)
{
    SettingsRestartApp->Visibility = Windows::UI::Xaml::Visibility::Visible;
    SettingsRestartApp->Text = LocalizationStringUtil::GetLocalizedString(resourceLoader->GetResourceString("SettingsRestartNotice"));
    Windows::Storage::ApplicationData::Current->LocalSettings->Values->Insert(L"themeSetting", ApplicationTheme::Dark.ToString());
}

void SettingsPage::SystemChecked(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e)
{
    SettingsRestartApp->Visibility = Windows::UI::Xaml::Visibility::Visible;
    SettingsRestartApp->Text = LocalizationStringUtil::GetLocalizedString(resourceLoader->GetResourceString("SettingsRestartNotice"));
    Windows::Storage::ApplicationData::Current->LocalSettings->Values->Insert(L"themeSetting", L"System");
}
