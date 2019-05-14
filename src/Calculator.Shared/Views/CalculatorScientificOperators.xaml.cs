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
    public sealed partial class CalculatorScientificOperators : UserControl
    {
        public StandardCalculatorViewModel Model => (StandardCalculatorViewModel)(DataContext);



        public bool IsErrorVisualState
        {
            get { return (bool)GetValue(IsErrorVisualStateProperty); }
            set { SetValue(IsErrorVisualStateProperty, value); }
        }

        public static readonly DependencyProperty IsErrorVisualStateProperty =
            DependencyProperty.Register("IsErrorVisualState", typeof(bool), typeof(CalculatorScientificOperators), new PropertyMetadata(false));

        public bool IsWideLayout
        {
            get { return (bool)GetValue(IsWideLayoutProperty); }
            set { SetValue(IsWideLayoutProperty, value); }
        }

        public static readonly DependencyProperty IsWideLayoutProperty =
            DependencyProperty.Register("IsWideLayout", typeof(bool), typeof(CalculatorScientificOperators), new PropertyMetadata(false));

        bool IsShiftEnabled(bool isWideLayout, bool isErrorState)
        {
            return !(isWideLayout || isErrorState);
        }

        public CalculatorScientificOperators()
        {
            this.InitializeComponent();

            // UNO TODO
            // expButton.SetValue(Common.KeyboardShortcutManager.VirtualKeyProperty, Common.MyVirtualKey.E);
            // Common.KeyboardShortcutManager.ShiftButtonChecked(false);
        }

        void ShortLayout_Completed(object sender, object e)
        {
            IsWideLayout = false;
            SetOperatorRowVisibility();
            // UNO TODO
            // Common.KeyboardShortcutManager.ShiftButtonChecked(Model.IsShiftChecked);
        }

        void WideLayout_Completed(object sender, object e)
        {
            IsWideLayout = true;
            SetOperatorRowVisibility();
            // UNO TODO
            // Common.KeyboardShortcutManager.ShiftButtonChecked(Model.IsShiftChecked);
        }

        void OnIsErrorVisualStatePropertyChanged(bool oldValue, bool newValue)
        {
            String newState = newValue ? "ErrorLayout" : "NoErrorLayout";
            VisualStateManager.GoToState(this, newState, false);
            NumberPad.IsErrorVisualState = newValue;
        }

        void shiftButton_Check(object sender, Windows.UI.Xaml.RoutedEventArgs e)
        {
            bool isChecked = shiftButton.IsChecked.Value;
            Model.IsShiftChecked = isChecked;
            // UNO TODO
            // Common.KeyboardShortcutManager.ShiftButtonChecked(isChecked);
            SetOperatorRowVisibility();
        }

        void shiftButton_IsEnabledChanged(
             object sender,
             Windows.UI.Xaml.DependencyPropertyChangedEventArgs e)
        {
            SetOperatorRowVisibility();
            // UNO TODO
            // Common.KeyboardShortcutManager.ShiftButtonChecked(ShiftButton.IsEnabled && ShiftButton.IsChecked.Value);
        }

        void SetOperatorRowVisibility()
        {
            Visibility rowVis, invRowVis;

            if (IsWideLayout)
            {
                rowVis = Visibility.Visible;
                invRowVis = Visibility.Visible;
            }
            else if (shiftButton.IsChecked.Value)
            {
                rowVis = Visibility.Collapsed;
                invRowVis = Visibility.Visible;
            }
            else
            {
                rowVis = Visibility.Visible;
                invRowVis = Visibility.Collapsed;
            }

            Row1.Visibility = rowVis;
            Row2.Visibility = rowVis;
            InvRow1.Visibility = invRowVis;
            InvRow2.Visibility = invRowVis;
        }

        void OpenParenthesisButton_GotFocus(object sender, RoutedEventArgs e)
        {
            Model.SetOpenParenthesisCountNarratorAnnouncement();
        }

        String ParenthesisCountToString(uint count)
        {
            return (count == 0) ? "" : count.ToString();
        }

    }
}
