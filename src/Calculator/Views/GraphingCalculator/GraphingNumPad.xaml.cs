// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using CalculatorApp.ViewModel.Common;

using System;
using System.Collections.Generic;

using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Input;

namespace CalculatorApp
{
    [Windows.Foundation.Metadata.WebHostHidden]
    public sealed partial class GraphingNumPad
    {
        public GraphingNumPad()
        {
            InitializeComponent();
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

        private void ShiftButton_Check(object sender, RoutedEventArgs e)
        {
            SetOperatorRowVisibility();
        }

        private void ShiftButton_Uncheck(object sender, RoutedEventArgs e)
        {
            ShiftButton.IsChecked = false;
            SetOperatorRowVisibility();

            Button_Clicked(sender, null);
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
            this.HypButton.IsChecked = false;
            this.TrigShiftButton.IsChecked = false;
            this.Trigflyout.Hide();
            this.FuncFlyout.Hide();
            this.InequalityFlyout.Hide();

            Button_Clicked(sender, null);
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
            else if (isShiftChecked && !isHypeChecked)
            {
                InverseTrigFunctions.Visibility = Visibility.Visible;
            }
            else if (!isShiftChecked && isHypeChecked)
            {
                HyperbolicTrigFunctions.Visibility = Visibility.Visible;
            }
            else
            {
                TrigFunctions.Visibility = Visibility.Visible;
            }
        }

        private void Button_Clicked(object sender, RoutedEventArgs e)
        {
            var mathRichEdit = GetActiveRichEdit();
            var button = sender as CalculatorApp.Controls.CalculatorButton;
            if (mathRichEdit != null && sender != null)
            {
                var id = button.ButtonId;
                TraceLogger.GetInstance().UpdateButtonUsage(id, CalculatorApp.ViewModel.Common.ViewMode.Graphing);
                Tuple<string, int, int> output = GraphingNumPad.GetButtonOutput(id);
                mathRichEdit.InsertText(output.Item1, output.Item2, output.Item3);
            }
        }

        private void SubmitButton_Clicked(object sender, RoutedEventArgs e)
        {
            var mathRichEdit = GetActiveRichEdit();
            if (mathRichEdit != null)
            {
                mathRichEdit.SubmitEquation(CalculatorApp.Controls.EquationSubmissionSource.ENTER_KEY);
                TraceLogger.GetInstance().UpdateButtonUsage(NumbersAndOperatorsEnum.Submit, CalculatorApp.ViewModel.Common.ViewMode.Graphing);
            }
        }

        private void ClearButton_Clicked(object sender, RoutedEventArgs e)
        {
            var mathRichEdit = GetActiveRichEdit();
            if (mathRichEdit != null)
            {
                mathRichEdit.TextDocument.GetText(Windows.UI.Text.TextGetOptions.NoHidden, out string text);

                if (!string.IsNullOrEmpty(text))
                {
                    mathRichEdit.MathText = "<math xmlns=\"http://www.w3.org/1998/Math/MathML\"></math>";
                    mathRichEdit.SubmitEquation(CalculatorApp.Controls.EquationSubmissionSource.PROGRAMMATIC);
                }

                TraceLogger.GetInstance().UpdateButtonUsage(NumbersAndOperatorsEnum.Clear, CalculatorApp.ViewModel.Common.ViewMode.Graphing);
            }
        }

        private void BackSpaceButton_Clicked(object sender, RoutedEventArgs e)
        {
            var mathRichEdit = GetActiveRichEdit();
            if (mathRichEdit != null)
            {
                mathRichEdit.BackSpace();
                TraceLogger.GetInstance().UpdateButtonUsage(NumbersAndOperatorsEnum.Backspace, CalculatorApp.ViewModel.Common.ViewMode.Graphing);
            }
        }

        // To avoid focus moving when the space between buttons is clicked, handle click events that make it through the keypad.
        private void GraphingNumPad_PointerPressed(object sender, PointerRoutedEventArgs e)
        {
            e.Handled = true;
        }

        private CalculatorApp.Controls.MathRichEditBox GetActiveRichEdit()
        {
            return FocusManager.GetFocusedElement() as Controls.MathRichEditBox;
        }

        // Adding event because the ShowMode property is ignored in xaml.
        private void Flyout_Opening(object sender, object e)
        {
            if (sender is Flyout flyout)
            {
                flyout.ShowMode = FlyoutShowMode.Transient;
            }
        }

        private static readonly Dictionary<NumbersAndOperatorsEnum, Tuple<string, int, int>> buttonOutput = new Dictionary<NumbersAndOperatorsEnum, Tuple<string, int, int>>
        {
            { NumbersAndOperatorsEnum.Sin, Tuple.Create("sin()", 4, 0) },
            { NumbersAndOperatorsEnum.Cos, Tuple.Create("cos()", 4, 0) },
            { NumbersAndOperatorsEnum.Tan, Tuple.Create("tan()", 4, 0) },
            { NumbersAndOperatorsEnum.Sec, Tuple.Create("sec()", 4, 0) },
            { NumbersAndOperatorsEnum.Csc, Tuple.Create("csc()", 4, 0) },
            { NumbersAndOperatorsEnum.Cot, Tuple.Create("cot()", 4, 0) },
            { NumbersAndOperatorsEnum.InvSin, Tuple.Create("arcsin()", 7, 0) },
            { NumbersAndOperatorsEnum.InvCos, Tuple.Create("arccos()", 7, 0) },
            { NumbersAndOperatorsEnum.InvTan, Tuple.Create("arctan()", 7, 0) },
            { NumbersAndOperatorsEnum.InvSec, Tuple.Create("arcsec()", 7, 0) },
            { NumbersAndOperatorsEnum.InvCsc, Tuple.Create("arccsc()", 7, 0) },
            { NumbersAndOperatorsEnum.InvCot, Tuple.Create("arccot()", 7, 0) },
            { NumbersAndOperatorsEnum.Sinh, Tuple.Create("sinh()", 5, 0) },
            { NumbersAndOperatorsEnum.Cosh, Tuple.Create("cosh()", 5, 0) },
            { NumbersAndOperatorsEnum.Tanh, Tuple.Create("tanh()", 5, 0) },
            { NumbersAndOperatorsEnum.Sech, Tuple.Create("sech()", 5, 0) },
            { NumbersAndOperatorsEnum.Csch, Tuple.Create("csch()", 5, 0) },
            { NumbersAndOperatorsEnum.Coth, Tuple.Create("coth()", 5, 0) },
            { NumbersAndOperatorsEnum.InvSinh, Tuple.Create("arcsinh()", 8, 0) },
            { NumbersAndOperatorsEnum.InvCosh, Tuple.Create("arccosh()", 8, 0) },
            { NumbersAndOperatorsEnum.InvTanh, Tuple.Create("arctanh()", 8, 0) },
            { NumbersAndOperatorsEnum.InvSech, Tuple.Create("arcsech()", 8, 0) },
            { NumbersAndOperatorsEnum.InvCsch, Tuple.Create("arccsch()", 8, 0) },
            { NumbersAndOperatorsEnum.InvCoth, Tuple.Create("arccoth()", 8, 0) },
            { NumbersAndOperatorsEnum.Abs, Tuple.Create("abs()", 4, 0) },
            { NumbersAndOperatorsEnum.Floor, Tuple.Create("floor()", 6, 0) },
            { NumbersAndOperatorsEnum.Ceil, Tuple.Create("ceiling()", 8, 0) },
            { NumbersAndOperatorsEnum.Pi, Tuple.Create("\u03C0", 1, 0) },
            { NumbersAndOperatorsEnum.Euler, Tuple.Create("e", 1, 0) },
            { NumbersAndOperatorsEnum.XPower2, Tuple.Create("^2", 2, 0) },
            { NumbersAndOperatorsEnum.Cube, Tuple.Create("^3", 2, 0) },
            { NumbersAndOperatorsEnum.XPowerY, Tuple.Create("^", 1, 0) },
            { NumbersAndOperatorsEnum.TenPowerX, Tuple.Create("10^", 3, 0) },
            { NumbersAndOperatorsEnum.LogBase10, Tuple.Create("log()", 4, 0) },
            { NumbersAndOperatorsEnum.LogBaseE, Tuple.Create("ln()", 3, 0) },
            { NumbersAndOperatorsEnum.Sqrt, Tuple.Create("sqrt()", 5, 0) },
            { NumbersAndOperatorsEnum.CubeRoot, Tuple.Create("cbrt()", 5, 0) },
            { NumbersAndOperatorsEnum.YRootX, Tuple.Create("root(x" + LocalizationSettings.GetInstance().GetListSeparatorWinRT().TrimEnd('\0') + "n)", 7, 1) },
            { NumbersAndOperatorsEnum.TwoPowerX, Tuple.Create("2^", 2, 0) },
            { NumbersAndOperatorsEnum.LogBaseY, Tuple.Create("log(b" + LocalizationSettings.GetInstance().GetListSeparatorWinRT().TrimEnd('\0') + " x)", 4, 1) },
            { NumbersAndOperatorsEnum.EPowerX, Tuple.Create("e^", 4, 0) },
            { NumbersAndOperatorsEnum.X, Tuple.Create("x", 1, 0) },
            { NumbersAndOperatorsEnum.Y, Tuple.Create("y", 1, 0) },
            { NumbersAndOperatorsEnum.OpenParenthesis, Tuple.Create("(", 1, 0) },
            { NumbersAndOperatorsEnum.CloseParenthesis, Tuple.Create(")", 1, 0) },
            { NumbersAndOperatorsEnum.Equals, Tuple.Create("=", 1, 0) },
            { NumbersAndOperatorsEnum.Divide, Tuple.Create("/", 1, 0) },
            { NumbersAndOperatorsEnum.Multiply, Tuple.Create("*", 1, 0) },
            { NumbersAndOperatorsEnum.Subtract, Tuple.Create("-", 1, 0) },
            { NumbersAndOperatorsEnum.Add, Tuple.Create("+", 1, 0) },
            { NumbersAndOperatorsEnum.Invert, Tuple.Create("1/", 2, 0) },
            { NumbersAndOperatorsEnum.Negate, Tuple.Create("-", 1, 0) },
            { NumbersAndOperatorsEnum.GreaterThan, Tuple.Create(">", 1, 0) },
            { NumbersAndOperatorsEnum.GreaterThanOrEqualTo, Tuple.Create("\u2265", 1, 0) },
            { NumbersAndOperatorsEnum.LessThan, Tuple.Create("<", 1, 0) },
            { NumbersAndOperatorsEnum.LessThanOrEqualTo, Tuple.Create("\u2264", 1, 0) },
            { NumbersAndOperatorsEnum.Zero, Tuple.Create("0", 1, 0) },
            { NumbersAndOperatorsEnum.One, Tuple.Create("1", 1, 0) },
            { NumbersAndOperatorsEnum.Two, Tuple.Create("2", 1, 0) },
            { NumbersAndOperatorsEnum.Three, Tuple.Create("3", 1, 0) },
            { NumbersAndOperatorsEnum.Four, Tuple.Create("4", 1, 0) },
            { NumbersAndOperatorsEnum.Five, Tuple.Create("5", 1, 0) },
            { NumbersAndOperatorsEnum.Six, Tuple.Create("6", 1, 0) },
            { NumbersAndOperatorsEnum.Seven, Tuple.Create("7", 1, 0) },
            { NumbersAndOperatorsEnum.Eight, Tuple.Create("8", 1, 0) },
            { NumbersAndOperatorsEnum.Nine, Tuple.Create("9", 1, 0) },
            { NumbersAndOperatorsEnum.Decimal, Tuple.Create(LocalizationSettings.GetInstance().GetDecimalSeparatorStrWinRT().TrimEnd('\0'), 1, 0) },
        };

        private static Tuple<string, int, int> GetButtonOutput(NumbersAndOperatorsEnum id)
        {
            if (buttonOutput.TryGetValue(id, out Tuple<string, int, int> output))
            {
                return output;
            }

            return null;
        }

        private static readonly LocalizationSettings localizationSettings = LocalizationSettings.GetInstance();
    }
}
