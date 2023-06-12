using CalculatorApp.ViewModel.Common;

using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

// The User Control item template is documented at https://go.microsoft.com/fwlink/?LinkId=234236

namespace CalculatorApp
{
    public sealed partial class NumberPad : UserControl
    {
        public NumberPad()
        {
            m_isErrorVisualState = false;
            InitializeComponent();

            var localizationSettings = LocalizationSettings.GetInstance();

            DecimalSeparatorButton.Content = localizationSettings.GetDecimalSeparator();
            Num0Button.Content = localizationSettings.GetDigitSymbolFromEnUsDigit('0');
            Num1Button.Content = localizationSettings.GetDigitSymbolFromEnUsDigit('1');
            Num2Button.Content = localizationSettings.GetDigitSymbolFromEnUsDigit('2');
            Num3Button.Content = localizationSettings.GetDigitSymbolFromEnUsDigit('3');
            Num4Button.Content = localizationSettings.GetDigitSymbolFromEnUsDigit('4');
            Num5Button.Content = localizationSettings.GetDigitSymbolFromEnUsDigit('5');
            Num6Button.Content = localizationSettings.GetDigitSymbolFromEnUsDigit('6');
            Num7Button.Content = localizationSettings.GetDigitSymbolFromEnUsDigit('7');
            Num8Button.Content = localizationSettings.GetDigitSymbolFromEnUsDigit('8');
            Num9Button.Content = localizationSettings.GetDigitSymbolFromEnUsDigit('9');
        }

        public Windows.UI.Xaml.Style ButtonStyle
        {
            get => (Windows.UI.Xaml.Style)GetValue(ButtonStyleProperty);
            set => SetValue(ButtonStyleProperty, value);
        }

        // Using a DependencyProperty as the backing store for ButtonStyle.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty ButtonStyleProperty =
            DependencyProperty.Register(nameof(ButtonStyle), typeof(Windows.UI.Xaml.Style), typeof(NumberPad), new PropertyMetadata(default(Windows.UI.Xaml.Style)));

        public CalculatorApp.ViewModel.Common.NumberBase CurrentRadixType
        {
            get => (CalculatorApp.ViewModel.Common.NumberBase)GetValue(CurrentRadixTypeProperty);
            set => SetValue(CurrentRadixTypeProperty, value);
        }

        // Using a DependencyProperty as the backing store for CurrentRadixType.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty CurrentRadixTypeProperty =
            DependencyProperty.Register(nameof(CurrentRadixType), typeof(CalculatorApp.ViewModel.Common.NumberBase), typeof(NumberPad), new PropertyMetadata(CalculatorApp.ViewModel.Common.NumberBase.DecBase, (sender, args) =>
            {
                var self = (NumberPad)sender;
                self.OnCurrentRadixTypePropertyChanged((NumberBase)args.OldValue, (NumberBase)args.NewValue);
            }));

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

        private void OnCurrentRadixTypePropertyChanged(NumberBase oldValue, NumberBase newValue)
        {
            Num0Button.IsEnabled = true;
            Num1Button.IsEnabled = true;
            Num2Button.IsEnabled = true;
            Num3Button.IsEnabled = true;
            Num4Button.IsEnabled = true;
            Num5Button.IsEnabled = true;
            Num6Button.IsEnabled = true;
            Num7Button.IsEnabled = true;
            Num8Button.IsEnabled = true;
            Num9Button.IsEnabled = true;

            if (newValue == NumberBase.BinBase)
            {
                Num2Button.IsEnabled = false;
                Num3Button.IsEnabled = false;
                Num4Button.IsEnabled = false;
                Num5Button.IsEnabled = false;
                Num6Button.IsEnabled = false;
                Num7Button.IsEnabled = false;
                Num8Button.IsEnabled = false;
                Num9Button.IsEnabled = false;
            }
            else if (newValue == NumberBase.OctBase)
            {
                Num8Button.IsEnabled = false;
                Num9Button.IsEnabled = false;
            }
        }

        private bool m_isErrorVisualState;
    }
}
