// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using CalculatorApp.Utils;
using CalculatorApp.ViewModel;
using CalculatorApp.ViewModel.Common;

using System;

using Windows.Foundation;
using Windows.Globalization.NumberFormatting;
using Windows.UI.Core;
using Windows.UI.ViewManagement;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Automation;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Input;

namespace CalculatorApp
{
    public class FontTable
    {
        public string numericSystem;
        public double fullFont;
        public double fullFontMin;
        public double portraitMin;
        public double snapFont;
        public double fullNumPadFont;
        public double snapScientificNumPadFont;
        public double portraitScientificNumPadFont;
    }

    public delegate void FullscreenFlyoutClosedEventHandler();

    [Windows.Foundation.Metadata.WebHostHidden]
    public sealed partial class Calculator
    {
        public event FullscreenFlyoutClosedEventHandler FullscreenFlyoutClosed;

        public Calculator()
        {
            m_doAnimate = false;
            m_isLastAnimatedInScientific = false;
            m_isLastAnimatedInProgrammer = false;
            m_resultAnimate = false;

            SetFontSizeResources();
            InitializeComponent();
            LoadResourceStrings();

            if (LocalizationService.GetInstance().IsRtlLayout())
            {
                HistoryButton.HorizontalAlignment = HorizontalAlignment.Left;
            }

            m_displayFlyout = (MenuFlyout)Resources["DisplayContextMenu"];
            var resLoader = AppResourceProvider.GetInstance();
            CopyMenuItem.Text = resLoader.GetResourceString("copyMenuItem");
            PasteMenuItem.Text = resLoader.GetResourceString("pasteMenuItem");

            this.SizeChanged += Calculator_SizeChanged;
        }

        public CalculatorApp.ViewModel.StandardCalculatorViewModel Model => (StandardCalculatorViewModel)this.DataContext;

        public bool IsStandard
        {
            get => (bool)GetValue(IsStandardProperty);
            set => SetValue(IsStandardProperty, value);
        }

        // Using a DependencyProperty as the backing store for IsStandard.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty IsStandardProperty =
            DependencyProperty.Register(nameof(IsStandard), typeof(bool), typeof(Calculator), new PropertyMetadata(false, (sender, args) =>
            {
                var self = (Calculator)sender;
                self.OnIsStandardPropertyChanged((bool)args.OldValue, (bool)args.NewValue);
            }));

        public bool IsScientific
        {
            get => (bool)GetValue(IsScientificProperty);
            set => SetValue(IsScientificProperty, value);
        }

        // Using a DependencyProperty as the backing store for IsScientific.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty IsScientificProperty =
            DependencyProperty.Register(nameof(IsScientific), typeof(bool), typeof(Calculator), new PropertyMetadata(false, (sender, args) =>
            {
                var self = (Calculator)sender;
                self.OnIsScientificPropertyChanged((bool)args.OldValue, (bool)args.NewValue);
            }));

        public bool IsProgrammer
        {
            get => (bool)GetValue(IsProgrammerProperty);
            set => SetValue(IsProgrammerProperty, value);
        }

        // Using a DependencyProperty as the backing store for IsProgrammer.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty IsProgrammerProperty =
            DependencyProperty.Register(nameof(IsProgrammer), typeof(bool), typeof(Calculator), new PropertyMetadata(false, (sender, args) =>
            {
                var self = (Calculator)sender;
                self.OnIsProgrammerPropertyChanged((bool)args.OldValue, (bool)args.NewValue);
            }));

        public bool IsAlwaysOnTop
        {
            get => (bool)GetValue(IsAlwaysOnTopProperty);
            set => SetValue(IsAlwaysOnTopProperty, value);
        }

        // Using a DependencyProperty as the backing store for IsAlwaysOnTop.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty IsAlwaysOnTopProperty =
            DependencyProperty.Register(nameof(IsAlwaysOnTop), typeof(bool), typeof(Calculator), new PropertyMetadata(false, (sender, args) =>
            {
                var self = (Calculator)sender;
                self.OnIsAlwaysOnTopPropertyChanged((bool)args.OldValue, (bool)args.NewValue);
            }));

