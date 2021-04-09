// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using CalculatorApp.Common;
using System;
using System.Diagnostics;
using Windows.ApplicationModel;
using Windows.System;
using Windows.UI.Xaml;

namespace CalculatorApp
{
    public sealed partial class AboutFlyout
    {
        // CSHARP_MIGRATION: TODO:
        // BUILD_YEAR was a C++/CX macro and may update the value from the pipeline
        private const string BUILD_YEAR = "2021";

        public AboutFlyout()
        {
            var locService = LocalizationService.GetInstance();
            var resourceLoader = AppResourceProvider.GetInstance();

            InitializeComponent();

            Language = locService.GetLanguage();

            SetVersionString();

            Header.Text = resourceLoader.GetResourceString("AboutButton/Content");

            var copyrightText =
                LocalizationStringUtil.GetLocalizedString(resourceLoader.GetResourceString("AboutControlCopyright"), BUILD_YEAR);
            AboutControlCopyrightRun.Text = copyrightText;

            InitializeContributeTextBlock();
        }

        public void SetDefaultFocus()
        {
            AboutFlyoutEULA.Focus(FocusState.Programmatic);
        }

        private void FeedbackButton_Click(object sender, RoutedEventArgs e)
        {
            PackageVersion version = Package.Current.Id.Version;
            string versionNumber = "Version ";
            versionNumber = versionNumber + version.Major + "." + version.Minor + "." + version.Build + "." + version.Revision;
            _ = Launcher.LaunchUriAsync(new Uri("windows-feedback:?contextid=130&metadata=%7B%22Metadata%22:[%7B%22AppBuild%22:%22" + versionNumber + "%22%7D]%7D"));
        }

        private void SetVersionString()
        {
            PackageVersion version = Package.Current.Id.Version;
            string appName = AppResourceProvider.GetInstance().GetResourceString("AppName");
            AboutFlyoutVersion.Text = appName + " " + version.Major + "." + version.Minor + "." + version.Build + "." + version.Revision;
        }

        private void InitializeContributeTextBlock()
        {
            var resProvider = AppResourceProvider.GetInstance();
            string contributeHyperlinkText = resProvider.GetResourceString("AboutFlyoutContribute");

            // The resource string has the 'GitHub' hyperlink wrapped with '%HL%'.
            // Break the string and assign pieces appropriately.
            string delimiter = "%HL%";
            int delimiterLength = delimiter.Length;

            // Find the delimiters.
            int firstSplitPosition = contributeHyperlinkText.IndexOf(delimiter, 0);
            Debug.Assert(firstSplitPosition != -1);
            int secondSplitPosition = contributeHyperlinkText.IndexOf(delimiter, firstSplitPosition + 1);
            Debug.Assert(secondSplitPosition != -1);
            int hyperlinkTextLength = secondSplitPosition - (firstSplitPosition + delimiterLength);

            // Assign pieces.
            var contributeTextBeforeHyperlink = contributeHyperlinkText.Substring(0, firstSplitPosition);
            var contributeTextLink = contributeHyperlinkText.Substring(firstSplitPosition + delimiterLength, hyperlinkTextLength);
            var contributeTextAfterHyperlink = contributeHyperlinkText.Substring(secondSplitPosition + delimiterLength);

            ContributeRunBeforeLink.Text = contributeTextBeforeHyperlink;
            ContributeRunLink.Text = contributeTextLink;
            ContributeRunAfterLink.Text = contributeTextAfterHyperlink;
        }
    }
}
