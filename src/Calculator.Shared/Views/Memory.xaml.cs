// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

//
// xaml.h
// Declaration of the Memory class
//

using CalculatorApp.Common;
using CalculatorApp.ViewModel;
using Windows.Foundation;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;

namespace CalculatorApp
{
	public partial class Memory
	{
		private Windows.UI.Xaml.Controls.MenuFlyout m_memoryItemFlyout;
		private bool m_isErrorVisualState;

		public CalculatorApp.ViewModel.StandardCalculatorViewModel Model => (CalculatorApp.ViewModel.StandardCalculatorViewModel)(this.DataContext);

		public GridLength RowHeight
		{
			get { return (GridLength)GetValue(RowHeightProperty); }
			set { SetValue(RowHeightProperty, value); }
		}

		public static readonly DependencyProperty RowHeightProperty =
			DependencyProperty.Register("RowHeight", typeof(GridLength), typeof(Memory), new PropertyMetadata(GridLength.Auto));

		public Memory()
		{
			m_isErrorVisualState = false;
			InitializeComponent();

			// UNO TODO
			m_memoryItemFlyout = Resources["MemoryContextMenu"] as MenuFlyout ?? new MenuFlyout();

			MemoryPaneEmpty.FlowDirection = LocalizationService.GetInstance().GetFlowDirection();
		}

		void MemoryListItemClick(object sender, ItemClickEventArgs e)
		{
			MemoryItemViewModel memorySlot = (MemoryItemViewModel)(e.ClickedItem);

			// In case the memory list is clicked and enter is pressed,
			// On Item clicked event gets fired and e.ClickedItem is Null.
			if (memorySlot != null)
			{
				Model.OnMemoryItemPressed(memorySlot.Position);
			}
		}

		void OnContextRequested(Windows.UI.Xaml.UIElement sender, Windows.UI.Xaml.Input.ContextRequestedEventArgs e)
		{
			// Walk up the tree to find the ListViewItem.
			// There may not be one if the click wasn't on an item.
			var requestedElement = (FrameworkElement)(e.OriginalSource);
			while ((requestedElement != sender) && !(requestedElement is ListViewItem))
			{
				requestedElement = (FrameworkElement)(VisualTreeHelper.GetParent(requestedElement));
			}

			if (requestedElement != sender)
			{
				// The context menu request was for a ListViewItem.
				var memorySlot = (MemoryItemViewModel)(MemoryListView.ItemFromContainer(requestedElement));
				Point point;
				if (e.TryGetPosition(requestedElement, out point))
				{
					m_memoryItemFlyout.ShowAt(requestedElement, point);
				}
				else
				{
					// Not invoked via pointer, so let XAML choose a default location.
					m_memoryItemFlyout.ShowAt(requestedElement);
				}

				e.Handled = true;
			}
		}

		void OnContextCanceled(Windows.UI.Xaml.UIElement sender, Windows.UI.Xaml.RoutedEventArgs e)
		{
			m_memoryItemFlyout.Hide();
		}

		void OnClearMenuItemClicked(object sender, RoutedEventArgs e)
		{
			GetMemoryItemForCurrentFlyout().Clear();
		}

		void OnMemoryAddMenuItemClicked(object sender, RoutedEventArgs e)
		{
			GetMemoryItemForCurrentFlyout().MemoryAdd();
		}

		void OnMemorySubtractMenuItemClicked(object sender, RoutedEventArgs e)
		{
			GetMemoryItemForCurrentFlyout().MemorySubtract();
		}

		public bool IsErrorVisualState
		{
			get => m_isErrorVisualState;
			set
			{
				if (m_isErrorVisualState != value)
				{
					m_isErrorVisualState = value;
					string newState = m_isErrorVisualState ? "ErrorLayout" : "NoErrorLayout";
					VisualStateManager.GoToState(this, newState, false);
				}
			}
		}

		MemoryItemViewModel GetMemoryItemForCurrentFlyout()
		{
			var listViewItem = m_memoryItemFlyout.Target;

			return (MemoryItemViewModel)(MemoryListView.ItemFromContainer(listViewItem));
		}
	}
}
