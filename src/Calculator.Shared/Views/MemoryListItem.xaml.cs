// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using CalculatorApp.ViewModel;
using Windows.UI.Xaml;

namespace CalculatorApp
{
	/// <summary>
	/// Represents a single item in the Memory list.
	/// </summary>
	public partial class MemoryListItem
	{

		public MemoryItemViewModel Model
		{
			get { return (MemoryItemViewModel)GetValue(ModelProperty); }
			set { SetValue(ModelProperty, value); }
		}

		// Using a DependencyProperty as the backing store for Model.  This enables animation, styling, binding, etc...
		public static readonly DependencyProperty ModelProperty =
			DependencyProperty.Register("Model", typeof(MemoryItemViewModel), typeof(MemoryListItem), new PropertyMetadata(null));

		public MemoryListItem()
		{
			InitializeComponent();
		}

		protected override void OnPointerEntered(Windows.UI.Xaml.Input.PointerRoutedEventArgs e)
		{
			base.OnPointerEntered(e);

			// Only show hover buttons when the user is using mouse or pen.
			if (e.Pointer.PointerDeviceType == Windows.Devices.Input.PointerDeviceType.Mouse
				|| e.Pointer.PointerDeviceType == Windows.Devices.Input.PointerDeviceType.Pen)
			{
				VisualStateManager.GoToState(this, "MemoryButtonsVisible", true);
			}
		}

		protected override void OnPointerExited(Windows.UI.Xaml.Input.PointerRoutedEventArgs e)
		{
			base.OnPointerExited(e);

			VisualStateManager.GoToState(this, "MemoryButtonsHidden", true);
		}

		void OnClearButtonClicked(object sender, RoutedEventArgs e)
		{
			Model.Clear();
		}

		void OnMemoryAddButtonClicked(object sender, RoutedEventArgs e)
		{
			Model.MemoryAdd();
		}

		void OnMemorySubtractButtonClicked(object sender, RoutedEventArgs e)
		{
			Model.MemorySubtract();
		}

#if false
		void OnClearSwipeInvoked(object sender, SwipeItemInvokedEventArgs e)
		{
			Model.Clear();
		}

		void OnMemoryAddSwipeInvoked(object sender, SwipeItemInvokedEventArgs e)
		{
			Model.MemoryAdd();
		}

		void OnMemorySubtractSwipeInvoked(object sender, SwipeItemInvokedEventArgs e)
		{
			Model.MemorySubtract();
		}
#endif
	};
}
