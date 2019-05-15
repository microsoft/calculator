using CalculatorApp.Common;
using CalculatorApp.ViewModel;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Text.RegularExpressions;
using System.Windows.Input;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.Globalization.NumberFormatting;
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

// The User Control item template is documented at https://go.microsoft.com/fwlink/?LinkId=234236

namespace CalculatorApp
{
    public sealed partial class Calculator : UserControl
    {
        event FullscreenFlyoutClosedEventHandler FullscreenFlyoutClosed;


        StandardCalculatorViewModel Model => (CalculatorApp.ViewModel.StandardCalculatorViewModel)(this.DataContext);



        public bool IsStandard
        {
            get { return (bool)GetValue(IsStandardProperty); }
            set { SetValue(IsStandardProperty, value); }
        }

        public static readonly DependencyProperty IsStandardProperty =
            DependencyProperty.Register("IsStandard", typeof(bool), typeof(Calculator), new PropertyMetadata(false, (snd, args) => ((Calculator)snd).OnIsStandardPropertyChanged((bool)args.OldValue, (bool)args.NewValue)));



        public bool IsScientific
        {
            get { return (bool)GetValue(IsScientificProperty); }
            set { SetValue(IsScientificProperty, value); }
        }

        public static readonly DependencyProperty IsScientificProperty =
            DependencyProperty.Register("IsScientific", typeof(bool), typeof(Calculator), new PropertyMetadata(false, (snd, args) => ((Calculator)snd).OnIsScientificPropertyChanged((bool)args.OldValue, (bool)args.NewValue)));

		public bool IsProgrammer
        {
            get { return (bool)GetValue(IsProgrammerProperty); }
            set { SetValue(IsProgrammerProperty, value); }
        }

        // Using a DependencyProperty as the backing store for IsProgrammer.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty IsProgrammerProperty =
            DependencyProperty.Register("IsProgrammer", typeof(bool), typeof(Calculator), new PropertyMetadata(false, (snd, args) => ((Calculator)snd).OnIsProgrammerPropertyChanged((bool)args.OldValue, (bool)args.NewValue)));


		Windows.UI.Xaml.Controls.ListView m_tokenList;
        Windows.UI.Xaml.Controls.MenuFlyout m_displayFlyout;
        bool m_doAnimate;
        bool m_resultAnimate;
        bool m_isLastAnimatedInScientific;
        bool m_isLastAnimatedInProgrammer;
        bool m_IsLastFlyoutMemory = false;
        bool m_IsLastFlyoutHistory = false;
        HistoryList m_historyList;
        bool m_fIsHistoryFlyoutOpen;
        bool m_fIsMemoryFlyoutOpen;

        string m_openMemoryFlyoutAutomationName;
        string m_closeMemoryFlyoutAutomationName;
        string m_openHistoryFlyoutAutomationName;
        string m_closeHistoryFlyoutAutomationName;

        public ICommand HistoryButtonPressed;

        Windows.UI.Xaml.Controls.PivotItem m_pivotItem;
        bool m_IsDigit = false;
        Memory m_memory;

        public Calculator()
        {
            this.InitializeComponent();

            m_doAnimate = false;
            m_isLastAnimatedInScientific = false;
            m_isLastAnimatedInProgrammer = false;
            m_resultAnimate = false;
            HistoryButtonPressed = new DelegateCommand(ToggleHistoryFlyout);

            SetFontSizeResources();
            LoadResourceStrings();

            if (LocalizationService.GetInstance().IsRtlLayout())
            {
                HistoryButton.HorizontalAlignment = HorizontalAlignment.Left;
            }

            m_displayFlyout = (MenuFlyout)(Resources["DisplayContextMenu"]);
            var resLoader = AppResourceProvider.GetInstance();

            // UNO TODO
            // CopyMenuItem.Text = resLoader.GetResourceString("copyMenuItem");
            // PasteMenuItem.Text = resLoader.GetResourceString("pasteMenuItem");
        }

