using CalculatorApp.ViewModel;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

// The User Control item template is documented at https://go.microsoft.com/fwlink/?LinkId=234236

namespace CalculatorApp
{
    public sealed class DelighterUnitToStyleConverter : Windows.UI.Xaml.Data.IValueConverter
    {
        public DelighterUnitToStyleConverter()
        {
            m_delighters = new Windows.UI.Xaml.ResourceDictionary();
            m_delighters.Source = new Uri(@"ms-appx:///Views/DelighterUnitStyles.xaml");
        }

        public object Convert(object value, Type targetType, object parameter, string language)
        {
            Unit unit = (Unit)value;
            Debug.Assert(unit.IsModelUnitWhimsical());
            if (!unit.IsModelUnitWhimsical())
            {
                return null;
            }

            string key = $"Unit_{unit.ModelUnitID()}";
            return (Style)m_delighters[key];
        }

        public object ConvertBack(object value, Type targetType, object parameter, string language)
        {
            // We never use convert back, only one way binding supported
            Debug.Assert(false);
            return null;
        }

        private Windows.UI.Xaml.ResourceDictionary m_delighters;
    }

    public sealed class SupplementaryResultDataTemplateSelector : Windows.UI.Xaml.Controls.DataTemplateSelector
    {
        public SupplementaryResultDataTemplateSelector()
        { }

        public Windows.UI.Xaml.DataTemplate RegularTemplate
        {
            get => m_regularTemplate;
            set => m_regularTemplate = value;
        }

        public Windows.UI.Xaml.DataTemplate DelighterTemplate
        {
            get => m_delighterTemplate;
            set => m_delighterTemplate = value;
        }

        protected override DataTemplate SelectTemplateCore(object item, DependencyObject container)
        {
            SupplementaryResult result = (SupplementaryResult)item;
            if (result.IsWhimsical())
            {
                return DelighterTemplate;
            }
            else
            {
                return RegularTemplate;
            }
        }

        private Windows.UI.Xaml.DataTemplate m_regularTemplate;
        private Windows.UI.Xaml.DataTemplate m_delighterTemplate;
    }

    public sealed class SupplementaryResultNoOverflowStackPanel : CalculatorApp.Controls.HorizontalNoOverflowStackPanel
    {
        protected override bool ShouldPrioritizeLastItem()
        {
            if (Children.Count == 0)
            {
                return false;
            }

            var lastChild = Children[Children.Count - 1] as FrameworkElement;
            if (lastChild == null)
            {
                return false;
            }

            var suppResult = lastChild.DataContext as SupplementaryResult;
            return suppResult == null ? false : suppResult.IsWhimsical();
        }
    }

    [Windows.Foundation.Metadata.WebHostHidden]
    public sealed partial class SupplementaryResults : UserControl
    {
        public SupplementaryResults()
        {
            InitializeComponent();
        }

        public IEnumerable<ViewModel.SupplementaryResult> Results
        {
            get { return (IEnumerable<ViewModel.SupplementaryResult>)GetValue(ResultsProperty); }
            set { SetValue(ResultsProperty, value); }
        }

        // Using a DependencyProperty as the backing store for Results.  This enables animation, styling, binding, etc...
        public static readonly DependencyProperty ResultsProperty =
            DependencyProperty.Register(nameof(Results), typeof(IEnumerable<ViewModel.SupplementaryResult>), typeof(SupplementaryResult), new PropertyMetadata(null));
    }
}
