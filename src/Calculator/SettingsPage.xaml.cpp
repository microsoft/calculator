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
using namespace winrt::Windows::Storage;

#ifndef BUILD_YEAR
#define BUILD_YEAR 2021
#endif

SettingsPage::SettingsPage()
{
    auto resourceLoader = AppResourceProvider::GetInstance();
    auto themevalue = Windows::Storage::ApplicationData::Current->LocalSettings->Values->Lookup(L"themeSetting");
    auto restartValue = Windows::Storage::ApplicationData::Current->LocalSettings->Values->Lookup(L"restartApp");
    String ^ colorSetting = safe_cast<String ^>(themevalue);
    String ^ restartApp = safe_cast<String ^>(restartValue);

    InitializeComponent();

    this->SetVersionString();
    this->SetCopyrightString();
    this->InitializeContributeTextBlock();
    SetDefaultFocus(); 
    
    if (themevalue != nullptr)
    {
        if (colorSetting == L"Light")
        {
            SettingsLightTheme->IsChecked = true;
            m_currentTheme = "Light";
        }
        else if (colorSetting == L"Dark")
        {
            SettingsDarkTheme->IsChecked = true;
            m_currentTheme = "Dark";
        }
        else if (colorSetting == L"System")
        {
            SettingsSystemTheme->IsChecked = true;
            m_currentTheme = "System";
        }
    }
    else
    {
        SettingsSystemTheme->IsChecked = true;
        m_currentTheme = "System";
    }

    if (restartValue != nullptr)
    {
        if (restartApp == L"True")
        {
            SettingsRestartApp->Visibility = ::Visibility::Visible;
        }
        else if (restartApp == L"False")
        {
            SettingsRestartApp->Visibility = ::Visibility::Collapsed;
        }
    }
    else
    {
        SettingsRestartApp->Visibility = ::Visibility::Collapsed;
    }
}

void SettingsPage::OnNavigatedTo(Windows::UI::Xaml::Navigation::NavigationEventArgs ^ e)
{
    MainPage ^ mainPage = dynamic_cast<MainPage ^> (e->Parameter);
    if (mainPage != nullptr)
    {
        MainPageProperty = mainPage;
    }
}

void SettingsPage::SetDefaultFocus()
{
    SettingsLightTheme->Focus(::FocusState::Programmatic);
}

void SettingsPage::InitializeContributeTextBlock()
{
    auto resourceLoader = AppResourceProvider::GetInstance();
    std::wstring contributeHyperlinkText = resourceLoader->GetResourceString(L"SettingsContribute")->Data();

    // The resource string has '%HL%' wrapped around 'GitHub'
    // Break the string and assign pieces appropriately.
    static const std::wstring delimiter{ L"%HL%" };
    static const size_t delimiterLength{ delimiter.length() };

    // Find the delimiters.
    size_t firstSplitPosition = contributeHyperlinkText.find(delimiter, 0);
    assert(firstSplitPosition != std::wstring::npos);
    size_t secondSplitPosition = contributeHyperlinkText.find(delimiter, firstSplitPosition + 1);
    assert(secondSplitPosition != std::wstring::npos);
    size_t hyperlinkTextLength = secondSplitPosition - (firstSplitPosition + delimiterLength);

    // Assign pieces.
    auto contributeTextBeforeHyperlink = ref new String(contributeHyperlinkText.substr(0, firstSplitPosition).c_str());
    auto contributeTextLink = ref new String(contributeHyperlinkText.substr(firstSplitPosition + delimiterLength, hyperlinkTextLength).c_str());
    auto contributeTextAfterHyperlink = ref new String(contributeHyperlinkText.substr(secondSplitPosition + delimiterLength).c_str());

    ContributeRunBeforeLink->Text = contributeTextBeforeHyperlink;
    ContributeRunLink->Text = contributeTextLink;
    ContributeRunAfterLink->Text = contributeTextAfterHyperlink;

}

void SettingsPage::BackButtonClick(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e)
{
    if (SettingsRestartApp->Visibility == ::Visibility::Visible)
    {
        Windows::Storage::ApplicationData::Current->LocalSettings->Values->Insert(L"restartApp", L"True");
    }
    else
    {
        Windows::Storage::ApplicationData::Current->LocalSettings->Values->Insert(L"restartApp", L"False");
    }
    MainPageProperty->CollapseSettings();
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

void SettingsPage::ThemeChecked(Platform::Object ^ sender, Windows::UI::Xaml::RoutedEventArgs ^ e)
{
    auto resourceLoader = AppResourceProvider::GetInstance();
    RadioButton ^ radioButton = safe_cast<RadioButton ^>(sender);
    String ^ tag = radioButton->Tag->ToString();

    if (m_currentTheme == tag)
    {
        SettingsRestartApp->Visibility = ::Visibility::Collapsed;
    }
    else
    {
        SettingsRestartApp->Visibility = ::Visibility::Visible;
        SettingsRestartApp->Text = resourceLoader->GetResourceString("SettingsRestartNotice");
    }

    if (tag == "Light")
    {
        Windows::Storage::ApplicationData::Current->LocalSettings->Values->Insert(L"themeSetting", ApplicationTheme::Light.ToString());
    }
    else if (tag == "Dark")
    {
        Windows::Storage::ApplicationData::Current->LocalSettings->Values->Insert(L"themeSetting", ApplicationTheme::Dark.ToString());
    }
    else if (tag == "System")
    {
        Windows::Storage::ApplicationData::Current->LocalSettings->Values->Insert(L"themeSetting", L"System");
    }
}
