// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "AboutFlyout.xaml.h"
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
#define BUILD_YEAR 2021
#endif

AboutFlyout::AboutFlyout()
{
    auto locService = LocalizationService::GetInstance();
    auto resourceLoader = AppResourceProvider::GetInstance();

    InitializeComponent();

    Language = locService->GetLanguage();

    this->SetVersionString();

    Header->Text = resourceLoader->GetResourceString("AboutButton/Content");

    auto copyrightText =
        LocalizationStringUtil::GetLocalizedString(resourceLoader->GetResourceString("AboutControlCopyright"), StringReference(to_wstring(BUILD_YEAR).c_str()));
    AboutControlCopyrightRun->Text = copyrightText;

    InitializeContributeTextBlock();
}

void AboutFlyout::FeedbackButton_Click(_In_ Object ^ sender, _In_ RoutedEventArgs ^ e)
{
    PackageVersion version = Package::Current->Id->Version;
    String ^ versionNumber = ref new String(L"Version ");
    versionNumber = versionNumber + version.Major + "." + version.Minor + "." + version.Build + "." + version.Revision;
    Launcher::LaunchUriAsync(ref new Uri("windows-feedback:?contextid=130&metadata=%7B%22Metadata%22:[%7B%22AppBuild%22:%22" + versionNumber + "%22%7D]%7D"));
}

void AboutFlyout::SetVersionString()
{
    PackageVersion version = Package::Current->Id->Version;
    String ^ appName = AppResourceProvider::GetInstance()->GetResourceString(L"AppName");
    AboutFlyoutVersion->Text = appName + L" " + version.Major + L"." + version.Minor + L"." + version.Build + L"." + version.Revision;
}

void AboutFlyout::SetDefaultFocus()
{
    AboutFlyoutEULA->Focus(::FocusState::Programmatic);
}

void AboutFlyout::InitializeContributeTextBlock()
{
    auto resProvider = AppResourceProvider::GetInstance();
    std::wstring contributeHyperlinkText = resProvider->GetResourceString(L"AboutFlyoutContribute")->Data();

    // The resource string has the 'GitHub' hyperlink wrapped with '%HL%'.
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
