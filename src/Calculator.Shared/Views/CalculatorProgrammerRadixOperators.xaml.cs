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
    public sealed partial class CalculatorProgrammerRadixOperators : UserControl
    {
        bool m_isErrorVisualState;
        StandardCalculatorViewModel Model => (StandardCalculatorViewModel) (DataContext);

        public CalculatorProgrammerRadixOperators()
        {
            this.InitializeComponent();

            var booleanToVisibilityNegationConverter = new Converters.BooleanToVisibilityNegationConverter();
            SetVisibilityBinding(ProgRadixOps, "IsBinaryBitFlippingEnabled", booleanToVisibilityNegationConverter);
        }

        void OnLoaded(object sender, RoutedEventArgs args)
        {
            Model.ProgModeRadixChange += ProgModeRadixChange;
        }

        void OnUnloaded(object sender, RoutedEventArgs args)
        {
            Model.ProgModeRadixChange -= ProgModeRadixChange;
        }

        void Shift_Clicked(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            bool isShiftChecked = ((ToggleButton)sender).IsChecked.Value;
            var scvm = (StandardCalculatorViewModel)(this.DataContext);
            scvm.IsShiftProgrammerChecked = isShiftChecked;

            if (rolButton == null)
            {
                FindName("RolButton");
                FindName("RorButton");
            }

            if (isShiftChecked)
            {
                rolButton.Visibility = Visibility.Visible;
                rorButton.Visibility = Visibility.Visible;
                lshButton.Visibility = Visibility.Collapsed;
                rshButton.Visibility = Visibility.Collapsed;
            }
            else
            {
                rolButton.Visibility = Visibility.Collapsed;
                rorButton.Visibility = Visibility.Collapsed;
                lshButton.Visibility = Visibility.Visible;
                rshButton.Visibility = Visibility.Visible;
            }
        }

        void SetVisibilityBinding(FrameworkElement element, String path, IValueConverter converter)
        {
            Binding commandBinding = new Binding();
            commandBinding.Path = new PropertyPath(path);
            commandBinding.Converter = converter;
            element.SetBinding(VisibilityProperty, commandBinding);
        }

        void ProgModeRadixChange()
        {
            NumberPad.ProgModeRadixChange();
        }

        public bool IsErrorVisualState {
            get => m_isErrorVisualState;
            set
            {
                if (m_isErrorVisualState != value)
                {
                    m_isErrorVisualState = value;
                    String newState = m_isErrorVisualState ? "ErrorLayout" : "NoErrorLayout";
                    VisualStateManager.GoToState(this, newState, false);
                    NumberPad.IsErrorVisualState = m_isErrorVisualState;
                }
            }
        }


        String ParenthesisCountToString(uint count)
        {
            return (count == 0) ? "" : count.ToString();
        }

        void OpenParenthesisButton_GotFocus(object sender, RoutedEventArgs e)
        {
            Model.SetOpenParenthesisCountNarratorAnnouncement();
        }

    }
}
