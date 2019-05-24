// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.System;
using Windows.UI.ViewManagement;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Automation;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;
using Calculator;
using CalculatorApp.Common;
using CalculatorApp.Controls;
using CalculatorApp.ViewModel;
using UnitConversionManager;

// The User Control item template is documented at https://go.microsoft.com/fwlink/?LinkId=234236

namespace CalculatorApp
{
	public sealed partial class UnitConverter : UserControl
	{
		const long DURATION_500_MS = 10000 * 500;

		Windows.UI.Xaml.FlowDirection m_layoutDirection;
		//Windows.Foundation.EventRegistrationToken m_propertyChangedToken;
		Windows.UI.Xaml.Controls.MenuFlyout m_resultsFlyout;

		string m_chargesMayApplyText;
		string m_failedToRefreshText;

		bool m_meteredConnectionOverride;

		Windows.UI.Xaml.DispatcherTimer m_delayTimer;

		bool m_isAnimationEnabled;

		// TODO UNO: DEPENDENCY_PROPERTY_OWNER(UnitConverter);

		private HorizontalAlignment m_FlowDirectionHorizontalAlignment;
		public HorizontalAlignment FlowDirectionHorizontalAlignment
		{
			get => m_FlowDirectionHorizontalAlignment;
			set => m_FlowDirectionHorizontalAlignment = value;
		}

		public UnitConverterViewModel Model
		{
			get { return (UnitConverterViewModel)this.DataContext; }
		}

		public FlowDirection LayoutDirection

		{
			get { return m_layoutDirection;
			 }
		}

		public UnitConverter()
		{
			m_meteredConnectionOverride = false;
			m_isAnimationEnabled = false;

			m_layoutDirection = LocalizationService.GetInstance().GetFlowDirection();
			m_FlowDirectionHorizontalAlignment = m_layoutDirection == FlowDirection.RightToLeft ? HorizontalAlignment.Right : HorizontalAlignment.Left;

			this.InitializeComponent();

			// adding ESC key shortcut binding to clear button
			ClearEntryButtonPos0.SetValue(Common.KeyboardShortcutManager.VirtualKeyProperty, Common.MyVirtualKey.Escape);

			// Is currency symbol preference set to right side
			bool preferRight = LocalizationSettings.GetInstance().GetCurrencySymbolPrecedence() == 0;
			VisualStateManager.GoToState(this, preferRight ? "CurrencySymbolRightState" : "CurrencySymbolLeftState", false);

			var userSettings = new UISettings();
			m_isAnimationEnabled = userSettings.AnimationsEnabled;

			var resLoader = AppResourceProvider.GetInstance();
			m_chargesMayApplyText = resLoader.GetResourceString("DataChargesMayApply");
			m_failedToRefreshText = resLoader.GetResourceString("FailedToRefresh");

			InitializeOfflineStatusTextBlock();

			// TODO UNO: m_resultsFlyout = (MenuFlyout) (Resources["CalculationResultContextMenu"]);
			// CopyMenuItem.Text = resLoader.GetResourceString("copyMenuItem");
			// PasteMenuItem.Text = resLoader.GetResourceString("pasteMenuItem");
		}

		void OnPropertyChanged( object sender,  PropertyChangedEventArgs e)
		{
			String propertyName = e.PropertyName;
			if (propertyName == UnitConverterViewModel.NetworkBehaviorPropertyName || propertyName == UnitConverterViewModel.CurrencyDataLoadFailedPropertyName)
			{
				OnNetworkBehaviorChanged();
			}
			else if (propertyName == UnitConverterViewModel.CurrencyDataIsWeekOldPropertyName)
			{
				SetCurrencyTimestampFontWeight();
			}
			else if (propertyName == UnitConverterViewModel.IsCurrencyLoadingVisiblePropertyName)
			{
				OnIsDisplayVisibleChanged();
			}
		}

		void OnNetworkBehaviorChanged()
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

		void OnNormalNetworkAccess()
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

		void OnOptInNetworkAccess()
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

		void OnOfflineNetworkAccess()
		{
			CurrencyRefreshBlockControl.Visibility = Visibility.Collapsed;
			OfflineBlock.Visibility = Visibility.Visible;
		}

		void SetNormalCurrencyStatus()
		{
			CurrencySecondaryStatus.Text = "";
		}

		void SetChargesMayApplyStatus()
		{
			VisualStateManager.GoToState(this, "ChargesMayApplyCurrencyStatus", false);
			CurrencySecondaryStatus.Text = m_chargesMayApplyText;
		}

		void SetFailedToRefreshStatus()
		{
			VisualStateManager.GoToState(this, "FailedCurrencyStatus", false);
			CurrencySecondaryStatus.Text = m_failedToRefreshText;
		}

		void InitializeOfflineStatusTextBlock()
		{
			var resProvider = AppResourceProvider.GetInstance();
			string offlineStatusHyperlinkText = resProvider.GetResourceString("OfflineStatusHyperlinkText");

			// The resource string has the 'NetworkSettings' hyperlink wrapped with '%HL%'.
			// Break the string and assign pieces appropriately.
			string delimiter = "%HL%" ;
			int delimiterLength = delimiter.Length;

			// Find the delimiters.
			int firstSplitPosition = offlineStatusHyperlinkText.find(delimiter, 0);
			System.Diagnostics.Debug.Assert(firstSplitPosition != "".npos());
			int secondSplitPosition = offlineStatusHyperlinkText.find(delimiter, firstSplitPosition + 1);
			System.Diagnostics.Debug.Assert(secondSplitPosition != "".npos());
			int hyperlinkTextLength = secondSplitPosition - (firstSplitPosition + delimiterLength);

			// Assign pieces.
			var offlineStatusTextBeforeHyperlink = offlineStatusHyperlinkText.substr(0, firstSplitPosition);
			var offlineStatusTextLink = offlineStatusHyperlinkText.substr(firstSplitPosition + delimiterLength, hyperlinkTextLength);
			var offlineStatusTextAfterHyperlink = offlineStatusHyperlinkText.substr(secondSplitPosition + delimiterLength);

			OfflineRunBeforeLink.Text = offlineStatusTextBeforeHyperlink;
			OfflineRunLink.Text = offlineStatusTextLink;
			OfflineRunAfterLink.Text = offlineStatusTextAfterHyperlink;

			AutomationProperties.SetName(OfflineBlock, offlineStatusTextBeforeHyperlink + " " + offlineStatusTextLink + " " + offlineStatusTextAfterHyperlink);
		}