        void LoadResourceStrings()
        {
            var resProvider = AppResourceProvider.GetInstance();
            m_openMemoryFlyoutAutomationName = resProvider.GetResourceString("MemoryButton_Open");
            m_closeMemoryFlyoutAutomationName = resProvider.GetResourceString("MemoryButton_Close");
            m_openHistoryFlyoutAutomationName = resProvider.GetResourceString("HistoryButton_Open");
            m_closeHistoryFlyoutAutomationName = resProvider.GetResourceString("HistoryButton_Close");
            AutomationProperties.SetName(MemoryButton, m_openMemoryFlyoutAutomationName);
            AutomationProperties.SetName(HistoryButton, m_openHistoryFlyoutAutomationName);
        }

        void InitializeHistoryView(HistoryViewModel historyVM)
        {
            if (m_historyList == null)
            {
                m_historyList = new HistoryList();
                m_historyList.DataContext = historyVM;
                historyVM.HideHistoryClicked += OnHideHistoryClicked;
                historyVM.HistoryItemClicked += OnHistoryItemClicked;
            }
        }

        // Since we need different font sizes for different numeric system,
        // we use a table of optimal font sizes for each numeric system.
        static readonly FontTable[] fontTables = {
            new FontTable("Arab", 104, 29.333, 23, 40, 56, 40, 56 ),   new FontTable( "ArabExt", 104, 29.333, 23, 40, 56, 40, 56 ), new FontTable( "Beng", 104, 26, 17, 40, 56, 40, 56 ),
            new FontTable("Deva", 104, 29.333, 20.5, 40, 56, 40, 56 ), new FontTable( "Gujr", 104, 29.333, 18.5, 40, 56, 40, 56 ),  new FontTable( "Khmr", 104, 29.333, 19.5, 40, 56, 40, 56 ),
            new FontTable("Knda", 104, 25, 17, 40, 56, 40, 56 ),       new FontTable( "Laoo", 104, 28, 18, 40, 56, 40, 56 ),        new FontTable( "Latn", 104, 29.333, 23, 40, 56, 40, 56 ),
            new FontTable("Mlym", 80, 22, 15.5, 30, 56, 35, 48 ),      new FontTable( "Mymr", 104, 29.333, 20, 35, 48, 36, 48 ),    new FontTable( "Orya", 88, 26, 20, 40, 56, 40, 56 ),
            new FontTable("TamlDec", 77, 25, 16, 28, 48, 34, 48 ),     new FontTable( "Telu", 104, 25, 16.5, 40, 56, 40, 56 ),      new FontTable( "Thai", 104, 28, 18, 40, 56, 40, 56 ),
            new FontTable("Tibt", 104, 29.333, 20, 40, 56, 40, 56 ),   new FontTable( "Default", 104, 29.333, 23, 40, 56, 40, 56 ),
        };

        void SetFontSizeResources()
        {
#if true // HAS_UNO
            FontTable currentItem = fontTables.First(f => f.numericSystem == "Default");
#else
            DecimalFormatter formatter = LocalizationService.GetRegionalSettingsAwareDecimalFormatter();

            FontTable currentItem = fontTables.First(f => f.numericSystem == formatter.NumeralSystem);
#endif

            this.Resources.Add(("ResultFullFontSize"), currentItem.fullFont);
            this.Resources.Add(("ResultFullMinFontSize"), currentItem.fullFontMin);
            this.Resources.Add(("ResultPortraitMinFontSize"), currentItem.portraitMin);
            this.Resources.Add(("ResultSnapFontSize"), currentItem.snapFont);
            this.Resources.Add(("CalcButtonCaptionSizeOverride"), currentItem.fullNumPadFont);
            this.Resources.Add(("CalcButtonScientificSnapCaptionSizeOverride"), currentItem.snapScientificNumPadFont);
            this.Resources.Add(("CalcButtonScientificPortraitCaptionSizeOverride"), currentItem.portraitScientificNumPadFont);
        }

