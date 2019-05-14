using CalculationManager;
using CalculatorApp;
using CalculatorApp.Controls;
using CalculatorApp.ViewModel;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Core;
using Windows.UI.ViewManagement;
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
    public sealed partial class CalculatorProgrammerOperators : UserControl
    {
        public StandardCalculatorViewModel Model => (StandardCalculatorViewModel)(this.DataContext);

        public Style SymbolButtonStyle
        {
            get { return (Style)GetValue(SymbolButtonStyleProperty); }
            set { SetValue(SymbolButtonStyleProperty, value); }
        }

        // Using a DependencyProperty as the backing store for SymbolButtonStyle.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty SymbolButtonStyleProperty =
            DependencyProperty.Register("SymbolButtonStyle", typeof(Style), typeof(CalculatorProgrammerOperators), new PropertyMetadata(null));


        public CalculatorProgrammerOperators()
        {
            this.InitializeComponent();

            // UNO TODO
            // CopyMenuItem.Text = AppResourceProvider.GetInstance().GetResourceString("copyMenuItem");
        }

        void HexButtonChecked(object sender, RoutedEventArgs e)
        {
            // TraceLogger.GetInstance().LogRadixButtonUsed(ApplicationView.GetApplicationViewIdForWindow(CoreWindow.GetForCurrentThread()));
            if (Model != null)
            {
                Model.SwitchProgrammerModeBase(RADIX_TYPE.HEX_RADIX);
            }
        }

        void DecButtonChecked(object sender, RoutedEventArgs e)
        {
            // TraceLogger.GetInstance().LogRadixButtonUsed(ApplicationView.GetApplicationViewIdForWindow(CoreWindow.GetForCurrentThread()));
            if (Model != null)
            {
                Model.SwitchProgrammerModeBase(RADIX_TYPE.DEC_RADIX);
            }
        }

        void OctButtonChecked(object sender, RoutedEventArgs e)
        {
            // TraceLogger.GetInstance().LogRadixButtonUsed(ApplicationView.GetApplicationViewIdForWindow(CoreWindow.GetForCurrentThread()));
            if (Model != null)
            {
                Model.SwitchProgrammerModeBase(RADIX_TYPE.OCT_RADIX);
            }
        }

        void BinButtonChecked(object sender, RoutedEventArgs e)
        {
            // TraceLogger.GetInstance().LogRadixButtonUsed(ApplicationView.GetApplicationViewIdForWindow(CoreWindow.GetForCurrentThread()));
            if (Model != null)
            {
                Model.SwitchProgrammerModeBase(RADIX_TYPE.BIN_RADIX);
            }
        }

        void SetRadixButton(RADIX_TYPE radixType)
        {
            switch (radixType)
            {
                case RADIX_TYPE.DEC_RADIX:
                    {
                        decimalButton.IsChecked = true;
                        break;
                    }
                case RADIX_TYPE.HEX_RADIX:
                    {
                        hexButton.IsChecked = true;
                        break;
                    }
                case RADIX_TYPE.OCT_RADIX:
                    {
                        octButton.IsChecked = true;
                        break;
                    }
                case RADIX_TYPE.BIN_RADIX:
                    {
                        binaryButton.IsChecked = true;
                        break;
                    }
                default:
                    System.Diagnostics.Debug.Assert(false);
                    break;
            }
        }

        void OnCopyMenuItemClicked(object sender, RoutedEventArgs e)
        {
            // UNO TODO 
            // var source = (RadixButton)(ProgrammerOperatorsContextMenu.Target);

            // CopyPasteManager.CopyToClipboard(source.GetRawDisplayValue());
        }
    }
}
