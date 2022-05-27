using CalculatorApp.ViewModel.Common;

using GraphControl;

using System.Collections.Generic;

using Windows.UI;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;

// The User Control item template is documented at https://go.microsoft.com/fwlink/?LinkId=234236

namespace CalculatorApp
{
    public sealed partial class EquationStylePanelControl : UserControl
    {
        public EquationStylePanelControl()
        {
            InitializeComponent();

            var allStyles = new List<EquationLineStyle>
            {
                EquationLineStyle.Solid,
                EquationLineStyle.Dash,
                EquationLineStyle.Dot
            };

            StyleChooserBox.ItemsSource = allStyles;
        }

        public Windows.UI.Color SelectedColor
        {
            get => (Windows.UI.Color)GetValue(SelectedColorProperty);
            set => SetValue(SelectedColorProperty, value);
        }

        // Using a DependencyProperty as the backing store for SelectedColor.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty SelectedColorProperty =
            DependencyProperty.Register(nameof(SelectedColor), typeof(Windows.UI.Color), typeof(EquationStylePanelControl), new PropertyMetadata(Windows.UI.Colors.Black, (sender, args) =>
            {
                var self = (EquationStylePanelControl)sender;
                self.OnSelectedColorPropertyChanged((Windows.UI.Color)args.OldValue, (Windows.UI.Color)args.NewValue);
            }));

        public GraphControl.EquationLineStyle SelectedStyle
        {
            get => (GraphControl.EquationLineStyle)GetValue(SelectedStyleProperty);
            set => SetValue(SelectedStyleProperty, value);
        }

        // Using a DependencyProperty as the backing store for SelectedStyle.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty SelectedStyleProperty =
            DependencyProperty.Register(nameof(SelectedStyle), typeof(GraphControl.EquationLineStyle), typeof(EquationStylePanelControl), new PropertyMetadata(GraphControl.EquationLineStyle.Solid, (sender, args) =>
            {
                var self = (EquationStylePanelControl)sender;
                self.OnSelectedStylePropertyChanged((GraphControl.EquationLineStyle)args.OldValue, (GraphControl.EquationLineStyle)args.NewValue);
            }));

        public int SelectedColorIndex
        {
            get => (int)GetValue(SelectedColorIndexProperty);
            set => SetValue(SelectedColorIndexProperty, value);
        }

        // Using a DependencyProperty as the backing store for SelectedColorIndex.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty SelectedColorIndexProperty =
            DependencyProperty.Register(nameof(SelectedColorIndex), typeof(int), typeof(EquationStylePanelControl), new PropertyMetadata(default(int)));

        public IList<Windows.UI.Xaml.Media.SolidColorBrush> AvailableColors
        {
            get => (IList<Windows.UI.Xaml.Media.SolidColorBrush>)GetValue(AvailableColorsProperty);
            set => SetValue(AvailableColorsProperty, value);
        }

        // Using a DependencyProperty as the backing store for AvailableColors.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty AvailableColorsProperty =
            DependencyProperty.Register(nameof(AvailableColors), typeof(IList<Windows.UI.Xaml.Media.SolidColorBrush>), typeof(EquationStylePanelControl), new PropertyMetadata(null));

        public bool EnableLineStylePicker
        {
            get => (bool)GetValue(EnableLineStylePickerProperty);
            set => SetValue(EnableLineStylePickerProperty, value);
        }

        // Using a DependencyProperty as the backing store for EnableLineStylePicker.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty EnableLineStylePickerProperty =
            DependencyProperty.Register(nameof(EnableLineStylePicker), typeof(bool), typeof(EquationStylePanelControl), new PropertyMetadata(default(bool)));

        public static Windows.UI.Xaml.Media.DoubleCollection GetLinePattern(object line)
        {
            var lineStyle = ((EquationLineStyle?)line).Value;

            var linePattern = new DoubleCollection();
            switch (lineStyle)
            {
                case EquationLineStyle.Dot:
                    linePattern.Add(1);
                    break;
                case EquationLineStyle.Dash:
                    linePattern.Add(2);
                    linePattern.Add(1);
                    break;
                default:
                    break;
            }

            return linePattern;
        }