        public System.Windows.Input.ICommand HistoryButtonPressed
        {
            get
            {
                if (donotuse_HistoryButtonPressed == null)
                {
                    donotuse_HistoryButtonPressed = DelegateCommandUtils.MakeDelegateCommand(this,
                        (that, param) =>
                        {
                            that.ToggleHistoryFlyout(param);
                        });
                }
                return donotuse_HistoryButtonPressed;
            }
        }
        private System.Windows.Input.ICommand donotuse_HistoryButtonPressed;

        private static readonly UISettings uiSettings = new UISettings();
        public void AnimateCalculator(bool resultAnimate)
        {
            if (uiSettings.AnimationsEnabled)
            {
                m_doAnimate = true;
                m_resultAnimate = resultAnimate;
                if (((m_isLastAnimatedInScientific && IsScientific) || (!m_isLastAnimatedInScientific && !IsScientific))
                    & ((m_isLastAnimatedInProgrammer && IsProgrammer) || (!m_isLastAnimatedInProgrammer && !IsProgrammer)))
                {
                    // We are forcing the animation here
                    // It's because if last animation was in standard, then go to unit converter, then comes back to standard
                    // The state for the calculator does not change and the animation would not get run.
                    this.OnModeVisualStateCompleted(null, null);
                }
            }
        }

        public void InitializeHistoryView(CalculatorApp.ViewModel.HistoryViewModel historyVM)
        {
            if (m_historyList == null)
            {
                m_historyList = new HistoryList
                {
                    DataContext = historyVM
                };
                historyVM.HideHistoryClicked += OnHideHistoryClicked;
                historyVM.HistoryItemClicked += OnHistoryItemClicked;
            }
        }

        public void UpdatePanelViewState()
        {
            UpdateHistoryState();
            UpdateMemoryState();
        }

        public void UnregisterEventHandlers()
        {
            ExpressionText.UnregisterEventHandlers();
            AlwaysOnTopResults.UnregisterEventHandlers();
        }

        public void CloseHistoryFlyout()
        {
            if (m_fIsHistoryFlyoutOpen)
            {
                HistoryFlyout.Hide();
            }
        }

        public void CloseMemoryFlyout()
        {
            if (m_fIsMemoryFlyoutOpen)
            {
                MemoryFlyout.Hide();
            }
        }

        public void SetDefaultFocus()
        {
            if (!IsAlwaysOnTop)
            {
                Results.Focus(FocusState.Programmatic);
            }
            else
            {
                AlwaysOnTopResults.Focus(FocusState.Programmatic);
            }
        }

        // Methods used by native bindings
        public static Visibility ShouldDisplayHistoryButton(bool isAlwaysOnTop, bool isProgrammer, Visibility dockPanelVisibility)
        {
            return !isAlwaysOnTop && !isProgrammer && dockPanelVisibility == Visibility.Collapsed ? Visibility.Visible : Visibility.Collapsed;
        }

        private void OnLoaded(object sender, RoutedEventArgs e)
        {
            Model.PropertyChanged += OnCalcPropertyChanged;
            Model.HideMemoryClicked += OnHideMemoryClicked;

            InitializeHistoryView(Model.HistoryVM);
            string historyPaneName = AppResourceProvider.GetInstance().GetResourceString("HistoryPane");
            HistoryFlyout.FlyoutPresenterStyle.Setters.Add(new Setter(AutomationProperties.NameProperty, historyPaneName));
            string memoryPaneName = AppResourceProvider.GetInstance().GetResourceString("MemoryPane");
            MemoryFlyout.FlyoutPresenterStyle.Setters.Add(new Setter(AutomationProperties.NameProperty, memoryPaneName));

            // Delay load things later when we get a chance.
            WeakReference weakThis = new WeakReference(this);
            _ = this.Dispatcher.RunAsync(
                CoreDispatcherPriority.Normal, () =>
                {
                    if (TraceLogger.GetInstance().IsWindowIdInLog(ApplicationView.GetApplicationViewIdForWindow(CoreWindow.GetForCurrentThread())))
                    {
                        if (weakThis.Target is Calculator refThis)
                        {
                            refThis.GetMemory();
                        }
                    }
                });
        }

