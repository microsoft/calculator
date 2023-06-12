//
// MyUserControl.xaml.h
// Declaration of the MyUserControl class
//


//using Graphing;
using CalculatorApp.ViewModel;
using CalculatorApp.ViewModel.Common;

using Windows.Storage;
using Windows.System;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Input;

namespace CalculatorApp
{
    [Windows.Foundation.Metadata.WebHostHidden]
    public sealed partial class GraphingSettings
    {
        public GraphingSettings()
        {
            ViewModel = new GraphingSettingsViewModel();
            m_IsMatchAppTheme = false;
            InitializeComponent();
        }

        public CalculatorApp.ViewModel.GraphingSettingsViewModel ViewModel { get; set; }

        public bool IsMatchAppTheme
        {
            get => m_IsMatchAppTheme;
            set
            {
                if (m_IsMatchAppTheme == value)
                {
                    return;
                }

                m_IsMatchAppTheme = value;
                SetGraphTheme(m_IsMatchAppTheme);
            }
        }

        public Style SelectTextBoxStyle(bool incorrectRange, bool error)
        {
            if ((incorrectRange || error) && this.Resources.ContainsKey("ErrorTextBoxStyle"))
            {
                return (Style)(this.Resources["ErrorTextBoxStyle"]);
            }
            else
            {
                return null;
            }
        }

        public void SetGrapher(GraphControl.Grapher grapher)
        {
            ViewModel.SetGrapher(grapher);
        }

        public void RefreshRanges()
        {
            ViewModel.ResetView();
        }

        public static string GetLineWidthAutomationName(double width)
        {
            var resourceLoader = AppResourceProvider.GetInstance();

            if (width == 1.0)
            {
                return resourceLoader.GetResourceString("SmallLineWidthAutomationName");
            }
            else if (width == 2.0)
            {
                return resourceLoader.GetResourceString("MediumLineWidthAutomationName");
            }
            else if (width == 3.0)
            {
                return resourceLoader.GetResourceString("LargeLineWidthAutomationName");
            }
            else
            {
                return resourceLoader.GetResourceString("ExtraLargeLineWidthAutomationName");
            }
        }

        // Event sends the if the IsMatchAppTheme is selected
        public event System.EventHandler<bool> GraphThemeSettingChanged;

        private void GridSettingsTextBox_PreviewKeyDown(object sender, KeyRoutedEventArgs e)
        {
            if (e.Key == VirtualKey.Enter)
            {
                if (FocusManager.TryMoveFocusAsync(FocusNavigationDirection.Next) == null)
                {
                    _ = FocusManager.TryMoveFocusAsync(FocusNavigationDirection.Previous);
                }
                e.Handled = true;
            }
        }

        private void ResetViewButton_Clicked(object sender, RoutedEventArgs e)
        {
            ViewModel.ResetView();
        }

        private void SetGraphTheme(bool isMatchAppTheme)
        {
            string propertyName = isMatchAppTheme ? "IsMatchAppTheme" : "IsAlwaysLightTheme";
            ApplicationDataContainer localSettings = ApplicationData.Current.LocalSettings;
            localSettings.Values["IsGraphThemeMatchApp"] = isMatchAppTheme;
            GraphThemeSettingChanged?.Invoke(this, isMatchAppTheme);
            CalculatorApp.ViewModel.Common.TraceLogger.GetInstance().LogGraphSettingsChanged(GraphSettingsType.Theme, propertyName);
        }

        private bool m_IsMatchAppTheme;
    };
}