        void OnLoaded(object sender, RoutedEventArgs args)
        {
            Model.PropertyChanged += OnCalcPropertyChanged;
            Model.HideMemoryClicked += OnHideMemoryClicked;

            InitializeHistoryView(Model.HistoryVM);
            String historyPaneName = AppResourceProvider.GetInstance().GetResourceString("HistoryPane");
            HistoryFlyout.FlyoutPresenterStyle.Setters.Append(new Setter(AutomationProperties.NameProperty, historyPaneName));
            String memoryPaneName = AppResourceProvider.GetInstance().GetResourceString("MemoryPane");
            MemoryFlyout.FlyoutPresenterStyle.Setters.Append(new Setter(AutomationProperties.NameProperty, memoryPaneName));

            if (Windows.Foundation.Metadata.ApiInformation.IsEventPresent("Windows.UI.Xaml.Controls.Primitives.FlyoutBase", "Closing"))
            {
                HistoryFlyout.Closing += HistoryFlyout_Closing;
                MemoryFlyout.Closing += OnMemoryFlyoutClosing;
            }

            // Delay load things later when we get a chance.
            var _ = Dispatcher.RunAsync(
                CoreDispatcherPriority.Normal, () =>
                {
                    // if (// TraceLogger.GetInstance().UpdateWindowIdLog(ApplicationView.GetApplicationViewIdForWindow(CoreWindow.GetForCurrentThread())))
                    {
                        // var refThis = weakThis.Resolve<Calculator>();
                        // if (refThis != null)
                        {
                            this.GetMemory();
                        }
                    }
                });
        }

        string GetCurrentLayoutState()
        {
            string state;

            if (IsProgrammer)
            {
                state = "Programmer";
            }
            else if (IsScientific)
            {
                state = "Scientific";
            }
            else
            {
                state = "Standard";
            }

            return state;
        }

        void UpdateViewState()
        {
            string state;
            if (IsProgrammer)
            {
                state = "Programmer";
                Model.IsDecimalEnabled = false;
                // UNO TODO
                // ResultsMVisualStateTrigger.MinWindowHeight = 640;
            }
            else if (IsScientific)
            {
                state = "Scientific";
                Model.IsDecimalEnabled = true;
                // UNO TODO
                // ResultsMVisualStateTrigger.MinWindowHeight = 544;
            }
            else
            {
                state = "Standard";
                Model.IsDecimalEnabled = true;
                // UNO TODO
                // ResultsMVisualStateTrigger.MinWindowHeight = 1;
            }

            CloseHistoryFlyout();
            CloseMemoryFlyout();

            VisualStateManager.GoToState(this, state, true /*useTransitions*/);
        }

        public void AnimateCalculator(bool resultAnimate)
        {
            if (App.IsAnimationEnabled())
            {
                m_doAnimate = true;
                m_resultAnimate = resultAnimate;
                if (((m_isLastAnimatedInScientific && IsScientific) || (!m_isLastAnimatedInScientific && !IsScientific))
                    && ((m_isLastAnimatedInProgrammer && IsProgrammer) || (!m_isLastAnimatedInProgrammer && !IsProgrammer)))
                {
                    // We are forcing the animation here
                    // It's because if last animation was in standard, then go to unit converter, then comes back to standard
                    // The state for the calculator does not change and the animation would not get run.
                    this.OnStoryboardCompleted(null, null);
                }
            }
        }

        void OnContextRequested(UIElement sender, ContextRequestedEventArgs e)
        {
            var requestedElement = (FrameworkElement)(e.OriginalSource);

            // UNO TODO
            // PasteMenuItem.IsEnabled = CopyPasteManager.HasStringToPaste();

            if (e.TryGetPosition(requestedElement, out var point))
            {
                m_displayFlyout.ShowAt(requestedElement, point);
            }
            else
            {
                // Not invoked via pointer, so let XAML choose a default location.
                m_displayFlyout.ShowAt(requestedElement);
            }

            e.Handled = true;
        }

