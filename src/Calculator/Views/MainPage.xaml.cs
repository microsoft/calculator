using CalculatorApp.Common;
using CalculatorApp.Converters;
using CalculatorApp.ViewModel;
using CalculatorApp.ViewModel.Common;
using CalculatorApp.ViewModel.Common.Automation;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.Graphics.Display;
using Windows.Storage;
using Windows.UI.Core;
using Windows.UI.ViewManagement;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Automation;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Navigation;
using MUXC = Microsoft.UI.Xaml.Controls;

namespace CalculatorApp
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page
    {
        public MainPage()
        {
            m_model = new ViewModel.ApplicationViewModel();
            InitializeComponent();

            KeyboardShortcutManager.Initialize();

            Application.Current.Suspending += App_Suspending;
            m_model.PropertyChanged += OnAppPropertyChanged;
            m_accessibilitySettings = new AccessibilitySettings();

            if(Utilities.GetIntegratedDisplaySize(out var sizeInInches))
            {
                if(sizeInInches < 7.0) // If device's display size (diagonal length) is less than 7 inches then keep the calc always in Portrait mode only
                {
                    DisplayInformation.AutoRotationPreferences = DisplayOrientations.Portrait | DisplayOrientations.PortraitFlipped;
                }
            }
        }

        public CalculatorApp.ViewModel.ApplicationViewModel Model
        {
            get => m_model;
        }

        public void UnregisterEventHandlers()
        {
            Window.Current.SizeChanged -= WindowSizeChanged;
            m_accessibilitySettings.HighContrastChanged -= OnHighContrastChanged;

            if (m_calculator != null)
            {
                m_calculator.UnregisterEventHandlers();
            }
        }

        public void SetDefaultFocus()
        {
            if (m_calculator != null && m_calculator.Visibility == Visibility.Visible)
            {
                m_calculator.SetDefaultFocus();
            }
            if (m_dateCalculator != null && m_dateCalculator.Visibility == Visibility.Visible)
            {
                m_dateCalculator.SetDefaultFocus();
            }
            if (m_graphingCalculator != null && m_graphingCalculator.Visibility == Visibility.Visible)
            {
                m_graphingCalculator.SetDefaultFocus();
            }
            if (m_converter != null && m_converter.Visibility == Visibility.Visible)
            {
                m_converter.SetDefaultFocus();
            }
        }

        public void SetHeaderAutomationName()
        {
            ViewMode mode = m_model.Mode;
            var resProvider = AppResourceProvider.GetInstance();

            string name;
            if (NavCategory.IsDateCalculatorViewMode(mode))
            {
                name = resProvider.GetResourceString("HeaderAutomationName_Date");
            }
            else
            {
                string full = string.Empty;
                if (NavCategory.IsCalculatorViewMode(mode) || NavCategory.IsGraphingCalculatorViewMode(mode))
                {
                    full = resProvider.GetResourceString("HeaderAutomationName_Calculator");
                }
                else if (NavCategory.IsConverterViewMode(mode))
                {
                    full = resProvider.GetResourceString("HeaderAutomationName_Converter");
                }
                name = LocalizationStringUtil.GetLocalizedString(full, m_model.CategoryName);
            }

            AutomationProperties.SetName(Header, name);
        }

        public ObservableCollection<object> CreateUIElementsForCategories(IObservableVector<NavCategoryGroup> categories)
        {
            var menuCategories = new ObservableCollection<object>();

            foreach (var group in categories)
            {
                menuCategories.Add(CreateNavViewHeaderFromGroup(group));

                foreach (var category in group.Categories)
                {
                    menuCategories.Add(CreateNavViewItemFromCategory(category));
                }
            }

            return menuCategories;
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            ViewMode initialMode = ViewMode.Standard;

            string stringParameter = (e.Parameter as string);
            if (!string.IsNullOrEmpty(stringParameter))
            {
                initialMode = (ViewMode)Convert.ToInt32(stringParameter);
            }
            else
            {
                ApplicationDataContainer localSettings = ApplicationData.Current.LocalSettings;
                if (localSettings.Values.ContainsKey(ApplicationViewModel.ModePropertyName))
                {
                    initialMode = NavCategory.Deserialize(localSettings.Values[ApplicationViewModel.ModePropertyName]);
                }
            }

            m_model.Initialize(initialMode);
        }

        private void UpdatePopupSize(Windows.UI.Core.WindowSizeChangedEventArgs e)
        {
            if(PopupContent != null)
            {
                PopupContent.Width = e.Size.Width;
                PopupContent.Height = e.Size.Height;
            }
        }

        private void WindowSizeChanged(object sender, Windows.UI.Core.WindowSizeChangedEventArgs e)
        {
            // We don't use layout aware page's view states, we have our own
            UpdateViewState();
            UpdatePopupSize(e);
        }

        private void OnAppPropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            string propertyName = e.PropertyName;
            if (propertyName == ApplicationViewModel.ModePropertyName)
            {
                ViewMode newValue = m_model.Mode;
                ViewMode previousMode = m_model.PreviousMode;

                KeyboardShortcutManager.DisableShortcuts(false);

                if (newValue == ViewMode.Standard)
                {
                    EnsureCalculator();
                    m_model.CalculatorViewModel.HistoryVM.AreHistoryShortcutsEnabled = true;
                    m_calculator.AnimateCalculator(NavCategory.IsConverterViewMode(previousMode));
                    m_model.CalculatorViewModel.HistoryVM.ReloadHistory(newValue);
                }
                else if (newValue == ViewMode.Scientific)
                {
                    EnsureCalculator();
                    m_model.CalculatorViewModel.HistoryVM.AreHistoryShortcutsEnabled = true;
                    if (m_model.PreviousMode != ViewMode.Scientific)
                    {
                        m_calculator.AnimateCalculator(NavCategory.IsConverterViewMode(previousMode));
                    }

                    m_model.CalculatorViewModel.HistoryVM.ReloadHistory(newValue);
                }
                else if (newValue == ViewMode.Programmer)
                {
                    m_model.CalculatorViewModel.HistoryVM.AreHistoryShortcutsEnabled = false;
                    EnsureCalculator();
                    if (m_model.PreviousMode != ViewMode.Programmer)
                    {
                        m_calculator.AnimateCalculator(NavCategory.IsConverterViewMode(previousMode));
                    }
                }
                else if (NavCategory.IsDateCalculatorViewMode(newValue))
                {
                    if (m_model.CalculatorViewModel != null)
                    {
                        m_model.CalculatorViewModel.HistoryVM.AreHistoryShortcutsEnabled = false;
                    }
                    EnsureDateCalculator();
                }
                else if (newValue == ViewMode.Graphing)
                {
                    EnsureGraphingCalculator();
                    KeyboardShortcutManager.DisableShortcuts(true);
                }
                else if (NavCategory.IsConverterViewMode(newValue))
                {
                    if (m_model.CalculatorViewModel != null)
                    {
                        m_model.CalculatorViewModel.HistoryVM.AreHistoryShortcutsEnabled = false;
                    }

                    EnsureConverter();
                    if (!NavCategory.IsConverterViewMode(previousMode))
                    {
                        m_converter.AnimateConverter();
                    }
                }

                ShowHideControls(newValue);

                UpdateViewState();
                SetDefaultFocus();
            }
            else if (propertyName == ApplicationViewModel.CategoryNamePropertyName)
            {
                SetHeaderAutomationName();
                AnnounceCategoryName();
            }
        }

        private void SelectNavigationItemByModel()
        {
            var menuItems = ((ObservableCollection<object>)NavView.MenuItemsSource);
            var itemCount = ((int)menuItems.Count);
            var flatIndex = NavCategory.GetFlatIndex(Model.Mode);

            if (flatIndex >= 0 && flatIndex < itemCount)
            {
                NavView.SelectedItem = menuItems[flatIndex];
            }
        }

        private void OnNavLoaded(object sender, RoutedEventArgs e)
        {
            if (NavView.SelectedItem == null)
            {
                SelectNavigationItemByModel();
            }

            var acceleratorList = new List<MyVirtualKey>();
            NavCategory.GetCategoryAcceleratorKeys(acceleratorList);

            foreach (var accelerator in acceleratorList)
            {
                NavView.SetValue(KeyboardShortcutManager.VirtualKeyAltChordProperty, accelerator);
            }
            // Special case logic for Ctrl+E accelerator for Date Calculation Mode
            NavView.SetValue(KeyboardShortcutManager.VirtualKeyControlChordProperty, MyVirtualKey.E);
        }

        private void OnNavPaneOpened(MUXC.NavigationView sender, object args)
        {
            KeyboardShortcutManager.HonorShortcuts(false);
            TraceLogger.GetInstance().LogNavBarOpened();
        }

        private void OnNavPaneClosed(MUXC.NavigationView sender, object args)
        {
            if (Popup.IsOpen)
            {
                return;
            }

            if (Model.Mode != ViewMode.Graphing)
            {
                KeyboardShortcutManager.HonorShortcuts(true);
            }

            SetDefaultFocus();
        }

        private void EnsurePopupContent()
        {
            if (PopupContent == null)
            {
                FindName("PopupContent");

                var windowBounds = Window.Current.Bounds;
                PopupContent.Width = windowBounds.Width;
                PopupContent.Height = windowBounds.Height;
            }
        }

        private void ShowSettingsPopup()
        {
            EnsurePopupContent();
            Popup.IsOpen = true;
        }

        private void CloseSettingsPopup()
        {
            Popup.IsOpen = false;
            SelectNavigationItemByModel();
            SetDefaultFocus();
        }

        private void Popup_Opened(object sender, object e)
        {
            KeyboardShortcutManager.IgnoreEscape(false);
            KeyboardShortcutManager.HonorShortcuts(false);
        }

        private void Popup_Closed(object sender, object e)
        {
            KeyboardShortcutManager.HonorEscape();
            KeyboardShortcutManager.HonorShortcuts(!NavView.IsPaneOpen);
        }

        private void OnNavSelectionChanged(object sender, MUXC.NavigationViewSelectionChangedEventArgs e)
        {
            if(e.IsSettingsSelected)
            {
                ShowSettingsPopup();
                return;
            }

            var item = (e.SelectedItemContainer as MUXC.NavigationViewItem);
            if (item != null)
            {
                var selectedItem = ((NavCategory)item.DataContext);
                Model.Mode = selectedItem.Mode;
            }
        }

        private void OnNavItemInvoked(MUXC.NavigationView sender, MUXC.NavigationViewItemInvokedEventArgs e)
        {
            NavView.IsPaneOpen = false;
        }

        private void AlwaysOnTopButtonClick(object sender, RoutedEventArgs e)
        {
            Model.ToggleAlwaysOnTop(0, 0);
        }

        private void TitleBarAlwaysOnTopButtonClick(object sender, RoutedEventArgs e)
        {
            var bounds = Window.Current.Bounds;
            Model.ToggleAlwaysOnTop((float)bounds.Width, (float)bounds.Height);
        }

        private MUXC.NavigationViewItemHeader CreateNavViewHeaderFromGroup(NavCategoryGroup group)
        {
            var header = new MUXC.NavigationViewItemHeader();
            header.DataContext = group;

            header.Content = group.Name;
            AutomationProperties.SetName(header, group.AutomationName);
            AutomationProperties.SetHeadingLevel(header, Windows.UI.Xaml.Automation.Peers.AutomationHeadingLevel.Level1);

            return header;
        }

        private MUXC.NavigationViewItem CreateNavViewItemFromCategory(NavCategory category)
        {
            var item = new MUXC.NavigationViewItem();
            item.DataContext = category;

            var icon = new FontIcon();
            icon.FontFamily = (Windows.UI.Xaml.Media.FontFamily)(App.Current.Resources["CalculatorFontFamily"]);
            icon.Glyph = category.Glyph;
            item.Icon = icon;

            item.Content = category.Name;
            item.AccessKey = category.AccessKey;
            item.IsEnabled = category.IsEnabled;
            item.Style = (Windows.UI.Xaml.Style)(Resources["NavViewItemStyle"]);

            AutomationProperties.SetName(item, category.AutomationName);
            AutomationProperties.SetAutomationId(item, category.AutomationId);

            return item;
        }

        private void ShowHideControls(ViewMode mode)
        {
            var isCalcViewMode = NavCategory.IsCalculatorViewMode(mode);
            var isDateCalcViewMode = NavCategory.IsDateCalculatorViewMode(mode);
            var isGraphingCalcViewMode = NavCategory.IsGraphingCalculatorViewMode(mode);
            var isConverterViewMode = NavCategory.IsConverterViewMode(mode);

            if (m_calculator != null)
            {
                m_calculator.Visibility = BooleanToVisibilityConverter.Convert(isCalcViewMode);
                m_calculator.IsEnabled = isCalcViewMode;
            }

            if (m_dateCalculator != null)
            {
                m_dateCalculator.Visibility = BooleanToVisibilityConverter.Convert(isDateCalcViewMode);
                m_dateCalculator.IsEnabled = isDateCalcViewMode;
            }

            if (m_graphingCalculator != null)
            {
                m_graphingCalculator.Visibility = BooleanToVisibilityConverter.Convert(isGraphingCalcViewMode);
                m_graphingCalculator.IsEnabled = isGraphingCalcViewMode;
            }

            if (m_converter != null)
            {
                m_converter.Visibility = BooleanToVisibilityConverter.Convert(isConverterViewMode);
                m_converter.IsEnabled = isConverterViewMode;
            }
        }

        private void UpdateViewState()
        {
            // All layout related view states are now handled only inside individual controls (standard, scientific, programmer, date, converter)
            if (NavCategory.IsConverterViewMode(m_model.Mode))
            {
                int modeIndex = NavCategory.GetIndexInGroup(m_model.Mode, CategoryGroupType.Converter);
                m_model.ConverterViewModel.CurrentCategory = m_model.ConverterViewModel.Categories[modeIndex];
            }
        }

        private void UpdatePanelViewState()
        {
            if (m_calculator != null)
            {
                m_calculator.UpdatePanelViewState();
            }
        }

        private void OnHighContrastChanged(AccessibilitySettings sender, object args)
        {
            if (Model.IsAlwaysOnTop && ActualHeight < 394)
            {
                // Sets to default always-on-top size to force re-layout
                ApplicationView.GetForCurrentView().TryResizeView(new Size(320, 394));
            }
        }

        private void OnPageLoaded(object sender, RoutedEventArgs args)
        {
            if (m_converter == null && m_calculator == null && m_dateCalculator == null && m_graphingCalculator == null)
            {
                // We have just launched into our default mode (standard calc) so ensure calc is loaded
                EnsureCalculator();
                m_model.CalculatorViewModel.IsStandard = true;
            }

            Window.Current.SizeChanged += WindowSizeChanged;
            m_accessibilitySettings.HighContrastChanged += OnHighContrastChanged;
            UpdateViewState();

            SetHeaderAutomationName();
            SetDefaultFocus();

            // Delay load things later when we get a chance.
            _ = Dispatcher.RunAsync(
                CoreDispatcherPriority.Normal, new DispatchedHandler(() =>
                {
                    if (TraceLogger.GetInstance().IsWindowIdInLog(ApplicationView.GetApplicationViewIdForWindow(CoreWindow.GetForCurrentThread())))
                    {
                        AppLifecycleLogger.GetInstance().LaunchUIResponsive();
                        AppLifecycleLogger.GetInstance().LaunchVisibleComplete();
                    }
                }));
        }

        private void App_Suspending(object sender, Windows.ApplicationModel.SuspendingEventArgs e)
        {
            if (m_model.IsAlwaysOnTop)
            {
                ApplicationDataContainer localSettings = ApplicationData.Current.LocalSettings;
                localSettings.Values[ApplicationViewModel.WidthLocalSettings] = ActualWidth;
                localSettings.Values[ApplicationViewModel.HeightLocalSettings] = ActualHeight;
            }
        }

        private void EnsureCalculator()
        {
            if (m_calculator == null)
            {
                // delay load calculator.
                m_calculator = new Calculator();
                m_calculator.Name = "Calculator";
                m_calculator.DataContext = m_model.CalculatorViewModel;
                Binding isStandardBinding = new Binding();
                isStandardBinding.Path = new PropertyPath("IsStandard");
                m_calculator.SetBinding(Calculator.IsStandardProperty, isStandardBinding);
                Binding isScientificBinding = new Binding();
                isScientificBinding.Path = new PropertyPath("IsScientific");
                m_calculator.SetBinding(Calculator.IsScientificProperty, isScientificBinding);
                Binding isProgramerBinding = new Binding();
                isProgramerBinding.Path = new PropertyPath("IsProgrammer");
                m_calculator.SetBinding(Calculator.IsProgrammerProperty, isProgramerBinding);
                Binding isAlwaysOnTopBinding = new Binding();
                isAlwaysOnTopBinding.Path = new PropertyPath("IsAlwaysOnTop");
                m_calculator.SetBinding(Calculator.IsAlwaysOnTopProperty, isAlwaysOnTopBinding);
                m_calculator.Style = CalculatorBaseStyle;

                CalcHolder.Child = m_calculator;

                // Calculator's "default" state is visible, but if we get delay loaded
                // when in converter, we should not be visible. This is not a problem for converter
                // since its default state is hidden.
                ShowHideControls(Model.Mode);
            }

            if (m_dateCalculator != null)
            {
                m_dateCalculator.CloseCalendarFlyout();
            }
        }

        private void EnsureDateCalculator()
        {
            if (m_dateCalculator == null)
            {
                // delay loading converter
                m_dateCalculator = new DateCalculator();
                m_dateCalculator.Name = "dateCalculator";
                m_dateCalculator.DataContext = m_model.DateCalcViewModel;

                DateCalcHolder.Child = m_dateCalculator;
            }

            if (m_calculator != null)
            {
                m_calculator.CloseHistoryFlyout();
                m_calculator.CloseMemoryFlyout();
            }
        }

        private void EnsureGraphingCalculator()
        {
            if (m_graphingCalculator == null)
            {
                m_graphingCalculator = new GraphingCalculator();
                m_graphingCalculator.Name = "GraphingCalculator";
                m_graphingCalculator.DataContext = m_model.GraphingCalcViewModel;

                GraphingCalcHolder.Child = m_graphingCalculator;
            }
        }

        private void EnsureConverter()
        {
            if (m_converter == null)
            {
                // delay loading converter
                m_converter = new CalculatorApp.UnitConverter();
                m_converter.Name = "unitConverter";
                m_converter.DataContext = m_model.ConverterViewModel;
                m_converter.Style = UnitConverterBaseStyle;
                ConverterHolder.Child = m_converter;
            }
        }

        private void AnnounceCategoryName()
        {
            string categoryName = AutomationProperties.GetName(Header);
            NarratorAnnouncement announcement = CalculatorAnnouncement.GetCategoryNameChangedAnnouncement(categoryName);
            NarratorNotifier.Announce(announcement);
        }

        private bool ShouldShowBackButton(bool isAlwaysOnTop, bool isPopupOpen)
        {
            return !isAlwaysOnTop && isPopupOpen;
        }

        private double NavigationViewOpenPaneLength(bool isAlwaysOnTop)
        {
            return isAlwaysOnTop ? 0 : (double)Application.Current.Resources["SplitViewOpenPaneLength"];
        }

        private GridLength DoubleToGridLength(double value)
        {
            return new GridLength(value);
        }

        private void Settings_BackButtonClick(object sender, RoutedEventArgs e)
        {
            CloseSettingsPopup();
        }

        private CalculatorApp.Calculator m_calculator;
        private GraphingCalculator m_graphingCalculator;
        private CalculatorApp.UnitConverter m_converter;
        private CalculatorApp.DateCalculator m_dateCalculator;
        private CalculatorApp.ViewModel.ApplicationViewModel m_model;
        private Windows.UI.ViewManagement.AccessibilitySettings m_accessibilitySettings;
    }
}
