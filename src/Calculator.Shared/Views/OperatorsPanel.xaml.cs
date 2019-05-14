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
    public sealed partial class OperatorsPanel : UserControl
    {


        public bool IsBitFlipChecked
        {
            get { return (bool)GetValue(IsBitFlipCheckedProperty); }
            set { SetValue(IsBitFlipCheckedProperty, value); }
        }

        public static readonly DependencyProperty IsBitFlipCheckedProperty =
            DependencyProperty.Register("IsBitFlipChecked", typeof(bool), typeof(OperatorsPanel), new PropertyMetadata(false));



        public bool IsErrorVisualState
        {
            get { return (bool)GetValue(IsErrorVisualStateProperty); }
            set { SetValue(IsErrorVisualStateProperty, value); }
        }

        // Using a DependencyProperty as the backing store for IsErrorVisualState.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty IsErrorVisualStateProperty =
            DependencyProperty.Register("IsErrorVisualState", typeof(bool), typeof(OperatorsPanel), new PropertyMetadata(false));

        public OperatorsPanel()
        {
            this.InitializeComponent();
        }

        StandardCalculatorViewModel Model => (CalculatorApp.ViewModel.StandardCalculatorViewModel)(this.DataContext);

        void OnIsBitFlipCheckedPropertyChanged(bool oldValue, bool newValue)
        {
            if (newValue)
            {
                EnsureProgrammerBitFlipPanel();
            }
        }

        void OnIsErrorVisualStatePropertyChanged(bool oldValue, bool newValue)
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

        void EnsureScientificOps()
        {
            if (ScientificOperators == null)
            {
                this.FindName("ScientificOperators");
            }
        }

        void EnsureProgrammerRadixOps()
        {
            if (ProgrammerRadixOperators == null)
            {
                this.FindName("ProgrammerRadixOperators");
            }
        }

        void EnsureProgrammerBitFlipPanel()
        {
            if (BitFlipPanel == null)
            {
                this.FindName("BitFlipPanel");
            }
        }

    }
}
