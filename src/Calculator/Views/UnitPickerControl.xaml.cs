// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.ComponentModel;

using CalculatorApp.ViewModel;

using Windows.System;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Automation.Peers;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Input;

namespace CalculatorApp
{
    public sealed partial class UnitPickerControl : UserControl
    {
        public UnitPickerControl()
        {
            InitializeComponent();
        }

        public event EventHandler<UnitPickerItem> UnitPicked;
        public event EventHandler CurrencyRefreshRequested;
        public event EventHandler DismissRequested;

        public static readonly DependencyProperty ViewModelProperty = DependencyProperty.Register(
            nameof(ViewModel),
            typeof(UnitPickerViewModel),
            typeof(UnitPickerControl),
            new PropertyMetadata(null, OnViewModelChanged));

        public UnitPickerViewModel ViewModel
        {
            get => (UnitPickerViewModel)GetValue(ViewModelProperty);
            set => SetValue(ViewModelProperty, value);
        }

        /// <summary>
        /// Dims the category list while a search is active. Kept out of the view model and bound with
        /// x:Bind off CategoryListEnabled, so the view model exposes state, not display values.
        /// </summary>
        public double CategoryOpacity(bool enabled) => enabled ? 1.0 : 0.4;

        /// <summary>
        /// Maps the picker's no-search-results state to message visibility, keeping the
        /// view model free of Visibility display values (bound the same way as CategoryOpacity).
        /// </summary>
        public Windows.UI.Xaml.Visibility NoSearchResultsVisibility(bool hasNoSearchResults) =>
            hasNoSearchResults ? Windows.UI.Xaml.Visibility.Visible : Windows.UI.Xaml.Visibility.Collapsed;

        public Windows.UI.Xaml.Visibility StateVisibility(bool isVisible) =>
            isVisible ? Windows.UI.Xaml.Visibility.Visible : Windows.UI.Xaml.Visibility.Collapsed;

        private static void OnViewModelChanged(DependencyObject sender, DependencyPropertyChangedEventArgs args)
        {
            var control = (UnitPickerControl)sender;
            control.UpdateViewModelSubscription(
                args.OldValue as UnitPickerViewModel,
                args.NewValue as UnitPickerViewModel);
        }

        private void UpdateViewModelSubscription(UnitPickerViewModel oldViewModel, UnitPickerViewModel newViewModel)
        {
            if (oldViewModel != null)
            {
                oldViewModel.PropertyChanged -= OnViewModelPropertyChanged;
            }

            _hadResults = newViewModel?.HasResults == true;
            if (newViewModel != null)
            {
                newViewModel.PropertyChanged += OnViewModelPropertyChanged;
            }
        }

        private void OnViewModelPropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            if (e.PropertyName != nameof(UnitPickerViewModel.HasResults))
            {
                return;
            }

            bool hasResults = ViewModel?.HasResults == true;
            if (_hadResults && !hasResults)
            {
                _ = Dispatcher.RunAsync(CoreDispatcherPriority.Normal, RaiseNoResultsAutomationEvent);
            }
            _hadResults = hasResults;
        }

        private void RaiseNoResultsAutomationEvent()
        {
            if (ViewModel?.HasResults != false || NoResultsText.Visibility != Visibility.Visible)
            {
                return;
            }

            AutomationPeer peer =
                FrameworkElementAutomationPeer.FromElement(NoResultsText)
                ?? FrameworkElementAutomationPeer.CreatePeerForElement(NoResultsText);
            peer?.RaiseAutomationEvent(AutomationEvents.LiveRegionChanged);
        }

        /// <summary>
        /// Clears any query left from a previous open and focuses the search box for keyboard users.
        /// </summary>
        public void PrepareForOpen()
        {
            SearchBox.Text = string.Empty;
            SearchBox.Focus(FocusState.Programmatic);
            ViewModel?.StartTrackingCategorySelections();
        }

        private void OnSearchTextChanged(AutoSuggestBox sender, AutoSuggestBoxTextChangedEventArgs args)
        {
            if (ViewModel != null)
            {
                ViewModel.SearchText = sender.Text ?? string.Empty;
            }
        }

