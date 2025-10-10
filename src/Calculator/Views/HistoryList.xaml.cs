using CalculatorApp.ViewModel;
using CalculatorApp.ViewModel.Common;

using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

using MUXC = Microsoft.UI.Xaml.Controls;

// The User Control item template is documented at https://go.microsoft.com/fwlink/?LinkId=234236

namespace CalculatorApp
{
    [Windows.Foundation.Metadata.WebHostHidden]
    public sealed partial class HistoryList : UserControl
    {
        public HistoryList()
        {
            InitializeComponent();

            HistoryEmpty.FlowDirection = LocalizationService.GetInstance().GetFlowDirection();
        }

        public CalculatorApp.ViewModel.HistoryViewModel Model => (CalculatorApp.ViewModel.HistoryViewModel)DataContext;

        public void ScrollToBottom()
        {
            var historyItems = HistoryListView.Items;
            if (historyItems.Count > 0)
            {
                HistoryListView.ScrollIntoView(historyItems[historyItems.Count - 1]);
            }
        }

        public Windows.UI.Xaml.GridLength RowHeight
        {
            get => (Windows.UI.Xaml.GridLength)GetValue(RowHeightProperty);
            set => SetValue(RowHeightProperty, value);
        }

        // Using a DependencyProperty as the backing store for RowHeight.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty RowHeightProperty =
            DependencyProperty.Register(nameof(RowHeight), typeof(Windows.UI.Xaml.GridLength), typeof(HistoryList), new PropertyMetadata(default(Windows.UI.Xaml.GridLength)));

        public static string GetHistoryItemAutomationName(string accExpression, string accResult)
        {
            return $"{accExpression} {accResult}";
        }

        private void ListView_ItemClick(object sender, ItemClickEventArgs e)
        {
            // When the user clears the history list in the overlay view and presses enter, the clickedItem is nullptr
            if (e.ClickedItem is HistoryItemViewModel clickedItem && DataContext is HistoryViewModel historyVM)
            {
                historyVM.ShowItem(clickedItem);
            }
        }
        private void OnCopyMenuItemClicked(object sender, RoutedEventArgs e)
        {
            var listViewItem = HistoryContextMenu.Target;
            if (HistoryListView.ItemFromContainer(listViewItem) is HistoryItemViewModel itemViewModel)
            {
                CopyPasteManager.CopyToClipboard(itemViewModel.Result);
            }
        }
        private void OnDeleteMenuItemClicked(object sender, RoutedEventArgs e)
        {
            var listViewItem = HistoryContextMenu.Target;
            if (HistoryListView.ItemFromContainer(listViewItem) is HistoryItemViewModel itemViewModel)
            {
                Model.DeleteItem(itemViewModel);
            }
        }
        private void OnDeleteSwipeInvoked(MUXC.SwipeItem sender, MUXC.SwipeItemInvokedEventArgs e)
        {
            if (e.SwipeControl.DataContext is HistoryItemViewModel swipedItem)
            {
                Model.DeleteItem(swipedItem);
            }
        }
    }
}

