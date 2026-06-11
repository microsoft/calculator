using System;

using CalculatorApp.ViewModel;
using CalculatorApp.ViewModel.Common;

using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

// The User Control item template is documented at https://go.microsoft.com/fwlink/?LinkId=234236

namespace CalculatorApp
{
    public sealed partial class Memory : UserControl
    {
        public Memory()
        {
            InitializeComponent();

            MemoryPaneEmpty.FlowDirection = LocalizationService.GetInstance().GetFlowDirection();
        }

        public CalculatorApp.ViewModel.StandardCalculatorViewModel Model => (CalculatorApp.ViewModel.StandardCalculatorViewModel)this.DataContext;

        // Raised when the last memory item is cleared via the context menu, so the
        // hosting page can move focus back to the main keypad (M+).
        public event EventHandler MemoryEmptied;

        public GridLength RowHeight
        {
            get => (GridLength)GetValue(RowHeightProperty);
            set => SetValue(RowHeightProperty, value);
        }

        // Using a DependencyProperty as the backing store for RowHeight.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty RowHeightProperty =
            DependencyProperty.Register(nameof(RowHeight), typeof(GridLength), typeof(Memory), new PropertyMetadata(default(GridLength)));

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

        private bool m_isErrorVisualState = false;

        private void MemoryListItemClick(object sender, ItemClickEventArgs e)
        {
            MemoryItemViewModel memorySlot = ((MemoryItemViewModel)e.ClickedItem);

            // In case the memory list is clicked and enter is pressed,
            // On Item clicked event gets fired and e->ClickedItem is Null.
            if (memorySlot != null)
            {
                Model.OnMemoryItemPressed(memorySlot.Position);
            }
        }

        private void OnClearMenuItemClicked(object sender, RoutedEventArgs e)
        {
            var memoryItem = GetMemoryItemForCurrentFlyout();
            if (memoryItem == null)
            {
                return;
            }

            var itemsRemainingAfterClear = MemoryListView.Items.Count - 1;

            if (itemsRemainingAfterClear > 0)
            {
                // Capture the position before clearing; the container is torn down on removal.
                var clearedIndex = MemoryListView.IndexFromContainer(MemoryContextMenu.Target);
                memoryItem.Clear();

                var newFocusIndex = Math.Min(clearedIndex, itemsRemainingAfterClear - 1);
                var newContainer = MemoryListView.ContainerFromIndex(newFocusIndex) as Control;
                newContainer?.Focus(FocusState.Programmatic);
            }
            else
            {
                // Move focus to the fallback target before removing the item, so focus does not
                // briefly escape (e.g. to the hamburger menu) while the list empties.
                MemoryEmptied?.Invoke(this, EventArgs.Empty);
                memoryItem.Clear();
            }
        }

        private void OnMemoryAddMenuItemClicked(object sender, RoutedEventArgs e)
        {
            var memoryItem = GetMemoryItemForCurrentFlyout();
            if (memoryItem != null)
            {
                memoryItem.MemoryAdd();
            }
        }

        private void OnMemorySubtractMenuItemClicked(object sender, RoutedEventArgs e)
        {
            var memoryItem = GetMemoryItemForCurrentFlyout();
            if (memoryItem != null)
            {
                memoryItem.MemorySubtract();
            }
        }

        private MemoryItemViewModel GetMemoryItemForCurrentFlyout()
        {
            var listViewItem = MemoryContextMenu.Target;
            return (MemoryListView.ItemFromContainer(listViewItem) as MemoryItemViewModel);
        }
    }
}

