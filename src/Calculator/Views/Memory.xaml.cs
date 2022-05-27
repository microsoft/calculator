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
            if (memoryItem != null)
            {
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

