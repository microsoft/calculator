// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using CalculatorApp.ViewModel.Common;

using Windows.System;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Input;

namespace CalculatorApp
{
    namespace Controls
    {
        public sealed class FlipButtons : Windows.UI.Xaml.Controls.Primitives.ToggleButton
        {
            public NumbersAndOperatorsEnum ButtonId
            {
                get => (NumbersAndOperatorsEnum)GetValue(ButtonIdProperty);
                set => SetValue(ButtonIdProperty, value);
            }

            // Using a DependencyProperty as the backing store for ButtonId.  This enables animation, styling, binding, etc...
            public static readonly DependencyProperty ButtonIdProperty =
                DependencyProperty.Register(nameof(ButtonId), typeof(NumbersAndOperatorsEnum), typeof(FlipButtons), new PropertyMetadata(default(NumbersAndOperatorsEnum)));

            public Windows.UI.Xaml.Media.Brush HoverBackground
            {
                get => (Windows.UI.Xaml.Media.Brush)GetValue(HoverBackgroundProperty);
                set => SetValue(HoverBackgroundProperty, value);
            }

            // Using a DependencyProperty as the backing store for HoverBackground.  This enables animation, styling, binding, etc...
            public static readonly DependencyProperty HoverBackgroundProperty =
                DependencyProperty.Register(nameof(HoverBackground), typeof(Windows.UI.Xaml.Media.Brush), typeof(FlipButtons), new PropertyMetadata(default(Windows.UI.Xaml.Media.Brush)));

            public Windows.UI.Xaml.Media.Brush HoverForeground
            {
                get => (Windows.UI.Xaml.Media.Brush)GetValue(HoverForegroundProperty);
                set => SetValue(HoverForegroundProperty, value);
            }

            // Using a DependencyProperty as the backing store for HoverForeground.  This enables animation, styling, binding, etc...
            public static readonly DependencyProperty HoverForegroundProperty =
                DependencyProperty.Register(nameof(HoverForeground), typeof(Windows.UI.Xaml.Media.Brush), typeof(FlipButtons), new PropertyMetadata(default(Windows.UI.Xaml.Media.Brush)));

            public Windows.UI.Xaml.Media.Brush PressBackground
            {
                get => (Windows.UI.Xaml.Media.Brush)GetValue(PressBackgroundProperty);
                set => SetValue(PressBackgroundProperty, value);
            }

            // Using a DependencyProperty as the backing store for PressBackground.  This enables animation, styling, binding, etc...
            public static readonly DependencyProperty PressBackgroundProperty =
                DependencyProperty.Register(nameof(PressBackground), typeof(Windows.UI.Xaml.Media.Brush), typeof(FlipButtons), new PropertyMetadata(default(Windows.UI.Xaml.Media.Brush)));

            public Windows.UI.Xaml.Media.Brush PressForeground
            {
                get => (Windows.UI.Xaml.Media.Brush)GetValue(PressForegroundProperty);
                set => SetValue(PressForegroundProperty, value);
            }

            // Using a DependencyProperty as the backing store for PressForeground.  This enables animation, styling, binding, etc...
            public static readonly DependencyProperty PressForegroundProperty =
                DependencyProperty.Register(nameof(PressForeground), typeof(Windows.UI.Xaml.Media.Brush), typeof(FlipButtons), new PropertyMetadata(default(Windows.UI.Xaml.Media.Brush)));

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

            private void OnButtonIdPropertyChanged(NumbersAndOperatorsEnum oldValue, NumbersAndOperatorsEnum newValue)
            {
                CommandParameter = newValue;
            }
        }
    }
}