        private void LoadResourceStrings()
        {
            var resProvider = AppResourceProvider.GetInstance();
            m_openMemoryFlyoutAutomationName = resProvider.GetResourceString("MemoryButton_Open");
            m_closeMemoryFlyoutAutomationName = resProvider.GetResourceString("MemoryButton_Close");
            m_openHistoryFlyoutAutomationName = resProvider.GetResourceString("HistoryButton_Open");
            m_closeHistoryFlyoutAutomationName = resProvider.GetResourceString("HistoryButton_Close");
            m_dockPanelHistoryMemoryLists = resProvider.GetResourceString("DockPanel_HistoryMemoryLists");
            m_dockPanelMemoryList = resProvider.GetResourceString("DockPanel_MemoryList");
            AutomationProperties.SetName(MemoryButton, m_openMemoryFlyoutAutomationName);
            AutomationProperties.SetName(HistoryButton, m_openHistoryFlyoutAutomationName);
            AutomationProperties.SetName(DockPanel, m_dockPanelHistoryMemoryLists);
        }

        private void UpdateViewState()
        {
            string state;
            if (IsProgrammer)
            {
                state = "Programmer";
                Model.IsDecimalEnabled = false;
                ResultsMVisualStateTrigger.MinWindowHeight = 640;
            }
            else if (IsScientific)
            {
                state = "Scientific";
                Model.IsDecimalEnabled = true;
                ResultsMVisualStateTrigger.MinWindowHeight = 544;
            }
            else
            {
                state = "Standard";
                Model.IsDecimalEnabled = true;
                ResultsMVisualStateTrigger.MinWindowHeight = 1;
            }

            CloseHistoryFlyout();
            CloseMemoryFlyout();

            VisualStateManager.GoToState(this, state, true);
        }

        private void UpdateMemoryState()
        {
            if (!IsAlwaysOnTop)
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

                if (DockPanel.Visibility == Visibility.Visible)
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
        }

        private void UpdateHistoryState()
        {
            if (DockPanel.Visibility == Visibility.Visible)
            {
                // docked view
                CloseHistoryFlyout();
                SetChildAsHistory();

                if (!IsProgrammer && m_IsLastFlyoutHistory)
                {
                    DockPivot.SelectedIndex = 0;
                }
            }
            else
            {
                // flyout view
                DockHistoryHolder.Child = null;
            }
        }

