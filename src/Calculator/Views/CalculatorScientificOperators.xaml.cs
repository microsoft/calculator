// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

//
// CalculatorScientificOperators.xaml.h
// Declaration of the CalculatorScientificOperators class
//

using CalculatorApp.Common;
using CalculatorApp.ViewModel;
using CalculatorApp.ViewModel.Common;

using Windows.UI.Xaml;

namespace CalculatorApp
{
    [Windows.Foundation.Metadata.WebHostHidden]
    public sealed partial class CalculatorScientificOperators
    {
        public CalculatorScientificOperators()
        {
            InitializeComponent();

            ExpButton.SetValue(KeyboardShortcutManager.VirtualKeyProperty, MyVirtualKey.E);
        }

        public StandardCalculatorViewModel Model => (StandardCalculatorViewModel)this.DataContext;

        public bool IsErrorVisualState
        {
            get => (bool)GetValue(IsErrorVisualStateProperty);
            set => SetValue(IsErrorVisualStateProperty, value);
        }

        // Using a DependencyProperty as the backing store for IsErrorVisualState.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty IsErrorVisualStateProperty =
            DependencyProperty.Register(nameof(IsErrorVisualState), typeof(bool), typeof(CalculatorScientificOperators), new PropertyMetadata(false, (sender, args) =>
            {
                var self = (CalculatorScientificOperators)sender;
                self.OnIsErrorVisualStatePropertyChanged((bool)args.OldValue, (bool)args.NewValue);
            }));

        public void OpenParenthesisButton_GotFocus(object sender, RoutedEventArgs e)
        {
            Model.SetOpenParenthesisCountNarratorAnnouncement();
        }

        public string ParenthesisCountToString(uint count)
        {
            return (count == 0) ? "" : count.ToString();
        }

        private void OnIsErrorVisualStatePropertyChanged(bool oldValue, bool newValue)
        {
            string newState = newValue ? "ErrorLayout" : "NoErrorLayout";
            VisualStateManager.GoToState(this, newState, false);
            NumberPad.IsErrorVisualState = newValue;
        }

        private void ShiftButton_Check(object sender, RoutedEventArgs e)
        {
            SetOperatorRowVisibility();
        }

        private void ShiftButton_Uncheck(object sender, RoutedEventArgs e)
        {
            ShiftButton.IsChecked = false;
            SetOperatorRowVisibility();
            ShiftButton.Focus(FocusState.Programmatic);
        }

        private void TrigFlyoutShift_Toggle(object sender, RoutedEventArgs e)
        {
            SetTrigRowVisibility();
        }

        private void TrigFlyoutHyp_Toggle(object sender, RoutedEventArgs e)
        {
            SetTrigRowVisibility();
        }

        private void FlyoutButton_Clicked(object sender, RoutedEventArgs e)
        {
            HypButton.IsChecked = false;
            TrigShiftButton.IsChecked = false;
            Trigflyout.Hide();
            FuncFlyout.Hide();
        }

        private void ShiftButton_IsEnabledChanged(object sender, DependencyPropertyChangedEventArgs e)
        {
            SetOperatorRowVisibility();
        }

        private void SetOperatorRowVisibility()
        {
            Visibility rowVis, invRowVis;
            if (ShiftButton.IsChecked.Value)
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
            InvRow1.Visibility = invRowVis;
        }

        private void SetTrigRowVisibility()
        {
            bool isShiftChecked = TrigShiftButton.IsChecked.Value;
            bool isHypeChecked = HypButton.IsChecked.Value;

            InverseHyperbolicTrigFunctions.Visibility = Visibility.Collapsed;
            InverseTrigFunctions.Visibility = Visibility.Collapsed;
            HyperbolicTrigFunctions.Visibility = Visibility.Collapsed;
            TrigFunctions.Visibility = Visibility.Collapsed;

            if (isShiftChecked && isHypeChecked)
            {
                InverseHyperbolicTrigFunctions.Visibility = Visibility.Visible;
            }
            else if (isShiftChecked)
            {
                InverseTrigFunctions.Visibility = Visibility.Visible;
            }
            else if (isHypeChecked)
            {
                HyperbolicTrigFunctions.Visibility = Visibility.Visible;
            }
            else
            {
                TrigFunctions.Visibility = Visibility.Visible;
            }
        }

        private void ClearEntryButton_LostFocus(object sender, RoutedEventArgs e)
        {
            if (ClearEntryButton.Visibility == Visibility.Collapsed && ClearButton.Visibility == Visibility.Visible)
            {
                ClearButton.Focus(FocusState.Programmatic);
            }
        }

        private void ClearButton_LostFocus(object sender, RoutedEventArgs e)
        {
            if (ClearEntryButton.Visibility == Visibility.Visible && ClearButton.Visibility == Visibility.Collapsed)
            {
                ClearEntryButton.Focus(FocusState.Programmatic);
            }
        }
    }
}
