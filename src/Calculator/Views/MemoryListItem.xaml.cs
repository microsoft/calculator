using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

// The User Control item template is documented at https://go.microsoft.com/fwlink/?LinkId=234236

namespace CalculatorApp
{
    [Windows.Foundation.Metadata.WebHostHidden]
    public sealed partial class MemoryListItem : UserControl
    {
        public MemoryListItem()
        {
            InitializeComponent();
        }

        public CalculatorApp.ViewModel.MemoryItemViewModel ViewModel
        {
            get => (CalculatorApp.ViewModel.MemoryItemViewModel)GetValue(ViewModelProperty);
            set => SetValue(ViewModelProperty, value);
        }

        // Using a DependencyProperty as the backing store for ViewModel.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty ViewModelProperty =
            DependencyProperty.Register(nameof(ViewModel), typeof(CalculatorApp.ViewModel.MemoryItemViewModel), typeof(MemoryListItem), new PropertyMetadata(default(CalculatorApp.ViewModel.MemoryItemViewModel)));

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

        private void OnMemoryAddButtonClicked(object sender, RoutedEventArgs e)
        {
            ViewModel.MemoryAdd();
        }

        private void OnClearButtonClicked(object sender, RoutedEventArgs e)
        {
            ViewModel.Clear();
        }
        private void OnMemorySubtractButtonClicked(object sender, RoutedEventArgs e)
        {
            ViewModel.MemorySubtract();
        }

        private void OnClearSwipeInvoked(Microsoft.UI.Xaml.Controls.SwipeItem sender, Microsoft.UI.Xaml.Controls.SwipeItemInvokedEventArgs e)
        {
            ViewModel.Clear();
        }

        private void OnMemoryAddSwipeInvoked(Microsoft.UI.Xaml.Controls.SwipeItem sender, Microsoft.UI.Xaml.Controls.SwipeItemInvokedEventArgs e)
        {
            ViewModel.MemoryAdd();
        }

        private void OnMemorySubtractSwipeInvoked(Microsoft.UI.Xaml.Controls.SwipeItem sender, Microsoft.UI.Xaml.Controls.SwipeItemInvokedEventArgs e)
        {
            ViewModel.MemorySubtract();
        }
    }
}