        void OnContextCanceled(UIElement sender, RoutedEventArgs e)
        {
            m_displayFlyout.Hide();
        }

        void OnLayoutStateChanged(object sender, object e)
        {
            UpdatePanelViewState();
        }

        void OnIsStandardPropertyChanged(bool oldValue, bool newValue)
        {
            UpdateViewState();
            UpdatePanelViewState();
        }

        void OnIsScientificPropertyChanged(bool oldValue, bool newValue)
        {
            if (newValue)
            {
                EnsureScientific();
            }

            UpdateViewState();
            UpdatePanelViewState();
        }

        void OnIsProgrammerPropertyChanged(bool oldValue, bool newValue)
        {
            if (newValue)
            {
                EnsureProgrammer();
                m_pivotItem = (Windows.UI.Xaml.Controls.PivotItem)(DockPivot.Items[0]);
                DockPivot.Items.RemoveAt(0);
            }
            else
            {
                if (m_pivotItem != null && DockPivot.Items.Count == 1)
                {
                    DockPivot.Items.Insert(0, m_pivotItem);
                }
            }

            DockPivot.SelectedIndex = 0;
            UpdateViewState();
            UpdatePanelViewState();
        }

        void OnIsInErrorPropertyChanged()
        {
            bool isError = Model.IsInError;

            String newState = isError ? "ErrorLayout" : "NoErrorLayout";
            VisualStateManager.GoToState(this, newState, false);

            if (m_memory != null)
            {
                m_memory.IsErrorVisualState = isError;
            }

            // UNO TODO
            // OpsPanel.IsErrorVisualState = isError;
            // if (IsScientific && ScientificAngleButtons != null)
            // {
            //     ScientificAngleButtons.IsErrorVisualState = isError;
            // }
            // else if (IsProgrammer && ProgrammerDisplayPanel != null)
            // {
            //     ProgrammerDisplayPanel.IsErrorVisualState = isError;
            // }
        }

        // Once the storyboard that rearranges the buttons completed,
        // We do the animation based on the Mode or Orientation change.
        void OnStoryboardCompleted(object sender, object e)
        {
            m_isLastAnimatedInScientific = IsScientific;
            m_isLastAnimatedInProgrammer = IsProgrammer;
            if (m_doAnimate)
            {
                m_doAnimate = false;
                if (m_resultAnimate)
                {
                    m_resultAnimate = false;
                    Animate.Begin();
                }
                else
                {
                    AnimateWithoutResult.Begin();
                }
            }
        }

        void EnsureScientific()
        {
            OpsPanel.EnsureScientificOps();

            if (ScientificAngleButtons == null)
            {
                this.FindName("ScientificAngleButtons");
            }
        }

        void EnsureProgrammer()
        {
            if (ProgrammerOperators == null)
            {
                this.FindName("ProgrammerOperators");
            }

            if (ProgrammerDisplayPanel == null)
            {
                this.FindName("ProgrammerDisplayPanel");
            }

            OpsPanel.EnsureProgrammerRadixOps();
			// UNO TODO
			// ProgrammerOperators.SetRadixButton(Model.GetCurrentRadixType());
		}

		void OnCalcPropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            String prop = e.PropertyName;
            if (prop == nameof(StandardCalculatorViewModel.IsMemoryEmpty))
            {
                UpdateMemoryState();
            }
            else if (prop == nameof(StandardCalculatorViewModel.IsMemoryEmpty))
            {
                OnIsInErrorPropertyChanged();
            }
        }

        public void UpdatePanelViewState()
        {
            UpdateHistoryState();
            UpdateMemoryState();
        }

