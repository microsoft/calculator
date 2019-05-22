// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.Diagnostics;
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
using CalculatorApp.Controls;
using CalculatorApp.ViewModel;

// The User Control item template is documented at https://go.microsoft.com/fwlink/?LinkId=234236

namespace CalculatorApp
{
	public sealed class DelighterUnitToStyleConverter : Windows.UI.Xaml.Data.IValueConverter
    {
		private Windows.UI.Xaml.ResourceDictionary m_delighters;

		public DelighterUnitToStyleConverter()
		{
			m_delighters = new Windows.UI.Xaml.ResourceDictionary();
			m_delighters.Source = new Uri("ms-appx:///Views/DelighterUnitStyles.xaml");
		}

		public object Convert(object value, Type targetType, object parameter, string language)
		{
			Unit unit = value as Unit;

			Debug.Assert(unit.GetModelUnit().isWhimsical);
			if ((!unit?.GetModelUnit().isWhimsical) ?? true)
			{
				return null;
			}

			string key = "Unit_" + unit.GetModelUnit().id;
			return m_delighters[key];
		}

		public object ConvertBack(object value, Type targetType, object parameter, string language)
		{
			// We never use convert back, only one way binding supported
			Debug.Assert(false);
			return null;
		}
    };

	public sealed class SupplementaryResultDataTemplateSelector : Windows.UI.Xaml.Controls.DataTemplateSelector
	{
		Windows.UI.Xaml.DataTemplate m_regularTemplate;
		Windows.UI.Xaml.DataTemplate m_delighterTemplate;

		public Windows.UI.Xaml.DataTemplate RegularTemplate
		{
			get { return m_regularTemplate; }
			set { m_regularTemplate = value; }
		}

		public Windows.UI.Xaml.DataTemplate DelighterTemplate
		{
			get { return m_delighterTemplate; }
			set { m_delighterTemplate = value; }
		}

		protected override Windows.UI.Xaml.DataTemplate SelectTemplateCore(object item, DependencyObject container)
		{
			SupplementaryResult result = item as SupplementaryResult;
			if (result?.IsWhimsical() ?? false)
			{
				return DelighterTemplate;
			}
			else
			{
				return RegularTemplate;
			}
		}
	}

	public sealed partial class SupplementaryResults : UserControl
	{
		// TODO UNO: DEPENDENCY_PROPERTY_OWNER(SupplementaryResults);
		// DEPENDENCY_PROPERTY_INITIALIZATION(SupplementaryResults, Results);

		public static readonly DependencyProperty ResultProperty = DependencyProperty.Register(
			"Results",
			typeof(System.Collections.Generic.IEnumerable<ViewModel.SupplementaryResult>),
			typeof(SupplementaryResults),
			new PropertyMetadata(default(System.Collections.Generic.IEnumerable<ViewModel.SupplementaryResult>)));

		public System.Collections.Generic.IEnumerable<ViewModel.SupplementaryResult> Results
		{
			get { return (System.Collections.Generic.IEnumerable<ViewModel.SupplementaryResult>)GetValue(ResultProperty); }
			set { SetValue(ResultProperty, value); }
		}

		public SupplementaryResults()
		{
			Results = new CalculatorObservableCollection<SupplementaryResult>();

			this.InitializeComponent();
		}
	}

	public sealed partial class SupplementaryResultNoOverflowStackPanel : HorizontalNoOverflowStackPanel
	{
		protected override bool ShouldPrioritizeLastItem()
		{
			if (Children.Count == 0)
			{
				return false;
			}
			var lastChild = Children.Last() as FrameworkElement;
			if (lastChild == null)
			{
				return false;
			}
			var suppResult = lastChild.DataContext as SupplementaryResult;
			return suppResult == null ? false : suppResult.IsWhimsical();
		}
	}
}
