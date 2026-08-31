// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;

using CalculatorApp.ViewModel;

using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

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

        public static readonly DependencyProperty ViewModelProperty = DependencyProperty.Register(
            nameof(ViewModel), typeof(UnitPickerViewModel), typeof(UnitPickerControl), new PropertyMetadata(null));

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
    }
}