        void UpdateHistoryState()
        {
            String viewState = App.GetAppViewState();
            if (viewState == ViewState.DockedView)
            {
                // docked view
                CloseHistoryFlyout();
                SetChildAsHistory();
                HistoryButton.Visibility = Visibility.Collapsed;

                if (!IsProgrammer && m_IsLastFlyoutHistory)
                {
                    DockPivot.SelectedIndex = 0;
                }
            }
            else
            { // flyout view
                DockHistoryHolder.Child = null;
                if (!IsProgrammer)
                {
                    HistoryButton.Visibility = Visibility.Visible;
                }
            }
        }

        void UpdateMemoryState()
        {
            if (!Model.IsMemoryEmpty)
            {
                MemRecall.IsEnabled = true;
                ClearMemoryButton.IsEnabled = true;
            }
            else
            {
                MemRecall.IsEnabled = false;
                ClearMemoryButton.IsEnabled = false;
            }

            String viewState = App.GetAppViewState();
            if (viewState == ViewState.DockedView)
            {
                CloseMemoryFlyout();
                SetChildAsMemory();
                MemoryButton.Visibility = Visibility.Collapsed;

                if (m_IsLastFlyoutMemory && !IsProgrammer)
                {
                    DockPivot.SelectedIndex = 1;
                }
            }
            else
            {
                MemoryButton.Visibility = Visibility.Visible;
                DockMemoryHolder.Child = null;
            }
        }

        void SetChildAsMemory()
        {
            DockMemoryHolder.Child = GetMemory();
        }

        void SetChildAsHistory()
        {
            if (m_historyList == null)
            {
                InitializeHistoryView(Model.HistoryVM);
            }

            DockHistoryHolder.Child = m_historyList;
        }

        void OnHideHistoryClicked()
        {
            ToggleHistoryFlyout(null);
        }

        void OnHistoryItemClicked(HistoryItemViewModel e)
        {
            int tokenSize;
            System.Diagnostics.Debug.Assert(e.GetTokens() != null);
            e.GetTokens().GetSize(out tokenSize);
            // TraceLogger.GetInstance().LogHistoryItemLoadBegin();
            Model.SetHistoryExpressionDisplay(e.GetTokens(), e.GetCommands());
            Model.SetExpressionDisplay(e.GetTokens(), e.GetCommands());
            Model.SetPrimaryDisplay(e.Result, false);
            Model.IsFToEEnabled = false;

            // TraceLogger.GetInstance().LogHistoryItemLoadEnd(tokenSize);
            CloseHistoryFlyout();
            this.Focus(FocusState.Programmatic);
        }

        void HistoryFlyout_Opened(object sender, object args)
        {
            m_fIsHistoryFlyoutOpen = true;
            m_IsLastFlyoutMemory = false;
            m_IsLastFlyoutHistory = true;
            EnableControls(false);
            AutomationProperties.SetName(HistoryButton, m_closeHistoryFlyoutAutomationName);
            // TraceLogger.GetInstance().LogHistoryFlyoutOpenEnd(Model.HistoryVM.ItemSize);
            // TraceLogger.GetInstance().LogHistoryBodyOpened();
        }

        void HistoryFlyout_Closing(FlyoutBase sender, FlyoutBaseClosingEventArgs args)
        {
            // Set in the Closing event so the new name is available when the Flyout has Closed.
            AutomationProperties.SetName(HistoryButton, m_openHistoryFlyoutAutomationName);
        }

        void HistoryFlyout_Closed(object sender, object args)
        {
            // Ideally, this would be renamed in the Closing event because the Closed event is too late.
            // Closing is not available until RS1+ so we set the name again here for TH2 support.
            AutomationProperties.SetName(HistoryButton, m_openHistoryFlyoutAutomationName);
            m_fIsHistoryFlyoutOpen = false;
            EnableControls(true);
            if (HistoryButton.IsEnabled && HistoryButton.Visibility == Visibility.Visible)
            {
                HistoryButton.Focus(FocusState.Programmatic);
            }

            FullscreenFlyoutClosed();
        }

        void CloseHistoryFlyout()
        {
            if (m_fIsHistoryFlyoutOpen)
            {
                HistoryFlyout.Hide();
            }
        }

