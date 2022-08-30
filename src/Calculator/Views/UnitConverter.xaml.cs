using CalculatorApp.Common;
using CalculatorApp.Controls;
using CalculatorApp.ViewModel;
using CalculatorApp.ViewModel.Common;

using System;
using System.ComponentModel;
using System.Diagnostics;
using System.Threading.Tasks;

using Windows.Foundation;
using Windows.System;
using Windows.UI.ViewManagement;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Automation;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Input;

// The User Control item template is documented at https://go.microsoft.com/fwlink/?LinkId=234236

namespace CalculatorApp
{
    internal class Activatable : ViewModel.IActivatable
    {
        public Activatable(Func<bool> getter, Action<bool> setter)
        {
            m_getter = getter;
            m_setter = setter;
        }

        public bool IsActive
        {
            get => m_getter();
            set => m_setter(value);
        }

        private readonly Func<bool> m_getter;
        private readonly Action<bool> m_setter;
    }

    public sealed partial class UnitConverter : UserControl
    {
        public UnitConverter()
        {
            m_meteredConnectionOverride = false;
            LayoutDirection = LocalizationService.GetInstance().GetFlowDirection();
            FlowDirectionHorizontalAlignment = LayoutDirection == FlowDirection.RightToLeft ? HorizontalAlignment.Right : HorizontalAlignment.Left;

            InitializeComponent();

            // adding ESC key shortcut binding to clear button
            ClearEntryButtonPos0.SetValue(KeyboardShortcutManager.VirtualKeyProperty, MyVirtualKey.Escape);

            // Is currency symbol preference set to right side
            bool preferRight = LocalizationSettings.GetInstance().GetCurrencySymbolPrecedence() == 0;
            VisualStateManager.GoToState(this, preferRight ? "CurrencySymbolRightState" : "CurrencySymbolLeftState", false);

            var resLoader = AppResourceProvider.GetInstance();
            m_chargesMayApplyText = resLoader.GetResourceString("DataChargesMayApply");
            m_failedToRefreshText = resLoader.GetResourceString("FailedToRefresh");

            InitializeOfflineStatusTextBlock();

            if (Resources.TryGetValue("CalculationResultContextMenu", out var value))
            {
                m_resultsFlyout = (MenuFlyout)value;
            }

            CopyMenuItem.Text = resLoader.GetResourceString("copyMenuItem");
            PasteMenuItem.Text = resLoader.GetResourceString("pasteMenuItem");
        }

        public Windows.UI.Xaml.HorizontalAlignment FlowDirectionHorizontalAlignment { get; } = default;

        public void AnimateConverter()
        {
            if (uiSettings.Value.AnimationsEnabled)
            {
                AnimationStory.Begin();
            }
        }

        public CalculatorApp.ViewModel.UnitConverterViewModel Model => (CalculatorApp.ViewModel.UnitConverterViewModel)this.DataContext;

        public Windows.UI.Xaml.FlowDirection LayoutDirection { get; } = default;

        public void SetDefaultFocus()
        {
            Control[] focusPrecedence = { Value1, CurrencyRefreshBlockControl, OfflineBlock, ClearEntryButtonPos0 };

            foreach (Control control in focusPrecedence)
            {
                if (control.Focus(FocusState.Programmatic))
                {
                    break;
                }
            }
        }

        private void OnValueKeyDown(object sender, Windows.UI.Xaml.Input.KeyRoutedEventArgs e)
        {
            if (e.Key == VirtualKey.Space)
            {
                OnValueSelected(sender);
            }
        }

        private void OnContextRequested(UIElement sender, ContextRequestedEventArgs e)
        {
            OnValueSelected(sender);
            var requestedElement = ((FrameworkElement)sender);

            PasteMenuItem.IsEnabled = CopyPasteManager.HasStringToPaste();

            if (e.TryGetPosition(requestedElement, out Point point))
            {
                m_resultsFlyout.ShowAt(requestedElement, point);
            }
            else
            {
                // Not invoked via pointer, so let XAML choose a default location.
                m_resultsFlyout.ShowAt(requestedElement);
            }

            e.Handled = true;
        }

        private void OnContextCanceled(UIElement sender, RoutedEventArgs e)
        {
            m_resultsFlyout.Hide();
        }

        private void OnCopyMenuItemClicked(object sender, RoutedEventArgs e)
        {
            var calcResult = ((CalculationResult)m_resultsFlyout.Target);
            CopyPasteManager.CopyToClipboard(calcResult.GetRawDisplayValue());
        }

