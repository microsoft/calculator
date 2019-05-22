// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using CalculatorApp.Common;
using CalculatorApp.ViewModel;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace CalculatorApp
{
	public partial class HistoryList
	{
		public HistoryViewModel Model => (CalculatorApp.ViewModel.HistoryViewModel)(this.DataContext);

		public GridLength RowHeight
		{
			get { return (GridLength)GetValue(RowHeightProperty); }
			set { SetValue(RowHeightProperty, value); }
		}

		// Using a DependencyProperty as the backing store for RowHeight.  This enables animation, styling, binding, etc...
		public static readonly DependencyProperty RowHeightProperty =
			DependencyProperty.Register("RowHeight", typeof(GridLength), typeof(HistoryList), new PropertyMetadata(GridLength.Auto));

		Windows.Foundation.Rect m_visibleBounds;
		Windows.Foundation.Rect m_coreBounds;

		public HistoryList()
		{
			InitializeComponent();

			HistoryEmpty.FlowDirection = LocalizationService.GetInstance().GetFlowDirection();
		}

		void ListView_ItemClick(object sender, ItemClickEventArgs e)
		{
			HistoryViewModel historyVM = (HistoryViewModel)(this.DataContext);
			HistoryItemViewModel clickedItem = (HistoryItemViewModel)(e.ClickedItem);

			// When the user clears the history list in the overlay view and presses enter, the clickedItem is null
			if (clickedItem != null)
			{
				historyVM.ShowItem(clickedItem);
			}
		}

		void OnDeleteMenuItemClicked(object sender, RoutedEventArgs e)
		{
			var clickedItem = (HistoryItemViewModel)((FrameworkElement)sender).DataContext;

			Model.DeleteItem(clickedItem);
		}

#if false // UNO TODO
		void OnDeleteSwipeInvoked(MUXC.SwipeItem sender, MUXC.SwipeItemInvokedEventArgs e)
		{
			var swipedItem = safe_cast<HistoryItemViewModel>(e.SwipeControl.DataContext);

			Model.DeleteItem(swipedItem);
		}
#endif

		void ScrollToBottom()
		{
			var historyItems = this.HistoryListView.Items;
			if (historyItems.Count > 0)
			{
				this.HistoryListView.ScrollIntoView(historyItems[historyItems.Count - 1]);
			}
		}

	};
}
