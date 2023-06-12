// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;

namespace CalculatorApp
{
    namespace Controls
    {
        public sealed class OperatorPanelButton : Windows.UI.Xaml.Controls.Primitives.ToggleButton
        {
            public OperatorPanelButton()
            {
            }

            public string Text
            {
                get => (string)GetValue(TextProperty);
                set => SetValue(TextProperty, value);
            }

            // Using a DependencyProperty as the backing store for Text.  This enables animation, styling, binding, etc...
            public static readonly DependencyProperty TextProperty =
                DependencyProperty.Register(nameof(Text), typeof(string), typeof(OperatorPanelButton), new PropertyMetadata(string.Empty));

            public string Glyph
            {
                get => (string)GetValue(GlyphProperty);
                set => SetValue(GlyphProperty, value);
            }

            // Using a DependencyProperty as the backing store for Glyph.  This enables animation, styling, binding, etc...
            public static readonly DependencyProperty GlyphProperty =
                DependencyProperty.Register(nameof(Glyph), typeof(string), typeof(OperatorPanelButton), new PropertyMetadata(string.Empty));

            public double GlyphFontSize
            {
                get => (double)GetValue(GlyphFontSizeProperty);
                set => SetValue(GlyphFontSizeProperty, value);
            }

            // Using a DependencyProperty as the backing store for GlyphFontSize.  This enables animation, styling, binding, etc...
            public static readonly DependencyProperty GlyphFontSizeProperty =
                DependencyProperty.Register(nameof(GlyphFontSize), typeof(double), typeof(OperatorPanelButton), new PropertyMetadata(default(double)));

            public double ChevronFontSize
            {
                get => (double)GetValue(ChevronFontSizeProperty);
                set => SetValue(ChevronFontSizeProperty, value);
            }

            // Using a DependencyProperty as the backing store for ChevronFontSize.  This enables animation, styling, binding, etc...
            public static readonly DependencyProperty ChevronFontSizeProperty =
                DependencyProperty.Register(nameof(ChevronFontSize), typeof(double), typeof(OperatorPanelButton), new PropertyMetadata(default(double)));

            public Flyout FlyoutMenu
            {
                get => (Flyout)GetValue(FlyoutMenuProperty);
                set => SetValue(FlyoutMenuProperty, value);
            }

            // Using a DependencyProperty as the backing store for FlyoutMenu.  This enables animation, styling, binding, etc...
            public static readonly DependencyProperty FlyoutMenuProperty =
                DependencyProperty.Register(nameof(FlyoutMenu), typeof(Flyout), typeof(OperatorPanelButton), new PropertyMetadata(default(Flyout)));

            protected override void OnApplyTemplate()
            {
                if (FlyoutMenu != null)
                {
                    FlyoutMenu.Closed += FlyoutClosed;
                }
            }

            protected override void OnToggle()
            {
                base.OnToggle();

                if (IsChecked == true)
                {
                    FlyoutMenu.ShowAt(this);
                }
            }

            private void FlyoutClosed(object sender, object args)
            {
                IsChecked = false;
            }
        }
    }
}