        void CloseMemoryFlyout()
        {
            if (m_fIsMemoryFlyoutOpen)
            {
                MemoryFlyout.Hide();
            }
        }

        public void SetDefaultFocus()
        {
            Results.Focus(FocusState.Programmatic);
        }

        void ToggleHistoryFlyout(object parameter)
        {
            String viewState = App.GetAppViewState();
            // If app starts correctly in snap mode and shortcut is used for history then we need to load history if not yet initialized.
            if (viewState != ViewState.DockedView)
            {
                if (m_fIsHistoryFlyoutOpen)
                {
                    HistoryFlyout.Hide();
                }
                else
                {
                    // TraceLogger.GetInstance().LogHistoryFlyoutOpenBegin(Model.HistoryVM.ItemSize);
                    HistoryFlyout.Content = m_historyList;
                    // UNO TODO
                    // m_historyList.RowHeight = NumpadPanel.ActualHeight;
                    FlyoutBase.ShowAttachedFlyout(HistoryButton);
                }
            }
        }

        void ToggleMemoryFlyout(object sender, object args)
        {
            String viewState = App.GetAppViewState();
            if (viewState != ViewState.DockedView)
            {
                if (m_fIsMemoryFlyoutOpen)
                {
                    MemoryFlyout.Hide();
                }
                else
                {
                    // TraceLogger.GetInstance().LogMemoryFlyoutOpenBegin(Model.MemorizedNumbers.Size);
                    MemoryFlyout.Content = GetMemory();
                    // UNO TODO
                    // m_memory.RowHeight = NumpadPanel.ActualHeight;
                    FlyoutBase.ShowAttachedFlyout(MemoryButton);
                }
            }
        }

        void OnMemoryFlyoutOpened(object sender, object args)
        {
            // TraceLogger.GetInstance().LogMemoryFlyoutOpenEnd(Model.MemorizedNumbers.Size);
            m_IsLastFlyoutMemory = true;
            m_IsLastFlyoutHistory = false;
            m_fIsMemoryFlyoutOpen = true;
            AutomationProperties.SetName(MemoryButton, m_closeMemoryFlyoutAutomationName);
            EnableControls(false);
            // TraceLogger.GetInstance().LogMemoryBodyOpened();
        }

        void OnMemoryFlyoutClosing(FlyoutBase sender, FlyoutBaseClosingEventArgs args)
        {
            // Set in the Closing event so the new name is available when the Flyout has Closed.
            AutomationProperties.SetName(MemoryButton, m_openMemoryFlyoutAutomationName);
        }

        void OnMemoryFlyoutClosed(object sender, object args)
        {
            // Ideally, this would be renamed in the Closing event because the Closed event is too late.
            // Closing is not available until RS1+ so we set the name again here for TH2 support.
            AutomationProperties.SetName(MemoryButton, m_openMemoryFlyoutAutomationName);
            m_fIsMemoryFlyoutOpen = false;
            EnableControls(true);
            if (MemoryButton.IsEnabled)
            {
                MemoryButton.Focus(FocusState.Programmatic);
            }

            FullscreenFlyoutClosed();
        }

        Memory GetMemory()
        {
            if (m_memory == null)
            {
                m_memory = new Memory();
                VisualStateManager.GoToState(m_memory, GetCurrentLayoutState(), true /*useTransitions*/);
            }

            return m_memory;
        }

        void OnHideMemoryClicked()
        {
            if (!m_fIsMemoryFlyoutOpen)
            {
                this.Focus(FocusState.Programmatic);
            }

            MemoryFlyout.Hide();
        }

        void EnableMemoryControls(bool enable)
        {
            memButton.IsEnabled = enable;
            MemMinus.IsEnabled = enable;
            MemPlus.IsEnabled = enable;
            if (!Model.IsMemoryEmpty)
            {
                MemRecall.IsEnabled = enable;
                ClearMemoryButton.IsEnabled = enable;
            }
        }