        public static string GetLineAutomationName(object line)
        {
            var resourceLoader = AppResourceProvider.GetInstance();
            var lineStyle = ((EquationLineStyle?)line).Value;

            switch (lineStyle)
            {
                case EquationLineStyle.Dot:
                    return resourceLoader.GetResourceString("dotLineStyleAutomationName");
                case EquationLineStyle.Dash:
                    return resourceLoader.GetResourceString("dashLineStyleAutomationName");
                case EquationLineStyle.Solid:
                default:
                    return resourceLoader.GetResourceString("solidLineStyleAutomationName");
            }
        }

        public static string GetColorAutomationName(Windows.UI.Xaml.Media.Brush brush)
        {
            var resourceLoader = AppResourceProvider.GetInstance();
            var color = ((SolidColorBrush)brush);

            var lightDictionary = (ResourceDictionary)(Application.Current.Resources.ThemeDictionaries["Light"]);
            var darkDictionary = (ResourceDictionary)(Application.Current.Resources.ThemeDictionaries["Default"]);
            var highContrast = (ResourceDictionary)(Application.Current.Resources.ThemeDictionaries["HighContrast"]);

            if (color == (SolidColorBrush)(lightDictionary["EquationBrush1"])
                || color == (SolidColorBrush)(darkDictionary["EquationBrush1"]))
            {
                return resourceLoader.GetResourceString("equationColor1AutomationName");
            }
            else if (color == (SolidColorBrush)(lightDictionary["EquationBrush2"])
                || color == (SolidColorBrush)(darkDictionary["EquationBrush2"]))
            {
                return resourceLoader.GetResourceString("equationColor2AutomationName");
            }
            else if (color == (SolidColorBrush)(lightDictionary["EquationBrush3"])
                || color == (SolidColorBrush)(darkDictionary["EquationBrush3"]))
            {
                return resourceLoader.GetResourceString("equationColor3AutomationName");
            }
            else if (color == (SolidColorBrush)(lightDictionary["EquationBrush4"])
                || color == (SolidColorBrush)(darkDictionary["EquationBrush4"]))
            {
                return resourceLoader.GetResourceString("equationColor4AutomationName");
            }
            else if (color == (SolidColorBrush)(lightDictionary["EquationBrush5"])
                || color == (SolidColorBrush)(darkDictionary["EquationBrush5"]))
            {
                return resourceLoader.GetResourceString("equationColor5AutomationName");
            }
            else if (color == (SolidColorBrush)(lightDictionary["EquationBrush6"])
                || color == (SolidColorBrush)(darkDictionary["EquationBrush6"]))
            {
                return resourceLoader.GetResourceString("equationColor6AutomationName");
            }
            else if (color == (SolidColorBrush)(lightDictionary["EquationBrush7"])
                || color == (SolidColorBrush)(darkDictionary["EquationBrush7"]))
            {
                return resourceLoader.GetResourceString("equationColor7AutomationName");
            }
            else if (color == (SolidColorBrush)(lightDictionary["EquationBrush8"])
                || color == (SolidColorBrush)(darkDictionary["EquationBrush8"]))
            {
                return resourceLoader.GetResourceString("equationColor8AutomationName");
            }
            else if (color == (SolidColorBrush)(lightDictionary["EquationBrush9"])
                || color == (SolidColorBrush)(darkDictionary["EquationBrush9"]))
            {
                return resourceLoader.GetResourceString("equationColor9AutomationName");
            }
            else if (color == (SolidColorBrush)(lightDictionary["EquationBrush10"])
                || color == (SolidColorBrush)(darkDictionary["EquationBrush10"]))
            {
                return resourceLoader.GetResourceString("equationColor10AutomationName");
            }
            else if (color == (SolidColorBrush)(lightDictionary["EquationBrush11"])
                || color == (SolidColorBrush)(darkDictionary["EquationBrush11"]))
            {
                return resourceLoader.GetResourceString("equationColor11AutomationName");
            }
            else if (color == (SolidColorBrush)(lightDictionary["EquationBrush12"])
                || color == (SolidColorBrush)(darkDictionary["EquationBrush12"]))
            {
                return resourceLoader.GetResourceString("equationColor12AutomationName");
            }
            else if (color == (SolidColorBrush)(lightDictionary["EquationBrush13"])
                || color == (SolidColorBrush)(darkDictionary["EquationBrush13"]))
            {
                return resourceLoader.GetResourceString("equationColor13AutomationName");
            }
            else if (color == (SolidColorBrush)(darkDictionary["EquationBrush14"]))
            {
                return resourceLoader.GetResourceString("equationColor14WhiteAutomationName");
            }
            else if (color == (SolidColorBrush)(lightDictionary["EquationBrush14"]))
            {
                return resourceLoader.GetResourceString("equationColor14BlackAutomationName");
            }
            else if (color == (SolidColorBrush)(highContrast["EquationBrush1"]))
            {
                return resourceLoader.GetResourceString("equationHighContrastColor1AutomationName");
            }
            else if (color == (SolidColorBrush)(highContrast["EquationBrush2"]))
            {
                return resourceLoader.GetResourceString("equationHighContrastColor2AutomationName");
            }
            else if (color == (SolidColorBrush)(highContrast["EquationBrush3"]))
            {
                return resourceLoader.GetResourceString("equationHighContrastColor3AutomationName");
            }
            else
            {
                return resourceLoader.GetResourceString("equationHighContrastColor4AutomationName");
            }
        }