        private void OnPasteMenuItemClicked(object sender, RoutedEventArgs e)
        {
            UnitConverter that = this;
            _ = Task.Run(async () =>
            {
                string pastedString = await CopyPasteManager.GetStringToPaste(Model.Mode, CategoryGroupType.Converter, NumberBase.Unknown, BitLength.BitLengthUnknown);
                that.Model.OnPaste(pastedString);
            });
        }

        private void OnValueSelected(object sender)
        {
            var value = ((CalculationResult)sender);
            // update the font size since the font is changed to bold
            value.UpdateTextState();
            ((UnitConverterViewModel)this.DataContext).OnValueActivated(new Activatable(() => value.IsActive, flag => value.IsActive = flag));
        }

        private void UpdateDropDownState(object sender, object e)
        {
            ((UnitConverterViewModel)this.DataContext).IsDropDownOpen = (Units1.IsDropDownOpen) || (Units2.IsDropDownOpen);
            KeyboardShortcutManager.UpdateDropDownState((Units1.IsDropDownOpen) || (Units2.IsDropDownOpen));
        }

        private void OnLoaded(object sender, RoutedEventArgs e)
        {
        }

        private void CurrencyRefreshButton_Click(object sender, RoutedEventArgs e)
        {
            // If IsCurrencyLoadingVisible is true that means CurrencyRefreshButton_Click was recently called
            // and is still executing. In this case there is no reason to process the click.
            if (!Model.IsCurrencyLoadingVisible)
            {
                if (Model.NetworkBehavior == NetworkAccessBehavior.OptIn)
                {
                    m_meteredConnectionOverride = true;
                }

                Model.RefreshCurrencyRatios();
            }
        }

        private void OnPropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            string propertyName = e.PropertyName;
            if (propertyName == UnitConverterViewModel.NetworkBehaviorPropertyName || propertyName == UnitConverterViewModel.CurrencyDataLoadFailedPropertyName)
            {
                OnNetworkBehaviorChanged();
            }
            else if (propertyName == UnitConverterViewModel.CurrencyDataIsWeekOldPropertyName)
            {
                SetCurrencyTimestampFontWeight();
            }
            else if (
                propertyName == UnitConverterViewModel.IsCurrencyLoadingVisiblePropertyName
                || propertyName == UnitConverterViewModel.IsCurrencyCurrentCategoryPropertyName)
            {
                OnIsDisplayVisibleChanged();
            }
        }

        private void OnDataContextChanged(FrameworkElement sender, DataContextChangedEventArgs args)
        {
            Model.PropertyChanged -= OnPropertyChanged;
            Model.PropertyChanged += OnPropertyChanged;

            OnNetworkBehaviorChanged();
        }

        private void OnIsDisplayVisibleChanged()
        {
            if (!Model.IsCurrencyCurrentCategory)
            {
                VisualStateManager.GoToState(this, UnitLoadedState.Name, false);
            }
            else
            {
                if (Model.IsCurrencyLoadingVisible)
                {
                    VisualStateManager.GoToState(this, UnitNotLoadedState.Name, false);
                    StartProgressRingWithDelay();
                }
                else
                {
                    HideProgressRing();
                    VisualStateManager.GoToState(this, !string.IsNullOrEmpty(Model.CurrencyTimestamp) ? UnitLoadedState.Name : UnitNotLoadedState.Name, true);
                }
            }
        }

        private void Units1_IsEnabledChanged(object sender, DependencyPropertyChangedEventArgs e)
        {
            if ((Units1.Visibility == Visibility.Visible) && Units1.IsEnabled)
            {
                SetDefaultFocus();
            }
        }

        private void OnNetworkBehaviorChanged()
        {
            switch (Model.NetworkBehavior)
            {
                case NetworkAccessBehavior.Normal:
                    OnNormalNetworkAccess();
                    break;
                case NetworkAccessBehavior.OptIn:
                    OnOptInNetworkAccess();
                    break;
                case NetworkAccessBehavior.Offline:
                    OnOfflineNetworkAccess();
                    break;
            }
        }

        private void OnNormalNetworkAccess()
        {
            CurrencyRefreshBlockControl.Visibility = Visibility.Visible;
            OfflineBlock.Visibility = Visibility.Collapsed;

            if (Model.CurrencyDataLoadFailed)
            {
                SetFailedToRefreshStatus();
            }
            else
            {
                SetNormalCurrencyStatus();
            }
        }

        private void OnOptInNetworkAccess()
        {
            CurrencyRefreshBlockControl.Visibility = Visibility.Visible;
            OfflineBlock.Visibility = Visibility.Collapsed;

            if (m_meteredConnectionOverride && Model.CurrencyDataLoadFailed)
            {
                SetFailedToRefreshStatus();
            }
            else
            {
                SetChargesMayApplyStatus();
            }
        }

