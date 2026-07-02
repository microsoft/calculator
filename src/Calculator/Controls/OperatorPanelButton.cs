// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Windows.UI.Xaml;
using Windows.UI.Xaml.Automation;
using Windows.UI.Xaml.Automation.Peers;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Input;

namespace CalculatorApp
{
    namespace Controls
    {
        // OperatorPanelButton opens a flyout menu. It derives from Button (not ToggleButton) so
        // that it does not advertise the Toggle pattern, which would make Narrator announce a
        // misleading On/Off state for a menu opener. The "checked while the flyout is open" visual
        // is reproduced here by driving the template's visual states from IsChecked, and the peer
        // exposes the ExpandCollapse pattern instead of Toggle.
        public sealed class OperatorPanelButton : Windows.UI.Xaml.Controls.Button
        {
            private bool m_isPointerOver;
            private bool m_isPressed;

            public OperatorPanelButton()
            {
                IsEnabledChanged += OnIsEnabledChanged;
                Click += OnClicked;
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

            // True while the flyout is open. Replaces ToggleButton.IsChecked so the button no longer
            // exposes the Toggle pattern, while still driving the "open" visual and ExpandCollapse state.
            public bool IsChecked
            {
                get => (bool)GetValue(IsCheckedProperty);
                set => SetValue(IsCheckedProperty, value);
            }

            public static readonly DependencyProperty IsCheckedProperty =
                DependencyProperty.Register(nameof(IsChecked), typeof(bool), typeof(OperatorPanelButton), new PropertyMetadata(false, OnIsCheckedPropertyChanged));

            protected override void OnApplyTemplate()
            {
                base.OnApplyTemplate();

                if (FlyoutMenu != null)
                {
                    FlyoutMenu.Closed += FlyoutClosed;
                }

                UpdateVisualState(false);
            }

            private void OnClicked(object sender, RoutedEventArgs e)
            {
                IsChecked = !IsChecked;

                if (IsChecked)
                {
                    FlyoutMenu?.ShowAt(this);
                }
            }

            protected override void OnPointerEntered(PointerRoutedEventArgs e)
            {
                base.OnPointerEntered(e);
                m_isPointerOver = true;
                UpdateVisualState();
            }

            protected override void OnPointerExited(PointerRoutedEventArgs e)
            {
                base.OnPointerExited(e);
                m_isPointerOver = false;
                m_isPressed = false;
                UpdateVisualState();
            }

            protected override void OnPointerPressed(PointerRoutedEventArgs e)
            {
                base.OnPointerPressed(e);
                m_isPressed = true;
                UpdateVisualState();
            }

            protected override void OnPointerReleased(PointerRoutedEventArgs e)
            {
                base.OnPointerReleased(e);
                m_isPressed = false;
                UpdateVisualState();
            }

            protected override void OnPointerCanceled(PointerRoutedEventArgs e)
            {
                base.OnPointerCanceled(e);
                m_isPressed = false;
                m_isPointerOver = false;
                UpdateVisualState();
            }

            protected override void OnPointerCaptureLost(PointerRoutedEventArgs e)
            {
                base.OnPointerCaptureLost(e);
                m_isPressed = false;
                m_isPointerOver = false;
                UpdateVisualState();
            }

            protected override AutomationPeer OnCreateAutomationPeer()
            {
                return new OperatorPanelButtonAutomationPeer(this);
            }

            private void FlyoutClosed(object sender, object args)
            {
                IsChecked = false;
            }

            private void OnIsEnabledChanged(object sender, DependencyPropertyChangedEventArgs e)
            {
                UpdateVisualState();
            }

            private static void OnIsCheckedPropertyChanged(DependencyObject d, DependencyPropertyChangedEventArgs e)
            {
                ((OperatorPanelButton)d).OnIsCheckedChanged((bool)e.NewValue);
            }

            private void OnIsCheckedChanged(bool isChecked)
            {
                UpdateVisualState();

                if (FrameworkElementAutomationPeer.FromElement(this) is OperatorPanelButtonAutomationPeer peer)
                {
                    peer.RaiseExpandCollapseAutomationEvent(
                        isChecked ? ExpandCollapseState.Collapsed : ExpandCollapseState.Expanded,
                        isChecked ? ExpandCollapseState.Expanded : ExpandCollapseState.Collapsed);
                }
            }

            // Mirrors ToggleButton's own visual-state selection so the existing template (with its
            // Checked/CheckedPointerOver/CheckedPressed/CheckedDisabled states) keeps working now
            // that the base control is a Button rather than a ToggleButton.
            private void UpdateVisualState(bool useTransitions = true)
            {
                string state;
                if (!IsEnabled)
                {
                    state = IsChecked ? "CheckedDisabled" : "Disabled";
                }
                else if (m_isPressed)
                {
                    state = IsChecked ? "CheckedPressed" : "Pressed";
                }
                else if (m_isPointerOver)
                {
                    state = IsChecked ? "CheckedPointerOver" : "PointerOver";
                }
                else
                {
                    state = IsChecked ? "Checked" : "Normal";
                }

                VisualStateManager.GoToState(this, state, useTransitions);
            }
        }
    }
}