        private void SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (e.AddedItems.Count > 0)
            {
                var brush = (e.AddedItems[0] as SolidColorBrush);
                if (brush == null)
                {
                    SelectedColor = Colors.Black;
                }
                else
                {
                    SelectedColor = brush.Color;
                }

                CalculatorApp.ViewModel.Common.TraceLogger.GetInstance().LogGraphLineStyleChanged(LineStyleType.Color);
            }
        }

        private void OnSelectedColorPropertyChanged(Color oldColor, Color newColor)
        {
            SelectColor(newColor);
        }

        private void ColorChooserLoaded(object sender, RoutedEventArgs e)
        {
            SelectColor(SelectedColor);
        }

        private void SelectColor(Color selectedColor)
        {
            for (int i = 0; i < ColorChooser.Items.Count; i++)
            {
                var item = ColorChooser.Items[i];
                var brush = ((SolidColorBrush)item);
                var gridViewItem = (ColorChooser.ContainerFromItem(brush) as GridViewItem);

                if (gridViewItem == null)
                {
                    continue;
                }

                if (Utilities.AreColorsEqual(brush.Color, selectedColor))
                {
                    gridViewItem.IsSelected = true;
                    SelectedColorIndex = i;
                    return;
                }
                else
                {
                    gridViewItem.IsSelected = false;
                }
            }
        }

        private void OnSelectedStylePropertyChanged(EquationLineStyle oldStyle, EquationLineStyle newStyle)
        {
            if (oldStyle != newStyle)
            {
                SelectStyle(newStyle);
                CalculatorApp.ViewModel.Common.TraceLogger.GetInstance().LogGraphLineStyleChanged(LineStyleType.Pattern);
            }
        }

        private void SelectStyle(EquationLineStyle selectedStyle)
        {
            foreach (var item in StyleChooserBox.Items)
            {
                var style = ((EquationLineStyle)item);
                var comboBoxItem = (StyleChooserBox.ContainerFromItem(style) as ComboBoxItem);

                if (comboBoxItem == null)
                {
                    continue;
                }

                if (style == selectedStyle)
                {
                    comboBoxItem.IsSelected = true;
                    return;
                }
                else
                {
                    comboBoxItem.IsSelected = false;
                }
            }
        }

        private void StyleChooserBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (e.AddedItems.Count > 0)
            {
                SelectedStyle = ((EquationLineStyle)e.AddedItems[0]);
            }
        }

        private void StyleChooserBox_Loaded(object sender, RoutedEventArgs e)
        {
            SelectStyle(SelectedStyle);
        }
    }
}