        private void OnContextRequested(UIElement sender, ContextRequestedEventArgs e)
        {
            var requestedElement = (FrameworkElement)e.OriginalSource;

            PasteMenuItem.IsEnabled = CopyPasteManager.HasStringToPaste();

            if (e.TryGetPosition(requestedElement, out Point point))
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

        private void OnContextCanceled(UIElement sender, RoutedEventArgs e)
        {
            m_displayFlyout.Hide();
        }

        private void OnIsScientificPropertyChanged(bool oldValue, bool newValue)
        {
            if (newValue)
            {
                EnsureScientific();
            }

            UpdateViewState();
            UpdatePanelViewState();
        }

        private void OnIsProgrammerPropertyChanged(bool oldValue, bool newValue)
        {
            if (newValue)
            {
                EnsureProgrammer();
                m_pivotItem = (PivotItem)DockPivot.Items[0];
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

        private void OnIsStandardPropertyChanged(bool oldValue, bool newValue)
        {
            UpdateViewState();
            UpdatePanelViewState();
        }

        private void OnIsAlwaysOnTopPropertyChanged(bool oldValue, bool newValue)
        {
            if (newValue)
            {
                VisualStateManager.GoToState(this, "DisplayModeAlwaysOnTop", false);
                AlwaysOnTopResults.UpdateScrollButtons();
            }
            else
            {
                VisualStateManager.GoToState(this, "DisplayModeNormal", false);
                if (!Model.IsInError)
                {
                    EnableMemoryControls(true);
                }
                Results.UpdateTextState();
            }

            Model.IsMemoryEmpty = (Model.MemorizedNumbers.Count == 0) || IsAlwaysOnTop;

            UpdateViewState();
            UpdatePanelViewState();
        }

        private void OnIsInErrorPropertyChanged()
        {
            bool isError = Model.IsInError;

            string newState = isError ? "ErrorLayout" : "NoErrorLayout";
            VisualStateManager.GoToState(this, newState, false);

            if (m_memory != null)
            {
                m_memory.IsErrorVisualState = isError;
            }

            OpsPanel.IsErrorVisualState = isError;
            if (IsScientific && ScientificAngleButtons != null)
            {
                ScientificAngleButtons.IsErrorVisualState = isError;
            }
            else if (IsProgrammer && ProgrammerDisplayPanel != null)
            {
                ProgrammerDisplayPanel.IsErrorVisualState = isError;
            }
        }

        private void OnCalcPropertyChanged(object sender, System.ComponentModel.PropertyChangedEventArgs e)
        {
            string prop = e.PropertyName;
            if (prop == StandardCalculatorViewModel.IsMemoryEmptyPropertyName)
            {
                UpdateMemoryState();
            }
            else if (prop == StandardCalculatorViewModel.IsInErrorPropertyName)
            {
                OnIsInErrorPropertyChanged();
            }
        }

        private void OnLayoutVisualStateCompleted(object sender, object e)
        {
            UpdatePanelViewState();
        }

        private void OnModeVisualStateCompleted(object sender, object e)
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
            if (IsProgrammer)
            {
                AutomationProperties.SetName(DockPanel, m_dockPanelMemoryList);
            }
            else
            {
                AutomationProperties.SetName(DockPanel, m_dockPanelHistoryMemoryLists);
            }
        }

        private void OnErrorVisualStateCompleted(object sender, object e)
        {
            SetDefaultFocus();
        }

        private void OnDisplayVisualStateCompleted(object sender, object e)
        {
            SetDefaultFocus();
        }

        private void EnsureScientific()
        {
            OpsPanel.EnsureScientificOps();

            if (ScientificAngleButtons == null)
            {
                this.FindName("ScientificAngleButtons");
            }
        }

        private void EnsureProgrammer()
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
            ProgrammerOperators.SetRadixButton(Model.CurrentRadixType);
        }

        // Since we need different font sizes for different numeric system,
        // we use a table of optimal font sizes for each numeric system.
        private static readonly FontTable[] fontTables = {
            new FontTable { numericSystem = "Arab", fullFont = 104, fullFontMin = 29.333, portraitMin = 23, snapFont = 40,
                            fullNumPadFont = 56, snapScientificNumPadFont = 40, portraitScientificNumPadFont = 56 },
            new FontTable { numericSystem = "ArabExt", fullFont = 104, fullFontMin = 29.333, portraitMin = 23, snapFont = 40,
                            fullNumPadFont = 56, snapScientificNumPadFont = 40, portraitScientificNumPadFont = 56 },
            new FontTable { numericSystem = "Beng", fullFont = 104, fullFontMin = 26, portraitMin = 17, snapFont = 40,
                            fullNumPadFont = 56, snapScientificNumPadFont = 40, portraitScientificNumPadFont = 56 },
            new FontTable { numericSystem = "Deva", fullFont = 104, fullFontMin = 29.333, portraitMin = 20.5, snapFont = 40,
                            fullNumPadFont = 56, snapScientificNumPadFont = 40, portraitScientificNumPadFont = 56 },
            new FontTable { numericSystem = "Gujr", fullFont = 104, fullFontMin = 29.333, portraitMin = 18.5, snapFont = 40,
                            fullNumPadFont = 56, snapScientificNumPadFont = 40, portraitScientificNumPadFont = 56 },
            new FontTable { numericSystem = "Khmr", fullFont = 104, fullFontMin = 29.333, portraitMin = 19.5, snapFont = 40,
                            fullNumPadFont = 56, snapScientificNumPadFont = 40, portraitScientificNumPadFont = 56 },
            new FontTable { numericSystem = "Knda", fullFont = 104, fullFontMin = 25, portraitMin = 17, snapFont = 40,
                            fullNumPadFont = 56, snapScientificNumPadFont = 40, portraitScientificNumPadFont = 56 },
            new FontTable { numericSystem = "Laoo", fullFont = 104, fullFontMin = 28, portraitMin = 18, snapFont = 40,
                            fullNumPadFont = 56, snapScientificNumPadFont = 40, portraitScientificNumPadFont = 56 },
            new FontTable { numericSystem = "Latn", fullFont = 104, fullFontMin = 29.333, portraitMin = 23, snapFont = 40,
                            fullNumPadFont = 56, snapScientificNumPadFont = 40, portraitScientificNumPadFont = 56 },
            new FontTable { numericSystem = "Mlym", fullFont = 80, fullFontMin = 22, portraitMin = 15.5, snapFont = 30,
                            fullNumPadFont = 56, snapScientificNumPadFont = 35, portraitScientificNumPadFont = 48 },
            new FontTable { numericSystem = "Mymr", fullFont = 104, fullFontMin = 29.333, portraitMin = 20, snapFont = 35,
                            fullNumPadFont = 48, snapScientificNumPadFont = 36, portraitScientificNumPadFont = 48 },
            new FontTable { numericSystem = "Orya", fullFont = 88, fullFontMin = 26, portraitMin = 20, snapFont = 40,
                            fullNumPadFont = 56, snapScientificNumPadFont = 40, portraitScientificNumPadFont = 56 },
            new FontTable { numericSystem = "TamlDec", fullFont = 77, fullFontMin = 25, portraitMin = 16, snapFont = 28,
                            fullNumPadFont = 48, snapScientificNumPadFont = 34, portraitScientificNumPadFont = 48 },
            new FontTable { numericSystem = "Telu", fullFont = 104, fullFontMin = 25, portraitMin = 16.5, snapFont = 40,
                            fullNumPadFont = 56, snapScientificNumPadFont = 40, portraitScientificNumPadFont = 56 },
            new FontTable { numericSystem = "Thai", fullFont = 104, fullFontMin = 28, portraitMin = 18, snapFont = 40,
                            fullNumPadFont = 56, snapScientificNumPadFont = 40, portraitScientificNumPadFont = 56 },
            new FontTable { numericSystem = "Tibt", fullFont = 104, fullFontMin = 29.333, portraitMin = 20, snapFont = 40,
                           fullNumPadFont =  56, snapScientificNumPadFont = 40, portraitScientificNumPadFont = 56 },
            new FontTable { numericSystem = "Default", fullFont = 104, fullFontMin = 29.333, portraitMin = 23, snapFont = 40,
                           fullNumPadFont =  56, snapScientificNumPadFont = 40, portraitScientificNumPadFont = 56 }
        };

        private void SetFontSizeResources()
        {
            DecimalFormatter formatter = LocalizationService.GetInstance().GetRegionalSettingsAwareDecimalFormatter();

            int currentItemIdx = 0;
            while (!fontTables[currentItemIdx].numericSystem.Equals("Default") &&
                   !fontTables[currentItemIdx].numericSystem.Equals(formatter.NumeralSystem))
            {
                ++currentItemIdx;
            }

            var currentItem = fontTables[currentItemIdx];
            this.Resources.Add("ResultFullFontSize", currentItem.fullFont);
            this.Resources.Add("ResultFullMinFontSize", currentItem.fullFontMin);
            this.Resources.Add("ResultPortraitMinFontSize", currentItem.portraitMin);
            this.Resources.Add("ResultSnapFontSize", currentItem.snapFont);
            this.Resources.Add("CalcButtonCaptionSizeOverride", currentItem.fullNumPadFont);
            this.Resources.Add("CalcButtonScientificSnapCaptionSizeOverride", currentItem.snapScientificNumPadFont);
            this.Resources.Add("CalcButtonScientificPortraitCaptionSizeOverride", currentItem.portraitScientificNumPadFont);
        }

        private string GetCurrentLayoutState()
        {
            if (IsProgrammer)
            {
                return "Programmer";
            }
            else if (IsScientific)
            {
                return "Scientific";
            }
            else
            {
                return "Standard";
            }
        }

        private void Calculator_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            if (Model.IsAlwaysOnTop)
            {
                AlwaysOnTopResults.UpdateScrollButtons();
            }
        }