        void EnableControls(bool enable)
        {
            OpsPanel.IsEnabled = enable;
            EnableMemoryControls(enable);
        }

        void OnMemoryFlyOutTapped(object sender, TappedRoutedEventArgs e)
        {
            Grid grid = (Grid)(sender);
            Point point = e.GetPosition(null);

            if (point.Y < (grid.ActualHeight - NumpadPanel.ActualHeight))
            {
                MemoryFlyout.Hide();
            }
        }

        void OnHistoryFlyOutTapped(object sender, TappedRoutedEventArgs e)
        {
            Grid grid = (Grid)(sender);
            Point point = e.GetPosition(null);

            if (point.Y < (grid.ActualHeight - NumpadPanel.ActualHeight))
            {
                HistoryFlyout.Hide();
            }
        }

        bool IsValidRegularExpression(string str)
        {
            bool result = false;
            string[] regexPatterns = new[] {
               "[-]{0,1}[0-9]{0,}[.]{0,1}[0-9]{0,}",
               "[-]{0,1}[0-9]{0,}[.]{0,1}[0-9]{0,}[e]{1}[+]{1}[0-9]{1,}",
               "[-]{0,1}[0-9]{0,}[.]{0,1}[0-9]{0,}[e]{1}[-]{1}[0-9]{1,}",
            };

            var localizer = LocalizationSettings.GetInstance();
            String englishString = localizer.GetEnglishValueFromLocalizedDigits(str);

            for (int i = 0; i < 3; ++i)
            {
                if (Regex.Match(englishString, regexPatterns[i]).Success)
                {
                    result = true;
                    break;
                }
            }
            return result;
        }

        void DockPanelTapped(object sender, TappedRoutedEventArgs e)
        {
            int index = DockPivot.SelectedIndex;
            if (index == 1 && !IsProgrammer)
            {
                SetChildAsMemory();
            }

            m_IsLastFlyoutMemory = false;
            m_IsLastFlyoutHistory = false;
        }

        public void UnregisterEventHandlers()
        {
            expressionText.UnregisterEventHandlers();
        }

        void OnErrorLayoutCompleted(object sender, object e)
        {
            SetDefaultFocus();
        }

        void OnHistoryAccessKeyInvoked(UIElement sender, AccessKeyInvokedEventArgs args)
        {
            DockPivot.SelectedItem = HistoryPivotItem;
        }

        void OnMemoryAccessKeyInvoked(UIElement sender, AccessKeyInvokedEventArgs args)
        {
            DockPivot.SelectedItem = MemoryPivotItem;
        }

        void DockPivot_SelectionChanged(object sender, Windows.UI.Xaml.Controls.SelectionChangedEventArgs e)
        {
            if (DockPivot.SelectedIndex == 0)
            {
                // TraceLogger.GetInstance().LogHistoryBodyOpened();
            }
            else
            {
                // TraceLogger.GetInstance().LogMemoryBodyOpened();
            }
        }

    }

    delegate void FullscreenFlyoutClosedEventHandler();

    struct FontTable
    {
        public FontTable(
            string numericSystem,
            double fullFont,
            double fullFontMin,
            double portraitMin,
            double snapFont,
            double fullNumPadFont,
            double snapScientificNumPadFont,
            double portraitScientificNumPadFont)
        {
            this.numericSystem = numericSystem;
            this.fullFont = fullFont;
            this.fullFontMin = fullFontMin;
            this.portraitMin = portraitMin;
            this.snapFont = snapFont;
            this.fullNumPadFont = fullNumPadFont;
            this.snapScientificNumPadFont = snapScientificNumPadFont;
            this.portraitScientificNumPadFont = portraitScientificNumPadFont;
        }

        public string numericSystem;
        public double fullFont;
        public double fullFontMin;
        public double portraitMin;
        public double snapFont;
        public double fullNumPadFont;
        public double snapScientificNumPadFont;
        public double portraitScientificNumPadFont;
    };

}
