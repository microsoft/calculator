using CalculatorApp.ViewModel.Common;
using System;
using Windows.ApplicationModel.Core;
using Windows.System.Profile;
using Windows.UI;
using Windows.UI.Core;
using Windows.UI.ViewManagement;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;

// The User Control item template is documented at https://go.microsoft.com/fwlink/?LinkId=234236

namespace CalculatorApp
{
    public sealed partial class TitleBar : UserControl
    {
        public TitleBar()
        {
            m_coreTitleBar = CoreApplication.GetCurrentView().TitleBar;
            m_uiSettings = new UISettings();
            m_accessibilitySettings = new AccessibilitySettings();
            InitializeComponent();

            m_coreTitleBar.ExtendViewIntoTitleBar = true;
            Window.Current.SetTitleBar(BackgroundElement);

            Loaded += OnLoaded;
            Unloaded += OnUnloaded;
#if IS_STORE_BUILD
            AppName.Text = AppResourceProvider.GetInstance().GetResourceString("AppName");

#else
            AppName.Text = AppResourceProvider.GetInstance().GetResourceString("DevAppName");
#endif
        }

        public bool IsAlwaysOnTopMode
        {
            get { return (bool)GetValue(IsAlwaysOnTopModeProperty); }
            set { SetValue(IsAlwaysOnTopModeProperty, value); }
        }

        // Using a DependencyProperty as the backing store for IsAlwaysOnTopMode.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty IsAlwaysOnTopModeProperty =
            DependencyProperty.Register(nameof(IsAlwaysOnTopMode), typeof(bool), typeof(TitleBar), new PropertyMetadata(default(bool), (sender, args) =>
            {
                var self = (TitleBar)sender;
                self.OnIsAlwaysOnTopModePropertyChanged((bool)args.OldValue, (bool)args.NewValue);
            }));

        public event Windows.UI.Xaml.RoutedEventHandler AlwaysOnTopClick;

        private void OnLoaded(object sender, RoutedEventArgs e)
        {
            // Register events
            m_coreTitleBar.IsVisibleChanged += CoreTitleBarIsVisibleChanged;
            m_coreTitleBar.LayoutMetricsChanged += CoreTitleBarLayoutMetricsChanged;

            m_uiSettings.ColorValuesChanged += ColorValuesChanged;
            m_accessibilitySettings.HighContrastChanged += OnHighContrastChanged;
            Window.Current.Activated += OnWindowActivated;

            // Set properties
            SetTitleBarControlColors();
            SetTitleBarHeightAndPadding();

            // As of Windows 10 1903: when an app runs on a PC (without Tablet mode activated)
            // properties of CoreApplicationViewTitleBar aren't initialized during the first seconds after launch.
            var forceDisplay = AnalyticsInfo.VersionInfo.DeviceFamily == "Windows.Desktop"
                && UIViewSettings.GetForCurrentView().UserInteractionMode == UserInteractionMode.Mouse;

            SetTitleBarVisibility(forceDisplay);
        }

        private void OnUnloaded(object sender, RoutedEventArgs e)
        {
            // Unregister events
            m_coreTitleBar.LayoutMetricsChanged -= CoreTitleBarLayoutMetricsChanged;
            m_coreTitleBar.IsVisibleChanged -= CoreTitleBarIsVisibleChanged;
            m_uiSettings.ColorValuesChanged -= ColorValuesChanged;
            m_accessibilitySettings.HighContrastChanged -= OnHighContrastChanged;
            Window.Current.Activated -= OnWindowActivated;
        }

        private void CoreTitleBarIsVisibleChanged(CoreApplicationViewTitleBar cTitleBar, object args)
        {
            SetTitleBarVisibility(false);
        }

        private void CoreTitleBarLayoutMetricsChanged(CoreApplicationViewTitleBar cTitleBar, object args)
        {
            SetTitleBarHeightAndPadding();
        }

        private void SetTitleBarVisibility(bool forceDisplay)
        {
            LayoutRoot.Visibility =
                forceDisplay || m_coreTitleBar.IsVisible || IsAlwaysOnTopMode ? Visibility.Visible : Visibility.Collapsed;
        }

