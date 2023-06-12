using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

// The User Control item template is documented at https://go.microsoft.com/fwlink/?LinkId=234236

namespace CalculatorApp
{
    [Windows.Foundation.Metadata.WebHostHidden]
    public sealed partial class OperatorsPanel : UserControl
    {
        public CalculatorApp.ViewModel.StandardCalculatorViewModel Model => (CalculatorApp.ViewModel.StandardCalculatorViewModel)DataContext;

        public OperatorsPanel()
        {
            InitializeComponent();
        }

        public bool IsBitFlipChecked
        {
            get => (bool)GetValue(IsBitFlipCheckedProperty);
            set => SetValue(IsBitFlipCheckedProperty, value);
        }

        // Using a DependencyProperty as the backing store for IsBitFlipChecked.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty IsBitFlipCheckedProperty =
            DependencyProperty.Register(nameof(IsBitFlipChecked), typeof(bool), typeof(OperatorsPanel), new PropertyMetadata(default(bool), (sender, args) =>
            {
                var self = (OperatorsPanel)sender;
                self.OnIsBitFlipCheckedPropertyChanged((bool)args.OldValue, (bool)args.NewValue);
            }));

        public bool IsErrorVisualState
        {
            get => (bool)GetValue(IsErrorVisualStateProperty);
            set => SetValue(IsErrorVisualStateProperty, value);
        }

        // Using a DependencyProperty as the backing store for IsErrorVisualState.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty IsErrorVisualStateProperty =
            DependencyProperty.Register(nameof(IsErrorVisualState), typeof(bool), typeof(OperatorsPanel), new PropertyMetadata(default(bool), (sender, args) =>
            {
                var self = (OperatorsPanel)sender;
                self.OnIsErrorVisualStatePropertyChanged((bool)args.OldValue, (bool)args.NewValue);
            }));

        private void OnIsBitFlipCheckedPropertyChanged(bool oldValue, bool newValue)
        {
            if (newValue)
            {
                EnsureProgrammerBitFlipPanel();
            }
        }

        private void OnIsErrorVisualStatePropertyChanged(bool oldValue, bool newValue)
        {
            if (Model.IsStandard)
            {
                StandardOperators.IsErrorVisualState = newValue;
            }
            else if (Model.IsScientific)
            {
                ScientificOperators.IsErrorVisualState = newValue;
            }
            else if (Model.IsProgrammer)
            {
                ProgrammerRadixOperators.IsErrorVisualState = newValue;
            }
        }

        public void EnsureScientificOps()
        {
            if (ScientificOperators == null)
            {
                FindName("ScientificOperators");
            }
        }

        public void EnsureProgrammerRadixOps()
        {
            if (ProgrammerRadixOperators == null)
            {
                FindName("ProgrammerRadixOperators");
            }

            ProgrammerRadixOperators.checkDefaultBitShift();
        }

        private void EnsureProgrammerBitFlipPanel()
        {
            if (BitFlipPanel == null)
            {
                this.FindName("BitFlipPanel");
            }
        }
    }
}