        private readonly Windows.UI.Xaml.Controls.MenuFlyout m_displayFlyout;
        private bool m_doAnimate;
        private bool m_resultAnimate;
        private bool m_isLastAnimatedInScientific;
        private bool m_isLastAnimatedInProgrammer;
        private bool m_IsLastFlyoutMemory = false;
        private bool m_IsLastFlyoutHistory = false;

        private string m_openMemoryFlyoutAutomationName;
        private string m_closeMemoryFlyoutAutomationName;
        private string m_openHistoryFlyoutAutomationName;
        private string m_closeHistoryFlyoutAutomationName;
        private string m_dockPanelHistoryMemoryLists;
        private string m_dockPanelMemoryList;

        private Windows.UI.Xaml.Controls.PivotItem m_pivotItem;
        private Memory m_memory;

        private void HistoryFlyout_Opened(object sender, object args)
        {
            m_fIsHistoryFlyoutOpen = true;
            m_IsLastFlyoutMemory = false;
            m_IsLastFlyoutHistory = true;
            EnableControls(false);
            AutomationProperties.SetName(HistoryButton, m_closeHistoryFlyoutAutomationName);
        }

        private void HistoryFlyout_Closing(FlyoutBase sender, FlyoutBaseClosingEventArgs args)
        {
            // Set in the Closing event so the new name is available when the Flyout has Closed.
            AutomationProperties.SetName(HistoryButton, m_openHistoryFlyoutAutomationName);
        }

