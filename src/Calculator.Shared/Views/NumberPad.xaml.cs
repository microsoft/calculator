using CalculationManager;
using CalculatorApp.Common;
using CalculatorApp.ViewModel;
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

// The User Control item template is documented at https://go.microsoft.com/fwlink/?LinkId=234236

namespace CalculatorApp
{
	public sealed partial class NumberPad : UserControl
	{
        public Style ButtonStyle
        {
            get { return (Style)GetValue(ButtonStyleProperty); }
            set { SetValue(ButtonStyleProperty, value); }
        }

        // Using a DependencyProperty as the backing store for ButtonStyle.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty ButtonStyleProperty =
            DependencyProperty.Register("ButtonStyle", typeof(Style), typeof(NumberPad), new PropertyMetadata(null));
        private bool m_isErrorVisualState;

        public NumberPad()
		{
			this.InitializeComponent();

            var localizationSettings = LocalizationSettings.GetInstance();

            this.decimalSeparatorButton.Content = localizationSettings.GetDecimalSeparator();
            this.num0Button.Content = localizationSettings.GetDigitSymbolFromEnUsDigit('0');
            this.num1Button.Content = localizationSettings.GetDigitSymbolFromEnUsDigit('1');
            this.num2Button.Content = localizationSettings.GetDigitSymbolFromEnUsDigit('2');
            this.num3Button.Content = localizationSettings.GetDigitSymbolFromEnUsDigit('3');
            this.num4Button.Content = localizationSettings.GetDigitSymbolFromEnUsDigit('4');
            this.num5Button.Content = localizationSettings.GetDigitSymbolFromEnUsDigit('5');
            this.num6Button.Content = localizationSettings.GetDigitSymbolFromEnUsDigit('6');
            this.num7Button.Content = localizationSettings.GetDigitSymbolFromEnUsDigit('7');
            this.num8Button.Content = localizationSettings.GetDigitSymbolFromEnUsDigit('8');
            this.num9Button.Content = localizationSettings.GetDigitSymbolFromEnUsDigit('9');
        }

        public void ProgModeRadixChange()
        {
            num0Button.IsEnabled = true;
            num1Button.IsEnabled = true;
            num2Button.IsEnabled = true;
            num3Button.IsEnabled = true;
            num4Button.IsEnabled = true;
            num5Button.IsEnabled = true;
            num6Button.IsEnabled = true;
            num7Button.IsEnabled = true;
            num8Button.IsEnabled = true;
            num9Button.IsEnabled = true;

            var vm = (StandardCalculatorViewModel)(this.DataContext);
            RADIX_TYPE radixType = vm.GetCurrentRadixType();

            if (radixType == RADIX_TYPE.BIN_RADIX)
            {
                num2Button.IsEnabled = false;
                num3Button.IsEnabled = false;
                num4Button.IsEnabled = false;
                num5Button.IsEnabled = false;
                num6Button.IsEnabled = false;
                num7Button.IsEnabled = false;
                num8Button.IsEnabled = false;
                num9Button.IsEnabled = false;
            }
            else if (radixType == RADIX_TYPE.OCT_RADIX)
            {
                num8Button.IsEnabled = false;
                num9Button.IsEnabled = false;
            }
        }

        public bool IsErrorVisualState
        {
            get => m_isErrorVisualState;
            set
            {
                if (m_isErrorVisualState != value)
                {
                    m_isErrorVisualState = value;
                    String newState = m_isErrorVisualState ? "ErrorLayout" : "NoErrorLayout";
                    VisualStateManager.GoToState(this, newState, false);
                }
            }
        }
    }
}
