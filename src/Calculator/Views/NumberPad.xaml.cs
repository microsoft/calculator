using CalculatorApp.ViewModel.Common;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Windows.UI.Core;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Media;

namespace CalculatorApp
{
    public sealed partial class NumberPad : UserControl
    {
        private bool m_isErrorVisualState;

        // Ancestor grid column mapping state
        private Grid _ancestorGrid;
        private int _ancestorStartColumn = -1;
        private int _ancestorSpan = 0;
        private bool _applyingColumnMap;

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

            Loaded += NumberPad_Loaded;
        }

        private async void NumberPad_Loaded(object sender, RoutedEventArgs e)
        {
            InitializeAncestorColumnBinding();
            // Wait a layout pass so ancestor ActualWidth values are valid
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () => { });
            ApplyAncestorColumnWidths();
        }

        private void InitializeAncestorColumnBinding()
        {
            var (grid, elementWithIndex, colIdx) = GetAncestorGridAndColumnIndex();
            if (grid == null)
                return;

            _ancestorGrid = grid;
            int span = 1;
            if (elementWithIndex != null)
                span = Math.Max(1, Grid.GetColumnSpan(elementWithIndex));

            _ancestorStartColumn = Math.Max(0, colIdx);
            _ancestorSpan = span;

            _ancestorGrid.SizeChanged -= AncestorGrid_SizeChanged;
            _ancestorGrid.SizeChanged += AncestorGrid_SizeChanged;
        }

        private async void AncestorGrid_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            // Defer to let inner layout settle
            await Dispatcher.RunAsync(CoreDispatcherPriority.Low, () => ApplyAncestorColumnWidths());
        }

        private void ApplyAncestorColumnWidths()
        {
            if (_ancestorGrid == null || _applyingColumnMap)
                return;

            int ancestorColumnCount = _ancestorGrid.ColumnDefinitions.Count;
            if (ancestorColumnCount == 0 || _ancestorStartColumn >= ancestorColumnCount)
                return;

            int effectiveSpan = Math.Min(_ancestorSpan, ancestorColumnCount - _ancestorStartColumn);
            if (effectiveSpan <= 0)
                return;

            var slice = new List<double>(effectiveSpan);
            for (int i = 0; i < effectiveSpan; i++)
            {
                slice.Add(_ancestorGrid.ColumnDefinitions[_ancestorStartColumn + i].ActualWidth);
            }

            // If any width is zero (not measured yet) skip this attempt
            if (slice.Any(w => w <= 0.0))
                return;

            var targetColumns = Root.ColumnDefinitions;
            if (targetColumns.Count == 0)
                return;

            _applyingColumnMap = true;
            try
            {
                if (targetColumns.Count == effectiveSpan)
                {
                    for (int i = 0; i < effectiveSpan; i++)
                    {
                        double w = slice[i];
                        var newWidth = new GridLength(w, GridUnitType.Pixel);
                        if (!GridLengthEquals(targetColumns[i].Width, newWidth))
                            targetColumns[i].Width = newWidth;
                    }
                }
                else
                {
                    double total = slice.Sum();
                    if (total <= 0) return;
                    for (int i = 0; i < targetColumns.Count; i++)
                    {
                        double ratio = slice[i % effectiveSpan] / total;
                        ratio = Math.Max(ratio, 0.0001);
                        // Scale ratios so sum ~ targetColumns.Count
                        double starValue = ratio * targetColumns.Count;
                        // Bound star value
                        starValue = Math.Max(0.0001, starValue);
                        var newWidth = new GridLength(starValue, GridUnitType.Star);
                        if (!GridLengthEquals(targetColumns[i].Width, newWidth))
                            targetColumns[i].Width = newWidth;
                    }
                }
            }
            finally
            {
                _applyingColumnMap = false;
            }
        }

        private static bool GridLengthEquals(GridLength a, GridLength b)
        {
            if (a.GridUnitType != b.GridUnitType) return false;
            if (a.GridUnitType == GridUnitType.Pixel)
                return Math.Abs(a.Value - b.Value) < 0.5;
            return Math.Abs(a.Value - b.Value) < 0.0001;
        }

        #region Ancestor Grid Introspection (existing)

        public async Task<IReadOnlyList<double>> GetAncestorGridColumnWidthsAsync()
        {
            await Dispatcher.RunAsync(CoreDispatcherPriority.Normal, () => { });
            var grid = FindAncestorGridWithColumns(this);
            if (grid == null || grid.ColumnDefinitions.Count == 0)
                return Array.Empty<double>();

            return grid.ColumnDefinitions.Select(cd => cd.ActualWidth).ToList().AsReadOnly();
        }

        public (Grid grid, FrameworkElement elementWithIndex, int columnIndex) GetAncestorGridAndColumnIndex()
        {
            FrameworkElement elementWithIndex = null;
            int columnIndex = -1;

            Grid targetGrid = FindAncestorGridWithColumns(this);
            if (targetGrid == null)
                return (null, null, -1);

            DependencyObject current = this;
            while (current != null && current != targetGrid)
            {
                if (current is FrameworkElement fe)
                {
                    int idx = Grid.GetColumn(fe);
                    elementWithIndex = fe;
                    columnIndex = idx;
                }
                current = VisualTreeHelper.GetParent(current);
            }

            return (targetGrid, elementWithIndex, columnIndex);
        }

        public IDictionary<int, double> GetAncestorButtonColumnWidths()
        {
            var (grid, _, _) = GetAncestorGridAndColumnIndex();
            if (grid == null)
                return new Dictionary<int, double>();

            if (grid.ColumnDefinitions.Count > 0)
            {
                return grid.ColumnDefinitions
                           .Select((cd, i) => new { i, cd.ActualWidth })
                           .ToDictionary(p => p.i, p => p.ActualWidth);
            }

            var result = new Dictionary<int, double>();
            foreach (var btn in grid.Children.OfType<Controls.CalculatorButton>())
            {
                int col = Grid.GetColumn(btn);
                double w = btn.ActualWidth;
                if (!result.ContainsKey(col) || w > result[col])
                    result[col] = w;
            }
            return result;
        }

        private static Grid FindAncestorGridWithColumns(DependencyObject start)
        {
            var current = start;
            while (current != null)
            {
                current = VisualTreeHelper.GetParent(current);
                if (current is Grid g && g.ColumnDefinitions.Count > 0)
                    return g;
            }
            return null;
        }

        private static bool IsDescendant(FrameworkElement root, FrameworkElement target)
        {
            if (root == target) return true;
            int count = VisualTreeHelper.GetChildrenCount(root);
            for (int i = 0; i < count; i++)
            {
                if (VisualTreeHelper.GetChild(root, i) is FrameworkElement fe && IsDescendant(fe, target))
                    return true;
            }
            return false;
        }

        #endregion

        public Style ButtonStyle
        {
            get => (Style)GetValue(ButtonStyleProperty);
            set => SetValue(ButtonStyleProperty, value);
        }
        public static readonly DependencyProperty ButtonStyleProperty =
            DependencyProperty.Register(nameof(ButtonStyle), typeof(Style), typeof(NumberPad), new PropertyMetadata(default(Style)));

        public NumberBase CurrentRadixType
        {
            get => (NumberBase)GetValue(CurrentRadixTypeProperty);
            set => SetValue(CurrentRadixTypeProperty, value);
        }
        public static readonly DependencyProperty CurrentRadixTypeProperty =
            DependencyProperty.Register(nameof(CurrentRadixType), typeof(NumberBase), typeof(NumberPad),
                new PropertyMetadata(NumberBase.DecBase, (sender, args) =>
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
                    VisualStateManager.GoToState(this, m_isErrorVisualState ? "ErrorLayout" : "NoErrorLayout", false);
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
    }
}