        private void HistoryFlyout_Closed(object sender, object args)
        {
            m_fIsHistoryFlyoutOpen = false;
            EnableControls(true);
            if (HistoryButton.IsEnabled && HistoryButton.Visibility == Visibility.Visible)
            {
                HistoryButton.Focus(FocusState.Programmatic);
            }

            FullscreenFlyoutClosed?.Invoke();
        }

        private void OnHideHistoryClicked()
        {
            ToggleHistoryFlyout(null);
        }

        private void OnHideMemoryClicked()
        {
            if (!m_fIsMemoryFlyoutOpen)
            {
                this.Focus(FocusState.Programmatic);
            }

            MemoryFlyout.Hide();
        }

        private void OnHistoryItemClicked(HistoryItemViewModel e)
        {
            Model.SelectHistoryItem(e);

            CloseHistoryFlyout();
            this.Focus(FocusState.Programmatic);
        }

        private void ToggleHistoryFlyout(object parameter)
        {
            if (Model.IsProgrammer || DockPanel.Visibility == Visibility.Visible)
            {
                return;
            }

            if (m_fIsHistoryFlyoutOpen)
            {
                HistoryFlyout.Hide();
            }
            else
            {
                HistoryFlyout.Content = m_historyList;
                m_historyList.RowHeight = new GridLength(NumpadPanel.ActualHeight);
                FlyoutBase.ShowAttachedFlyout(HistoryButton);
            }
        }

        private void ToggleMemoryFlyout(object sender, RoutedEventArgs e)
        {
            if (DockPanel.Visibility == Visibility.Visible)
            {
                return;
            }

            if (m_fIsMemoryFlyoutOpen)
            {
                MemoryFlyout.Hide();
            }
            else
            {
                MemoryFlyout.Content = GetMemory();
                m_memory.RowHeight = new GridLength(NumpadPanel.ActualHeight);
                FlyoutBase.ShowAttachedFlyout(MemoryButton);
            }
        }