		void SetCurrencyTimestampFontWeight()
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

		void OnValueKeyDown(object sender, Windows.UI.Xaml.Input.KeyRoutedEventArgs e)
		{
			if (e.Key == VirtualKey.Space)
			{
				OnValueSelected(sender);
			}
		}

		void OnContextRequested(UIElement sender, ContextRequestedEventArgs e)
		{
			OnValueSelected(sender);
			var requestedElement = sender as FrameworkElement;

			// TODO UNO
			//PasteMenuItem.IsEnabled = CopyPasteManager.HasStringToPaste();

			Point point;
			if (e.TryGetPosition(requestedElement, out point))
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

		void OnContextCanceled(UIElement sender, RoutedEventArgs e)
		{
			m_resultsFlyout.Hide();
		}

		void OnCopyMenuItemClicked( object sender,  RoutedEventArgs e)
		{
			var calcResult = (m_resultsFlyout.Target) as CalculationResult;
			CopyPasteManager.CopyToClipboard(calcResult.GetRawDisplayValue());
		}

		async void OnPasteMenuItemClicked( object sender,  RoutedEventArgs e)
		{
			var pastedString = await CopyPasteManager.GetStringToPaste(Model.Mode, CategoryGroupType.Converter, default(int), default(int));
			Model.OnPaste(pastedString, Model.Mode);
		}

		public void AnimateConverter()
		{
			if (App.IsAnimationEnabled())
			{
				AnimationStory.Begin();
			}
		}

		public void OnValueSelected( object sender)
		{
			var value = (CalculationResult) sender;
			// update the font size since the font is changed to bold
			value.UpdateTextState();
			((UnitConverterViewModel) this.DataContext).OnValueActivated(value);
		}

		void UpdateDropDownState( object sender,  object e)
		{
			((UnitConverterViewModel)this.DataContext).IsDropDownOpen = (Units1.IsDropDownOpen) || (Units2.IsDropDownOpen);
			// TODO UNO: KeyboardShortcutManager.UpdateDropDownState((Units1.IsDropDownOpen) || (Units2.IsDropDownOpen));
		}

		void OnLoaded(object sender,  RoutedEventArgs args)
		{
		}

		public void SetDefaultFocus()
		{
			CalculatorList<Control> focusPrecedence = new CalculatorList<Control>{ Value1, CurrencyRefreshBlockControl, OfflineBlock, ClearEntryButtonPos0 };

			foreach (Control control in focusPrecedence)
			{
				if (control.Focus(FocusState.Programmatic))
				{
					break;
				}
			}
		}

		void CurrencyRefreshButton_Click( object sender,  RoutedEventArgs e)
		{
			if (Model.NetworkBehavior == NetworkAccessBehavior.OptIn)
			{
				m_meteredConnectionOverride = true;
			}

			Model.RefreshCurrencyRatios();
		}

		void OnDataContextChanged( DependencyObject sender,  DataContextChangedEventArgs args)
		{
			Model.PropertyChanged -= OnPropertyChanged;

			Model.PropertyChanged += new PropertyChangedEventHandler(OnPropertyChanged);

			OnNetworkBehaviorChanged();
		}

		void Units1_IsEnabledChanged(object sender, DependencyPropertyChangedEventArgs e)
		{
			if ((Units1.Visibility == Visibility.Visible) && Units1.IsEnabled)
			{
				SetDefaultFocus();
			}
		}

		void OnIsDisplayVisibleChanged()
		{
			if (Model.IsCurrencyLoadingVisible)
			{
				StartProgressRingWithDelay();
			}
			else
			{
				HideProgressRing();

				if (m_isAnimationEnabled && Model.IsCurrencyCurrentCategory && !Model.CurrencyTimestamp.IsEmpty())
				{
					TimestampFadeInAnimation.Begin();
				}
			}
		}

		void StartProgressRingWithDelay()
		{
			HideProgressRing();

			TimeSpan delay = TimeSpan.FromMilliseconds(DURATION_500_MS);

			m_delayTimer = new DispatcherTimer();
			m_delayTimer.Interval = delay;
			m_delayTimer.Tick += new EventHandler<object> (OnDelayTimerTick);

			m_delayTimer.Start();
		}

		void OnDelayTimerTick(object sender, object e)
		{
			CurrencyLoadingProgressRing.IsActive = true;
			m_delayTimer.Stop();
		}

		void HideProgressRing()
		{
			if (m_delayTimer != null)
			{
				m_delayTimer.Stop();
			}

			CurrencyLoadingProgressRing.IsActive = false;
		}

		// The function will make sure the UI will have enough space to display supplementary results and currency information
		void SupplementaryResultsPanelInGrid_SizeChanged(object sender, Windows.UI.Xaml.SizeChangedEventArgs e)
		{
			// We add 0.01 to be sure to not create an infinite loop with SizeChanged events cascading due to float approximation
			RowDltrUnits.MinHeight = Math.Max(48.0, e.NewSize.Height + 0.01);
		}

	}
}
