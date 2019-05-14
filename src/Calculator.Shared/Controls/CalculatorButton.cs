using CalculatorApp.Common;
using System;
using System.Collections.Generic;
using System.Text;
using Windows.System;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;

namespace CalculatorApp.Controls
{
	public partial class CalculatorButton : Button
	{
		public CalculatorButton()
		{
			// Set the default bindings for this button, these can be overwritten by Xaml if needed
			// These are a replacement for binding in styles
			Binding commandBinding = new Binding();
			commandBinding.Path = new PropertyPath("ButtonPressed");
			this.SetBinding(Button.CommandProperty, commandBinding);
		}

		// ButtonId
		public NumbersAndOperatorsEnum ButtonId
		{
			get { return (NumbersAndOperatorsEnum)GetValue(ButtonIdProperty); }
			set { SetValue(ButtonIdProperty, value); }
		}

		public static readonly DependencyProperty ButtonIdProperty =
			DependencyProperty.Register(
                "ButtonId",
                typeof(NumbersAndOperatorsEnum), 
                typeof(CalculatorButton),
                new PropertyMetadata(
                    null, 
                    (s, e) => (s as CalculatorButton)?.OnButtonIdPropertyChanged(
                        (NumbersAndOperatorsEnum)(e.OldValue ?? NumbersAndOperatorsEnum.None),
                        (NumbersAndOperatorsEnum)(e.NewValue ?? NumbersAndOperatorsEnum.None)
                    )
                )
            );

		// AuditoryFeedback
		public string AuditoryFeedback
		{
			get { return (string)GetValue(AuditoryFeedbackProperty); }
			set { SetValue(AuditoryFeedbackProperty, value); }
		}

		public static readonly DependencyProperty AuditoryFeedbackProperty =
			DependencyProperty.Register("AuditoryFeedback", typeof(string), typeof(CalculatorButton), new PropertyMetadata(null));

		// HoverBackground
		public Brush HoverBackground
		{
			get { return (Brush)GetValue(HoverBackgroundProperty); }
			set { SetValue(HoverBackgroundProperty, value); }
		}

		public static readonly DependencyProperty HoverBackgroundProperty =
			DependencyProperty.Register("HoverBackground", typeof(Brush), typeof(CalculatorButton), new PropertyMetadata(null));

		// HoverForeground
		public Brush HoverForeground
		{
			get { return (Brush)GetValue(HoverForegroundProperty); }
			set { SetValue(HoverForegroundProperty, value); }
		}

		public static readonly DependencyProperty HoverForegroundProperty =
			DependencyProperty.Register("HoverForeground", typeof(Brush), typeof(CalculatorButton), new PropertyMetadata(null));

		// PressBackground
		public Brush PressBackground
		{
			get { return (Brush)GetValue(PressBackgroundProperty); }
			set { SetValue(PressBackgroundProperty, value); }
		}

		public static readonly DependencyProperty PressBackgroundProperty =
			DependencyProperty.Register("PressBackground", typeof(Brush), typeof(CalculatorButton), new PropertyMetadata(null));

		// PressForeground
		public Brush PressForeground
		{
			get { return (Brush)GetValue(PressForegroundProperty); }
			set { SetValue(PressBackgroundProperty, value); }
		}

		public static readonly DependencyProperty PressForegroundProperty =
			DependencyProperty.Register("PressForeground", typeof(Brush), typeof(CalculatorButton), new PropertyMetadata(null));

		// PROTECTED 

		protected override void OnKeyDown(KeyRoutedEventArgs e)
		{
			// Ignore the Enter key
			if (e.Key == VirtualKey.Enter)
			{
				return;
			}

			base.OnKeyDown(e);
		}

        protected override void OnKeyUp(KeyRoutedEventArgs e)
		{
			// Ignore the Enter key
			if (e.Key == VirtualKey.Enter)
			{
				return;
			}

            base.OnKeyUp(e);
		}

		// PRIVATE

		private void OnButtonIdPropertyChanged(NumbersAndOperatorsEnum oldValue, NumbersAndOperatorsEnum newValue)
		{
			this.CommandParameter = new CalculatorButtonPressedEventArgs(AuditoryFeedback, newValue);
		}

		private void OnAuditoryFeedbackPropertyChanged(string oldValue, string newValue)
		{
			this.CommandParameter = new CalculatorButtonPressedEventArgs(newValue, ButtonId);
		}
	}
}