        private CalculatorApp.HistoryList m_historyList;
        private bool m_fIsHistoryFlyoutOpen;
        private bool m_fIsMemoryFlyoutOpen;

        private void OnMemoryFlyoutOpened(object sender, object args)
        {
            m_IsLastFlyoutMemory = true;
            m_IsLastFlyoutHistory = false;
            m_fIsMemoryFlyoutOpen = true;
            AutomationProperties.SetName(MemoryButton, m_closeMemoryFlyoutAutomationName);
            EnableControls(false);
        }

        private void OnMemoryFlyoutClosing(FlyoutBase sender, FlyoutBaseClosingEventArgs args)
        {
            // Set in the Closing event so the new name is available when the Flyout has Closed.
            AutomationProperties.SetName(MemoryButton, m_openMemoryFlyoutAutomationName);
        }

        private void OnMemoryFlyoutClosed(object sender, object args)
        {
            m_fIsMemoryFlyoutOpen = false;
            EnableControls(true);
            if (MemoryButton.IsEnabled)
            {
                MemoryButton.Focus(FocusState.Programmatic);
            }

            FullscreenFlyoutClosed?.Invoke();
        }

        private void SetChildAsMemory()
        {
            DockMemoryHolder.Child = GetMemory();
        }

        private void SetChildAsHistory()
        {
            if (m_historyList == null)
            {
                InitializeHistoryView(Model.HistoryVM);
            }

            DockHistoryHolder.Child = m_historyList;
        }

        private Memory GetMemory()
        {
            if (m_memory == null)
            {
                m_memory = new Memory();
                VisualStateManager.GoToState(m_memory, GetCurrentLayoutState(), true);
            }

            return m_memory;
        }

        private void EnableControls(bool enable)
        {
            OpsPanel.IsEnabled = enable;
            EnableMemoryControls(enable);
        }

        private void EnableMemoryControls(bool enable)
        {
            MemButton.IsEnabled = enable;
            MemMinus.IsEnabled = enable;
            MemPlus.IsEnabled = enable;
            if (!Model.IsMemoryEmpty)
            {
                MemRecall.IsEnabled = enable;
                ClearMemoryButton.IsEnabled = enable;
            }
        }

        private void OnMemoryFlyOutTapped(object sender, TappedRoutedEventArgs e)
        {
            Grid grid = (Grid)sender;
            Point point = e.GetPosition(null);

            if (point.Y < (grid.ActualHeight - NumpadPanel.ActualHeight))
            {
                MemoryFlyout.Hide();
            }
        }

        private void OnHistoryFlyOutTapped(object sender, TappedRoutedEventArgs e)
        {
            Grid grid = (Grid)sender;
            Point point = e.GetPosition(null);

            if (point.Y < (grid.ActualHeight - NumpadPanel.ActualHeight))
            {
                HistoryFlyout.Hide();
            }
        }

        private void DockPanelTapped(object sender, TappedRoutedEventArgs e)
        {
            int index = DockPivot.SelectedIndex;
            if (index == 1 && !IsProgrammer)
            {
                SetChildAsMemory();
            }

            m_IsLastFlyoutMemory = false;
            m_IsLastFlyoutHistory = false;
        }

        private void OnHistoryAccessKeyInvoked(UIElement sender, AccessKeyInvokedEventArgs args)
        {
            DockPivot.SelectedItem = HistoryPivotItem;
        }

        private void OnMemoryAccessKeyInvoked(UIElement sender, AccessKeyInvokedEventArgs args)
        {
            DockPivot.SelectedItem = MemoryPivotItem;
        }

        private void OnVisualStateChanged(object sender, VisualStateChangedEventArgs e)
        {
            if (!IsStandard && !IsScientific && !IsProgrammer)
            {
                return;
            }

            var mode = IsStandard ? ViewMode.Standard : IsScientific ? ViewMode.Scientific : ViewMode.Programmer;
            TraceLogger.GetInstance().LogVisualStateChanged(mode, e.NewState.Name, IsAlwaysOnTop);
        }
    }
}
