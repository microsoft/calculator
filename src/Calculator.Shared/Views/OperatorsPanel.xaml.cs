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
			DependencyProperty.Register(
				name: "IsBitFlipChecked",
				propertyType: typeof(bool),
				ownerType: typeof(OperatorsPanel),
				typeMetadata: new PropertyMetadata(
					defaultValue: false,
					propertyChangedCallback: (s, e) => (s as OperatorsPanel)?.OnIsBitFlipCheckedPropertyChanged(
						oldValue: (bool)e.OldValue,
						newValue: (bool)e.NewValue)));

		public bool IsErrorVisualState
		{
			get => (bool)GetValue(IsErrorVisualStateProperty);
			set => SetValue(IsErrorVisualStateProperty, value);
		}

		// Using a DependencyProperty as the backing store for IsErrorVisualState.  This enables animation, styling, binding, etc...
		public static readonly DependencyProperty IsErrorVisualStateProperty =
			DependencyProperty.Register(
				name: "IsErrorVisualState",
				propertyType: typeof(bool),
				ownerType: typeof(OperatorsPanel),
				typeMetadata: new PropertyMetadata(
					defaultValue: false,
					propertyChangedCallback: (s, e) => (s as OperatorsPanel)?.OnIsErrorVisualStatePropertyChanged(
						oldValue: (bool)e.OldValue,
						newValue: (bool)e.NewValue)));

		public OperatorsPanel()
		{
			this.InitializeComponent();
		}

		private StandardCalculatorViewModel Model => (CalculatorApp.ViewModel.StandardCalculatorViewModel)(this.DataContext);

		private void OnIsBitFlipCheckedPropertyChanged(bool oldValue, bool newValue)
		{
			if (newValue)
			{
				EnsureProgrammerBitFlipPanel();
			}
		}

		private void OnIsErrorVisualStatePropertyChanged(bool oldValue, bool newValue)
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

		internal void EnsureStandardOps()
		{
			if (StandardOperators == null)
			{
				this.FindName("StandardOperators");
			}
		}

		internal void EnsureScientificOps()
		{
			if (ScientificOperators == null)
			{
				this.FindName("ScientificOperators");
			}
		}

		internal void EnsureProgrammerRadixOps()
		{
			if (ProgrammerRadixOperators == null)
			{
				this.FindName("ProgrammerRadixOperators");
			}
		}

		internal void EnsureProgrammerBitFlipPanel()
		{
			if (BitFlipPanel == null)
			{
				this.FindName("BitFlipPanel");
			}
		}

	}
}
