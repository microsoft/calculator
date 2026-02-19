using CalculatorApp.Utils;
using CalculatorApp.ViewModel.Common;
using CalculatorApp.ViewModel.Common.Automation;

using System;
using System.Diagnostics;
using System.Linq;

using Windows.ApplicationModel;
using Windows.System;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Automation.Peers;
using Windows.UI.Xaml.Automation.Provider;
using Windows.UI.Xaml.Controls;

// The User Control item template is documented at https://go.microsoft.com/fwlink/?LinkId=234236

namespace CalculatorApp
{
    public sealed partial class Settings : UserControl
    {
        private const string BUILD_YEAR = "2025";

        public event Windows.UI.Xaml.RoutedEventHandler BackButtonClick;

        public GridLength TitleBarHeight
        {
            get => (GridLength)GetValue(TitleBarHeightProperty);
            set => SetValue(TitleBarHeightProperty, value);
        }
        public static readonly DependencyProperty TitleBarHeightProperty =
            DependencyProperty.Register(nameof(TitleBarHeight), typeof(GridLength), typeof(Settings), new PropertyMetadata(default(GridLength)));

        public Settings()
        {
            var locService = LocalizationService.GetInstance();
            var resourceLoader = AppResourceProvider.GetInstance();

            InitializeComponent();

            Language = locService.GetLanguage();

            InitializeAboutContentTextBlock();

            var copyrightText =
                LocalizationStringUtil.GetLocalizedString(resourceLoader.GetResourceString("AboutControlCopyright"), BUILD_YEAR);
            AboutExpander.Description = copyrightText;

            InitializeContributeTextBlock();
        }

        private void OnThemeSelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (e.AddedItems.Count > 0 && e.AddedItems[0] is RadioButton selectItem)
            {
                ThemeHelper.RootTheme = ThemeHelper.GetEnum<ElementTheme>(selectItem.Tag.ToString());
            }
        }

        public void SetDefaultFocus()
        {
            AppThemeExpander.Focus(FocusState.Programmatic);
        }

        // OnLoaded would be invoked by Popup several times while contructed once
        private void OnLoaded(object sender, RoutedEventArgs args)
        {
            SystemNavigationManager.GetForCurrentView().BackRequested += System_BackRequested;

            AnnouncePageOpened();

            var currentTheme = ThemeHelper.RootTheme.ToString();
            (ThemeRadioButtons.Items.Cast<RadioButton>().FirstOrDefault(c => c?.Tag?.ToString() == currentTheme)).IsChecked = true;

            SetDefaultFocus();
        }

        private void AnnouncePageOpened()
        {
            string announcementText = AppResourceProvider.GetInstance().GetResourceString("SettingsPageOpenedAnnouncement");
            NarratorAnnouncement announcement = CalculatorAnnouncement.GetSettingsPageOpenedAnnouncement(announcementText);
            NarratorNotifier.Announce(announcement);
        }

        // OnUnloaded would be invoked by Popup several times while contructed once
        private void OnUnloaded(object sender, RoutedEventArgs e)
        {
            // back to the default state
            AppThemeExpander.IsExpanded = false;

            SystemNavigationManager.GetForCurrentView().BackRequested -= System_BackRequested;
        }

        private void FeedbackButton_Click(object sender, RoutedEventArgs e)
        {
            PackageVersion version = Package.Current.Id.Version;
            string versionNumber = "Version ";
            versionNumber = versionNumber + version.Major + "." + version.Minor + "." + version.Build + "." + version.Revision;
            _ = Launcher.LaunchUriAsync(new Uri("windows-feedback:?contextid=130&metadata=%7B%22Metadata%22:[%7B%22AppBuild%22:%22" + versionNumber + "%22%7D]%7D"));
        }

        private void InitializeAboutContentTextBlock()
        {
            SetVersionString();
        }

        private void SetVersionString()
        {
            PackageVersion version = Package.Current.Id.Version;
            AboutExpander.Header = AppResourceProvider.GetInstance().GetResourceString("AppName");
            AboutBuildVersion.Text = version.Major + "." + version.Minor + "." + version.Build + "." + version.Revision;
        }

        private void InitializeContributeTextBlock()
        {
            var resProvider = AppResourceProvider.GetInstance();
            string contributeHyperlinkText = resProvider.GetResourceString("AboutControlContribute");

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

        private void System_BackRequested(object sender, BackRequestedEventArgs e)
        {
            if (!e.Handled && BackButton.IsEnabled)
            {
                var buttonPeer = new ButtonAutomationPeer(BackButton);
                IInvokeProvider invokeProvider = buttonPeer.GetPattern(PatternInterface.Invoke) as IInvokeProvider;
                invokeProvider.Invoke();

                e.Handled = true;
            }
        }

        private void BackButton_Click(object sender, RoutedEventArgs e)
        {
            BackButtonClick?.Invoke(this, e);
        }
    }
}