        private void OnOfflineNetworkAccess()
        {
            CurrencyRefreshBlockControl.Visibility = Visibility.Collapsed;
            OfflineBlock.Visibility = Visibility.Visible;
        }

        private void InitializeOfflineStatusTextBlock()
        {
            var resProvider = AppResourceProvider.GetInstance();
            string offlineStatusHyperlinkText = resProvider.GetResourceString("OfflineStatusHyperlinkText");

            // The resource string has the 'NetworkSettings' hyperlink wrapped with '%HL%'.
            // Break the string and assign pieces appropriately.
            const string delimiter = "%HL%";
            int delimiterLength = delimiter.Length;

            // Find the delimiters.
            int firstSplitPosition = offlineStatusHyperlinkText.IndexOf(delimiter);
            Debug.Assert(firstSplitPosition != -1);
            int secondSplitPosition = offlineStatusHyperlinkText.IndexOf(delimiter, firstSplitPosition + 1);
            Debug.Assert(secondSplitPosition != -1);
            int hyperlinkTextLength = secondSplitPosition - (firstSplitPosition + delimiterLength);

            // Assign pieces.
            var offlineStatusTextBeforeHyperlink = offlineStatusHyperlinkText.Substring(0, firstSplitPosition);
            var offlineStatusTextLink = offlineStatusHyperlinkText.Substring(firstSplitPosition + delimiterLength, hyperlinkTextLength);
            var offlineStatusTextAfterHyperlink = offlineStatusHyperlinkText.Substring(secondSplitPosition + delimiterLength);

            OfflineRunBeforeLink.Text = offlineStatusTextBeforeHyperlink;
            OfflineRunLink.Text = offlineStatusTextLink;
            OfflineRunAfterLink.Text = offlineStatusTextAfterHyperlink;

            AutomationProperties.SetName(OfflineBlock, offlineStatusTextBeforeHyperlink + " " + offlineStatusTextLink + " " + offlineStatusTextAfterHyperlink);
        }

        private void SetNormalCurrencyStatus()
        {
            CurrencySecondaryStatus.Text = "";
        }

        private void SetChargesMayApplyStatus()
        {
            VisualStateManager.GoToState(this, "ChargesMayApplyCurrencyStatus", false);
            CurrencySecondaryStatus.Text = m_chargesMayApplyText;
        }

        private void SetFailedToRefreshStatus()
        {
            VisualStateManager.GoToState(this, "FailedCurrencyStatus", false);
            CurrencySecondaryStatus.Text = m_failedToRefreshText;
        }

        private void SetCurrencyTimestampFontWeight()
        {
            if (Model.CurrencyDataIsWeekOld)
            {
                VisualStateManager.GoToState(this, "WeekOldTimestamp", false);
            }
            else
            {
                VisualStateManager.GoToState(this, "DefaultTimestamp", false);
            }
        }

        private void StartProgressRingWithDelay()
        {
            HideProgressRing();

            TimeSpan delay = TimeSpan.FromMilliseconds(500);

            m_delayTimer = new DispatcherTimer
            {
                Interval = delay
            };
            m_delayTimer.Tick += OnDelayTimerTick;

            m_delayTimer.Start();
        }

        private void OnDelayTimerTick(object sender, object e)
        {
            CurrencyLoadingProgressRing.IsActive = true;
            m_delayTimer.Stop();
        }

        private void HideProgressRing()
        {
            m_delayTimer?.Stop();

            CurrencyLoadingProgressRing.IsActive = false;
        }

        private void SupplementaryResultsPanelInGrid_SizeChanged(object sender, Windows.UI.Xaml.SizeChangedEventArgs e)
        {
            // We add 0.01 to be sure to not create an infinite loop with SizeChanged events cascading due to float approximation
            RowDltrUnits.MinHeight = Math.Max(48.0, e.NewSize.Height + 0.01);
        }

        private void OnVisualStateChanged(object sender, Windows.UI.Xaml.VisualStateChangedEventArgs e)
        {
            var mode = NavCategoryStates.Deserialize(Model.CurrentCategory.GetModelCategoryId());
            TraceLogger.GetInstance().LogVisualStateChanged(mode, e.NewState.Name, false);
        }

        private static readonly Lazy<UISettings> uiSettings = new Lazy<UISettings>(true);
        private readonly Windows.UI.Xaml.Controls.MenuFlyout m_resultsFlyout = default;

        private readonly string m_chargesMayApplyText;
        private readonly string m_failedToRefreshText;

        private bool m_meteredConnectionOverride;

        private Windows.UI.Xaml.DispatcherTimer m_delayTimer;
    }
}