        private void OnSearchBoxPreviewKeyDown(object sender, KeyRoutedEventArgs e)
        {
            if (e.Key == VirtualKey.Down)
            {
                e.Handled = TryFocusFirstUnit();
            }
            else if (e.Key == VirtualKey.Enter)
            {
                e.Handled = TryPickFirstSearchResult();
            }
        }

        private void OnPickerPreviewKeyDown(object sender, KeyRoutedEventArgs e)
        {
            if (e.Key == VirtualKey.Escape)
            {
                e.Handled = true;
                DismissRequested?.Invoke(this, EventArgs.Empty);
            }
            else if (e.Key == VirtualKey.Up
                && ReferenceEquals(FocusManager.GetFocusedElement(), UnitList.ContainerFromIndex(0)))
            {
                e.Handled = SearchBox.Focus(FocusState.Keyboard);
            }
            else if (e.Key == (FlowDirection == Windows.UI.Xaml.FlowDirection.RightToLeft ? VirtualKey.Left : VirtualKey.Right)
                && IsFocusInList(CategoryList))
            {
                e.Handled = TryFocusSelectedUnit();
            }
            else if (e.Key == (FlowDirection == Windows.UI.Xaml.FlowDirection.RightToLeft ? VirtualKey.Right : VirtualKey.Left)
                && IsFocusInList(UnitList))
            {
                e.Handled = TryFocusSelectedCategory();
            }
        }

        private bool TryFocusFirstUnit()
        {
            // Null-conditional lifting makes "ViewModel?.FilteredUnits.Count <= 0" false when
            // ViewModel is null, so guard the null case explicitly before indexing below.
            if (ViewModel?.FilteredUnits == null || ViewModel.FilteredUnits.Count == 0)
            {
                return false;
            }

            return TryFocusUnit(ViewModel.FilteredUnits[0]);
        }

        private bool TryFocusSelectedUnit()
        {
            UnitPickerItem unit = ViewModel?.SelectedUnit;
            if (unit == null && ViewModel?.FilteredUnits?.Count > 0)
            {
                unit = ViewModel.FilteredUnits[0];
            }

            return TryFocusUnit(unit);
        }

        private bool TryFocusUnit(UnitPickerItem unit)
        {
            if (unit == null)
            {
                return false;
            }

            UnitList.ScrollIntoView(unit);
            UnitList.UpdateLayout();
            return (UnitList.ContainerFromItem(unit) as Control)?.Focus(FocusState.Keyboard) == true;
        }

        private bool TryFocusSelectedCategory()
        {
            UnitPickerCategory category = ViewModel?.SelectedCategory;
            if (category == null || !CategoryList.IsEnabled)
            {
                return false;
            }

            CategoryList.ScrollIntoView(category);
            CategoryList.UpdateLayout();
            return (CategoryList.ContainerFromItem(category) as Control)?.Focus(FocusState.Keyboard) == true;
        }

        private static bool IsFocusInList(ListView list)
        {
            return FocusManager.GetFocusedElement() is DependencyObject focused
                && list.IndexFromContainer(focused) >= 0;
        }

        private bool TryPickFirstSearchResult()
        {
            UnitPickerItem firstResult = ViewModel?.FirstSearchResult;
            if (firstResult == null)
            {
                return false;
            }

            UnitPicked?.Invoke(this, firstResult);
            return true;
        }

        private void OnCategorySelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (e.AddedItems.Count > 0 && e.AddedItems[0] is UnitPickerCategory category)
            {
                ViewModel?.SelectCategory(category);
            }
        }

        private void OnUnitClick(object sender, ItemClickEventArgs e)
        {
            if (e.ClickedItem is UnitPickerItem item)
            {
                UnitPicked?.Invoke(this, item);
            }
        }

        private void OnCurrencyRefreshClick(object sender, RoutedEventArgs e)
        {
            CurrencyRefreshRequested?.Invoke(this, EventArgs.Empty);
        }

        private bool _hadResults;
    }
}