        private void SetTitleBarHeightAndPadding()
        {
            if (m_coreTitleBar.Height == 0)
            {
                // The titlebar isn't init
                return;
            }

            double leftAddition = 0;
            double rightAddition = 0;

            if (FlowDirection == FlowDirection.LeftToRight)
            {
                leftAddition = m_coreTitleBar.SystemOverlayLeftInset;
                rightAddition = m_coreTitleBar.SystemOverlayRightInset;
            }
            else
            {
                leftAddition = m_coreTitleBar.SystemOverlayRightInset;
                leftAddition = m_coreTitleBar.SystemOverlayLeftInset;
            }

            LayoutRoot.Padding = new Thickness(leftAddition, 0, rightAddition, 0);
            LayoutRoot.Height = m_coreTitleBar.Height;
        }

        private void ColorValuesChanged(Windows.UI.ViewManagement.UISettings sender, object e)
        {
            _ = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, new DispatchedHandler(() => { SetTitleBarControlColors(); }));
        }

        private void SetTitleBarControlColors()
        {
            var applicationView = ApplicationView.GetForCurrentView();
            if (applicationView == null)
            {
                return;
            }

            var applicationTitleBar = applicationView.TitleBar;
            if (applicationTitleBar == null)
            {
                return;
            }

            if (m_accessibilitySettings.HighContrast)
            {
                // Reset to use default colors.
                applicationTitleBar.ButtonBackgroundColor = null;
                applicationTitleBar.ButtonForegroundColor = null;
                applicationTitleBar.ButtonInactiveBackgroundColor = null;
                applicationTitleBar.ButtonInactiveForegroundColor = null;
                applicationTitleBar.ButtonHoverBackgroundColor = null;
                applicationTitleBar.ButtonHoverForegroundColor = null;
                applicationTitleBar.ButtonPressedBackgroundColor = null;
                applicationTitleBar.ButtonPressedForegroundColor = null;
            }
            else
            {
                Color bgColor = Colors.Transparent;
                Color fgColor = ((SolidColorBrush)Application.Current.Resources["SystemControlPageTextBaseHighBrush"]).Color;
                Color inactivefgColor =
                    ((SolidColorBrush)Application.Current.Resources["SystemControlForegroundChromeDisabledLowBrush"]).Color;
                Color hoverbgColor = ((SolidColorBrush)Application.Current.Resources["SystemControlBackgroundListLowBrush"]).Color;
                Color hoverfgColor = ((SolidColorBrush)Application.Current.Resources["SystemControlForegroundBaseHighBrush"]).Color;
                Color pressedbgColor = ((SolidColorBrush)Application.Current.Resources["SystemControlBackgroundListMediumBrush"]).Color;
                Color pressedfgCoolor = ((SolidColorBrush)Application.Current.Resources["SystemControlForegroundBaseHighBrush"]).Color;
                applicationTitleBar.ButtonBackgroundColor = bgColor;
                applicationTitleBar.ButtonForegroundColor = fgColor;
                applicationTitleBar.ButtonInactiveBackgroundColor = bgColor;
                applicationTitleBar.ButtonInactiveForegroundColor = inactivefgColor;
                applicationTitleBar.ButtonHoverBackgroundColor = hoverbgColor;
                applicationTitleBar.ButtonHoverForegroundColor = hoverfgColor;
                applicationTitleBar.ButtonPressedBackgroundColor = pressedbgColor;
                applicationTitleBar.ButtonPressedForegroundColor = pressedfgCoolor;
            }
        }

        private void OnHighContrastChanged(Windows.UI.ViewManagement.AccessibilitySettings sender, object args)
        {
            _ = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, new DispatchedHandler(() =>
            {
                SetTitleBarControlColors();
                SetTitleBarVisibility(false);
            }));
        }

        private void OnWindowActivated(object sender, WindowActivatedEventArgs e)
        {
            VisualStateManager.GoToState(
                this, e.WindowActivationState == CoreWindowActivationState.Deactivated ? WindowNotFocused.Name : WindowFocused.Name, false);
        }

        private void OnIsAlwaysOnTopModePropertyChanged(bool oldValue, bool newValue)
        {
            SetTitleBarVisibility(false);
            VisualStateManager.GoToState(this, newValue ? "AOTMiniState" : "AOTNormalState", false);
        }

        private void AlwaysOnTopButton_Click(object sender, RoutedEventArgs e)
        {
            AlwaysOnTopClick?.Invoke(this, e);
        }

        private Windows.ApplicationModel.Core.CoreApplicationViewTitleBar m_coreTitleBar;
        private Windows.UI.ViewManagement.UISettings m_uiSettings;
        private Windows.UI.ViewManagement.AccessibilitySettings m_accessibilitySettings;
    }
}
