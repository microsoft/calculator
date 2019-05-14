using CalculatorApp.Common;
using CalculatorApp.Common.Automation;
using CalculatorApp.Converters;
using CalculatorApp.ViewModel;
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.ComponentModel;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.Graphics.Display;
using Windows.Storage;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Automation;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using WindowsCalculator;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

namespace CalculatorApp
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page
    {
        const string Calculator = "Calculator";
        const string DateCalculator = "Date";
        const string UnitConverter = "Converter";

        const string MemoryEmpty = "MemoryEmpty";
        const string MemoryRight = "MemoryRight";
        const string MemoryBottom = "MemoryBottom";
        const string MemoryDock = "MemoryDock";


        CalculatorApp.Calculator m_calculator;
        CalculatorApp.UnitConverter m_converter;
        CalculatorApp.DateCalculator m_dateCalculator;
        CalculatorApp.ViewModel.ApplicationViewModel m_model;

        public ApplicationViewModel Model => m_model;

        public MainPage()
        {
            this.InitializeComponent();
            m_model = new ApplicationViewModel();

            // UNO TODO
            // KeyboardShortcutManager.Initialize();

            m_model.PropertyChanged += OnAppPropertyChanged;

            double sizeInInches = 0.0;

            // UNO TODO
            //if (SUCCEEDED(GetIntegratedDisplaySize(&sizeInInches)))
            //{
            //    if (sizeInInches < 7.0) // If device's display size (diagonal length) is less than 7 inches then keep the calc always in Portrait mode only
            //    {
            //        DisplayInformation.AutoRotationPreferences = DisplayOrientations.Portrait | DisplayOrientations.PortraitFlipped;
            //    }
            //}
        }


        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            if (m_model.CalculatorViewModel != null)
            {
                m_model.CalculatorViewModel.HistoryVM.ClearHistory();
            }

            ViewMode initialMode = ViewMode.Standard;
            if (e.Parameter != null)
            {
                String stringParameter = (String)(e.Parameter);
                if (stringParameter != null)
                {
                    if (int.TryParse(stringParameter, out var intParameter))
                    {
                        initialMode = (ViewMode)intParameter;
                    }
                }
            }
            else
            {
                ApplicationDataContainer localSettings = ApplicationData.Current.LocalSettings;
                if (localSettings.Values.ContainsKey(nameof(ApplicationViewModel.Mode)))
                {
                    initialMode = NavCategory.Deserialize(localSettings.Values[nameof(ApplicationViewModel.Mode)]);
                }
            }

            m_model.Initialize(initialMode);
        }

        void WindowSizeChanged(object sender, Windows.UI.Core.WindowSizeChangedEventArgs e)
        {
            // We don't use layout aware page's view states, we have our own
            UpdateViewState();
        }

        void OnAppPropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            String propertyName = e.PropertyName;
            if (propertyName == nameof(ApplicationViewModel.Mode))
            {
                ViewMode newValue = m_model.Mode;
                ViewMode previousMode = m_model.PreviousMode;

                if (newValue == ViewMode.Standard)
                {
                    EnsureCalculator();
                    m_model.CalculatorViewModel.AreHistoryShortcutsEnabled = true;
                    m_model.CalculatorViewModel.HistoryVM.AreHistoryShortcutsEnabled = true;
                    m_calculator.AnimateCalculator(NavCategory.IsConverterViewMode(previousMode));
                    m_model.CalculatorViewModel.HistoryVM.ReloadHistory(newValue);
                }
                else if (newValue == ViewMode.Scientific)
                {
                    EnsureCalculator();
                    m_model.CalculatorViewModel.AreHistoryShortcutsEnabled = true;
                    m_model.CalculatorViewModel.HistoryVM.AreHistoryShortcutsEnabled = true;
                    if (m_model.PreviousMode != ViewMode.Scientific)
                    {
                        m_calculator.AnimateCalculator(NavCategory.IsConverterViewMode(previousMode));
                    }

                    m_model.CalculatorViewModel.HistoryVM.ReloadHistory(newValue);
                }
                else if (newValue == ViewMode.Programmer)
                {
                    m_model.CalculatorViewModel.AreHistoryShortcutsEnabled = false;
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
                        m_model.CalculatorViewModel.AreHistoryShortcutsEnabled = false;
                        m_model.CalculatorViewModel.HistoryVM.AreHistoryShortcutsEnabled = false;
                    }
                    EnsureDateCalculator();
                }
                else if (NavCategory.IsConverterViewMode(newValue))
                {
                    if (m_model.CalculatorViewModel != null)
                    {
                        m_model.CalculatorViewModel.AreHistoryShortcutsEnabled = false;
                        m_model.CalculatorViewModel.HistoryVM.AreHistoryShortcutsEnabled = false;
                    }
                    EnsureConverter();
                    if (!NavCategory.IsConverterViewMode(previousMode))
                    {
                        // UNO TODO
                        // m_converter.AnimateConverter();
                    }
                }

                ShowHideControls(newValue);

                UpdateViewState();
                SetDefaultFocus();
            }
            else if (propertyName == nameof(ApplicationViewModel.CategoryName))
            {
                SetHeaderAutomationName();
                AnnounceCategoryName();
            }
        }

        void ShowHideControls(ViewMode mode)
        {
            var isCalcViewMode = NavCategory.IsCalculatorViewMode(mode);
            var isDateCalcViewMode = NavCategory.IsDateCalculatorViewMode(mode);
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

            if (m_converter != null)
            {
                m_converter.Visibility = BooleanToVisibilityConverter.Convert(isConverterViewMode);
                m_converter.IsEnabled = isConverterViewMode;
            }
        }

        void UpdateViewState()
        {
            // All layout related view states are now handled only inside individual controls (standard, scientific, programmer, date, converter)
            if (NavCategory.IsConverterViewMode(m_model.Mode))
            {
                int modeIndex = NavCategory.GetIndexInGroup(m_model.Mode, CategoryGroupType.Converter);
                // UNO TODO
                // m_model.ConverterViewModel.CurrentCategory = m_model.ConverterViewModel.Categories.GetAt(modeIndex);
            }
        }

        void UpdatePanelViewState()
        {
            if (m_calculator != null)
            {
                m_calculator.UpdatePanelViewState();
            }
        }

        void OnPageLoaded(object sender, RoutedEventArgs args)
        {
            if (m_converter == null && m_calculator == null && m_dateCalculator == null)
            {
                // We have just launched into our default mode (standard calc) so ensure calc is loaded
                EnsureCalculator();
                m_model.CalculatorViewModel.IsStandard = true;
            }

            Windows.UI.Xaml.Window.Current.SizeChanged += WindowSizeChanged;
            UpdateViewState();

            SetHeaderAutomationName();
            SetDefaultFocus();

            // Delay load things later when we get a chance.
            this.Dispatcher.RunAsync(
                CoreDispatcherPriority.Normal, () => {
                // UNO TODO
                //if (TraceLogger.GetInstance().UpdateWindowIdLog(ApplicationView.GetApplicationViewIdForWindow(CoreWindow.GetForCurrentThread())))
                //{
                //    TraceLogger.GetInstance().LogAppLaunchComplete();
                //    AppLifecycleLogger.GetInstance().LaunchUIResponsive();
                //    AppLifecycleLogger.GetInstance().LaunchVisibleComplete();
                //}
            });
        }

        void SetDefaultFocus()
        {
            if (m_calculator != null && m_calculator.Visibility == Visibility.Visible)
            {
                m_calculator.SetDefaultFocus();
            }

            // UNO TODO
            //if (m_dateCalculator != null && m_dateCalculator.Visibility == Visibility.Visible)
            //{
            //    m_dateCalculator.SetDefaultFocus();
            //}
            //if (m_converter != null && m_converter.Visibility == Visibility.Visible)
            //{
            //    m_converter.SetDefaultFocus();
            //}
        }

        void EnsureCalculator()
        {
            if (m_calculator == null)
            {
                // delay load calculator.
                m_calculator = new Calculator();
                m_calculator.Name = "Calculator";
                m_calculator.DataContext = m_model.CalculatorViewModel;
                Binding isStandardBinding = new Binding();
                isStandardBinding.Path = new PropertyPath("IsStandard");
                m_calculator.SetBinding(CalculatorApp.Calculator.IsStandardProperty, isStandardBinding);
                Binding isScientificBinding = new Binding();
                isScientificBinding.Path = new PropertyPath("IsScientific");
                m_calculator.SetBinding(CalculatorApp.Calculator.IsScientificProperty, isScientificBinding);
                Binding isProgramerBinding = new Binding();
                isProgramerBinding.Path = new PropertyPath("IsProgrammer");
                m_calculator.SetBinding(CalculatorApp.Calculator.IsProgrammerProperty, isProgramerBinding);

#if NETFX_CORE
                // UNO TODO
                m_calculator.Style = CalculatorBaseStyle;
#endif

                CalcHolder.Child = m_calculator;

                // Calculator's "default" state is visible, but if we get delay loaded
                // when in converter, we should not be visible. This is not a problem for converter
                // since it's default state is hidden.
                ShowHideControls(this.Model.Mode);
            }

            if (m_dateCalculator != null)
            {
                // UNO TODO
                // m_dateCalculator.CloseCalendarFlyout();
            }
        }

        void EnsureDateCalculator()
        {
            // UNO TODO
            //if (m_dateCalculator == null)
            //{
            //    // delay loading converter
            //    m_dateCalculator = new DateCalculator();
            //    m_dateCalculator.Name = "dateCalculator";
            //    m_dateCalculator.DataContext = m_model.DateCalcViewModel;

            //    DateCalcHolder.Child = m_dateCalculator;
            //}

            //if (m_calculator != null)
            //{
            //    m_calculator.CloseHistoryFlyout();
            //    m_calculator.CloseMemoryFlyout();
            //}
        }

        void EnsureConverter()
        {
            // UNO TODO
            //if (m_converter == null)
            //{
            //    // delay loading converter
            //    m_converter = new CalculatorApp.UnitConverter();
            //    m_converter.Name = "unitConverter";
            //    m_converter.DataContext = m_model.ConverterViewModel;
            //    m_converter.Style = UnitConverterBaseStyle;
            //    ConverterHolder.Child = m_converter;
            //}
        }

        void OnNavLoaded(object sender, RoutedEventArgs e)
        {
            if (NavView.SelectedItem == null)
            {
                var menuItems = (IEnumerable<object>)(NavView.MenuItemsSource);
                var itemCount = (int)(menuItems.Count());
                var flatIndex = NavCategory.GetFlatIndex(Model.Mode);

                if (flatIndex >= 0 && flatIndex < itemCount)
                {
                    NavView.SelectedItem = menuItems.ElementAt(flatIndex);
                }
            }

            // UNO TODO
            //var acceleratorList = NavCategory.GetCategoryAcceleratorKeys();
            // foreach (var accelerator in acceleratorList)
            // {
            //     NavView.SetValue(Common.KeyboardShortcutManager.VirtualKeyAltChordProperty, accelerator);
            // }
            // // Special case logic for Ctrl+E accelerator for Date Calculation Mode
            // NavView.SetValue(Common.KeyboardShortcutManager.VirtualKeyControlChordProperty, Common.MyVirtualKey.E);
        }

        void OnNavPaneOpening(NavigationView sender, object args)
        {
            if (NavFooter == null)
            {
                this.FindName("NavFooter");
            }
        }

        void OnNavPaneOpened(NavigationView sender, object args)
        {
            // UNO TODO
            // KeyboardShortcutManager.HonorShortcuts(false);
            // TraceLogger.GetInstance().LogNavBarOpened();
        }

        void OnNavPaneClosed(NavigationView sender, object args)
        {
            // UNO TODO
            // KeyboardShortcutManager.HonorShortcuts(true);
            this.SetDefaultFocus();
        }

        void OnAboutButtonClick(object sender, ItemClickEventArgs e)
        {
            ShowAboutPage();
        }

        void OnAboutFlyoutOpened(object sender, object e)
        {
            // Keep Ignoring Escape till the About page flyout is opened
            // UNO TODO
            //KeyboardShortcutManager.IgnoreEscape(false);

            // UNO TODO
            //KeyboardShortcutManager.UpdateDropDownState(this.AboutPageFlyout);
        }

        void OnAboutFlyoutClosed(object sender, object e)
        {
            // Start Honoring Escape once the About page flyout is closed
            // UNO TODO
            //KeyboardShortcutManager.HonorEscape();

            // UNO TODO
            //KeyboardShortcutManager.UpdateDropDownState(null);
        }

        void OnNavSelectionChanged(object sender, NavigationViewSelectionChangedEventArgs e)
        {
            var item = (NavigationViewItem)(e.SelectedItemContainer);
            if (item != null)
            {
                var selectedItem = (NavCategory)(item.DataContext);
                Model.Mode = selectedItem.Mode;
            }
        }

        public List<object> CreateUIElementsForCategories(ObservableCollection<NavCategoryGroup> categories)
        {
            var menuCategories = new List<object>();

            foreach (var group in categories)
            {
                menuCategories.Append(CreateNavViewHeaderFromGroup(group));

                foreach (var category in group.Categories)
                {
                    menuCategories.Append(CreateNavViewItemFromCategory(category));
                }
            }

            return menuCategories;
        }

        NavigationViewItemHeader CreateNavViewHeaderFromGroup(NavCategoryGroup group)
        {
            var header = new NavigationViewItemHeader();
            header.DataContext = group;

            header.Content = group.Name;
            AutomationProperties.SetName(header, group.AutomationName);
            AutomationProperties.SetHeadingLevel(header, Windows.UI.Xaml.Automation.Peers.AutomationHeadingLevel.Level1);

            return header;
        }

        NavigationViewItem CreateNavViewItemFromCategory(NavCategory category)
        {
            var item = new NavigationViewItem();
            item.DataContext = category;

            var icon = new FontIcon();
            icon.FontFamily = (Windows.UI.Xaml.Media.FontFamily)(App.Current.Resources["CalculatorFontFamily"]);
            icon.Glyph = category.Glyph;
            item.Icon = icon;

            item.Content = category.Name;
            item.AccessKey = category.AccessKey;
            item.Style = (Windows.UI.Xaml.Style)(Resources["NavViewItemStyle"]);

            AutomationProperties.SetName(item, category.AutomationName);
            AutomationProperties.SetAutomationId(item, category.AutomationId);

            return item;
        }

        void ShowAboutPage()
        {
            if (AboutPage == null)
            {
                this.FindName("AboutPage");
            }

            FlyoutBase.ShowAttachedFlyout(AboutButton);
        }

        void UnregisterEventHandlers()
        {
            Windows.UI.Xaml.Window.Current.SizeChanged -= WindowSizeChanged;

            if (m_calculator != null)
            {
                m_calculator.UnregisterEventHandlers();
            }
        }

        void SetHeaderAutomationName()
        {
            ViewMode mode = m_model.Mode;
            var resProvider = AppResourceProvider.GetInstance();

            String name;
            if (NavCategory.IsDateCalculatorViewMode(mode))
            {
                name = resProvider.GetResourceString("HeaderAutomationName_Date");
            }
            else
            {
                string full = "";
                if (NavCategory.IsCalculatorViewMode(mode))
                {
                    full = resProvider.GetResourceString("HeaderAutomationName_Calculator");
                }
                else if (NavCategory.IsConverterViewMode(mode))
                {
                    full = resProvider.GetResourceString("HeaderAutomationName_Converter");
                }

                full = full.Replace("%1", m_model.CategoryName);

                name = full;
            }

            AutomationProperties.SetName(Header, name);
        }

        void AnnounceCategoryName()
        {
            String categoryName = AutomationProperties.GetName(Header);
            NarratorAnnouncement announcement = CalculatorAnnouncement.GetCategoryNameChangedAnnouncement(categoryName);
            // UNO TODO
            // NarratorNotifier.Announce(announcement);
        }

        void OnNavItemInvoked(NavigationView sender, NavigationViewItemInvokedEventArgs e)
        {
            NavView.IsPaneOpen = false;
        }

    }
}
