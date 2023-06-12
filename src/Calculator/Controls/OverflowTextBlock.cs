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
        public enum OverflowButtonPlacement
        {
            InLine,
            Above
        };

        public sealed class OverflowTextBlock : Windows.UI.Xaml.Controls.Control
        {
            public OverflowTextBlock()
            {
                m_isAccessibilityViewControl = false;
                m_expressionContent = null;
                m_itemsControl = null;
                m_expressionContainer = null;
                m_scrollLeft = null;
                m_scrollRight = null;
            }

            public bool TokensUpdated
            {
                get => (bool)GetValue(TokensUpdatedProperty);
                set => SetValue(TokensUpdatedProperty, value);
            }

            // Using a DependencyProperty as the backing store for TokensUpdated.  This enables animation, styling, binding, etc...
            public static readonly DependencyProperty TokensUpdatedProperty =
                DependencyProperty.Register(nameof(TokensUpdated), typeof(bool), typeof(OverflowTextBlock), new PropertyMetadata(default(bool), (sender, args) =>
                {
                    var self = (OverflowTextBlock)sender;
                    self.OnTokensUpdatedPropertyChanged((bool)args.OldValue, (bool)args.NewValue);
                }));

            public OverflowButtonPlacement ScrollButtonsPlacement
            {
                get => (OverflowButtonPlacement)GetValue(ScrollButtonsPlacementProperty);
                set => SetValue(ScrollButtonsPlacementProperty, value);
            }

            // Using a DependencyProperty as the backing store for ScrollButtonsPlacement.  This enables animation, styling, binding, etc...
            public static readonly DependencyProperty ScrollButtonsPlacementProperty =
                DependencyProperty.Register(nameof(ScrollButtonsPlacement), typeof(OverflowButtonPlacement), typeof(OverflowTextBlock), new PropertyMetadata(default(OverflowButtonPlacement), (sender, args) =>
                {
                    var self = (OverflowTextBlock)sender;
                    self.OnScrollButtonsPlacementPropertyChanged((OverflowButtonPlacement)args.OldValue, (OverflowButtonPlacement)args.NewValue);
                }));

            public bool IsActive
            {
                get => (bool)GetValue(IsActiveProperty);
                set => SetValue(IsActiveProperty, value);
            }

            // Using a DependencyProperty as the backing store for IsActive.  This enables animation, styling, binding, etc...
            public static readonly DependencyProperty IsActiveProperty =
                DependencyProperty.Register(nameof(IsActive), typeof(bool), typeof(OverflowTextBlock), new PropertyMetadata(default(bool)));

            public Windows.UI.Xaml.Style TextStyle
            {
                get => (Style)GetValue(TextStyleProperty);
                set => SetValue(TextStyleProperty, value);
            }

            // Using a DependencyProperty as the backing store for TextStyle.  This enables animation, styling, binding, etc...
            public static readonly DependencyProperty TextStyleProperty =
                DependencyProperty.Register(nameof(TextStyle), typeof(Style), typeof(OverflowTextBlock), new PropertyMetadata(default(Style)));

            public double ScrollButtonsWidth
            {
                get => (double)GetValue(ScrollButtonsWidthProperty);
                set => SetValue(ScrollButtonsWidthProperty, value);
            }

            // Using a DependencyProperty as the backing store for ScrollButtonsWidth.  This enables animation, styling, binding, etc...
            public static readonly DependencyProperty ScrollButtonsWidthProperty =
                DependencyProperty.Register(nameof(ScrollButtonsWidth), typeof(double), typeof(OverflowTextBlock), new PropertyMetadata(default(double)));

            public double ScrollButtonsFontSize
            {
                get => (double)GetValue(ScrollButtonsFontSizeProperty);
                set => SetValue(ScrollButtonsFontSizeProperty, value);
            }

            // Using a DependencyProperty as the backing store for ScrollButtonsFontSize.  This enables animation, styling, binding, etc...
            public static readonly DependencyProperty ScrollButtonsFontSizeProperty =
                DependencyProperty.Register(nameof(ScrollButtonsFontSize), typeof(double), typeof(OverflowTextBlock), new PropertyMetadata(default(double)));

            public void OnTokensUpdatedPropertyChanged(bool oldValue, bool newValue)
            {
                if (m_expressionContainer != null && newValue)
                {
                    m_expressionContainer.UpdateLayout();
                    m_expressionContainer.ChangeView(m_expressionContainer.ScrollableWidth, null, null, true);
                }
                var newIsAccessibilityViewControl = m_itemsControl != null && m_itemsControl.Items.Count > 0;
                if (m_isAccessibilityViewControl != newIsAccessibilityViewControl)
                {
                    m_isAccessibilityViewControl = newIsAccessibilityViewControl;
                    AutomationProperties.SetAccessibilityView(this, newIsAccessibilityViewControl ? AccessibilityView.Control : AccessibilityView.Raw);
                }
                UpdateScrollButtons();
            }

            public void OnScrollButtonsPlacementPropertyChanged(OverflowButtonPlacement oldValue, OverflowButtonPlacement newValue)
            {
                if (newValue == OverflowButtonPlacement.InLine)
                {
                    m_expressionContainer.Padding = new Thickness(0);
                    m_expressionContent.Margin = new Thickness(0);
                }
                UpdateScrollButtons();
            }

            public void UpdateScrollButtons()
            {
                if (m_expressionContent == null || m_expressionContainer == null || m_scrollLeft == null || m_scrollRight == null)
                {
                    return;
                }

                var realOffset = m_expressionContainer.HorizontalOffset + m_expressionContainer.Padding.Left + m_expressionContent.Margin.Left;
                var scrollLeftVisibility = realOffset > SCROLL_BUTTONS_APPROXIMATION_RANGE ? Visibility.Visible : Visibility.Collapsed;
                var scrollRightVisibility = realOffset + m_expressionContainer.ActualWidth + SCROLL_BUTTONS_APPROXIMATION_RANGE < m_expressionContent.ActualWidth
                                                 ? Visibility.Visible
                                                 : Visibility.Collapsed;

                bool shouldTryFocusScrollRight = false;
                if (m_scrollLeft.Visibility != scrollLeftVisibility)
                {
                    if (scrollLeftVisibility == Visibility.Collapsed)
                    {
                        shouldTryFocusScrollRight = m_scrollLeft.Equals(FocusManager.GetFocusedElement());
                    }

                    m_scrollLeft.Visibility = scrollLeftVisibility;
                }

                if (m_scrollRight.Visibility != scrollRightVisibility)
                {
                    if (scrollRightVisibility == Visibility.Collapsed && m_scrollLeft.Visibility == Visibility.Visible
                        && m_scrollRight.Equals(FocusManager.GetFocusedElement()))
                    {
                        m_scrollLeft.Focus(FocusState.Programmatic);
                    }
                    m_scrollRight.Visibility = scrollRightVisibility;
                }

                if (shouldTryFocusScrollRight && scrollRightVisibility == Visibility.Visible)
                {
                    m_scrollRight.Focus(FocusState.Programmatic);
                }

                if (ScrollButtonsPlacement == OverflowButtonPlacement.Above && m_expressionContent != null)
                {
                    double left = m_scrollLeft != null && m_scrollLeft.Visibility == Visibility.Visible ? ScrollButtonsWidth : 0;
                    double right = m_scrollRight != null && m_scrollRight.Visibility == Visibility.Visible ? ScrollButtonsWidth : 0;
                    if (m_expressionContainer.Padding.Left != left || m_expressionContainer.Padding.Right != right)
                    {
                        m_expressionContainer.ViewChanged -= OnViewChanged;

                        m_expressionContainer.Padding = new Thickness(left, 0, right, 0);
                        m_expressionContent.Margin = new Thickness(-left, 0, -right, 0);
                        m_expressionContainer.UpdateLayout();
                        m_expressionContainer.Measure(m_expressionContainer.RenderSize);

                        m_expressionContainer.ViewChanged += OnViewChanged;
                    }
                }
            }

            public void UnregisterEventHandlers()
            {
                // Unregister the event handlers
                if (m_scrollLeft != null)
                {
                    m_scrollLeft.Click -= OnScrollLeftClick;
                }

                if (m_scrollRight != null)
                {
                    m_scrollRight.Click -= OnScrollRightClick;
                }

                if (m_expressionContainer != null)
                {
                    m_expressionContainer.ViewChanged -= OnViewChanged;
                }
            }

            protected override void OnApplyTemplate()
            {
                UnregisterEventHandlers();

                var uiElement = GetTemplateChild("ExpressionContainer");
                if (uiElement != null)
                {
                    m_expressionContainer = (ScrollViewer)uiElement;
                    m_expressionContainer.ChangeView(m_expressionContainer.ExtentWidth - m_expressionContainer.ViewportWidth, null, null);
                    m_expressionContainer.ViewChanged += OnViewChanged;
                }

                uiElement = GetTemplateChild("ExpressionContent");
                if (uiElement != null)
                {
                    m_expressionContent = (FrameworkElement)uiElement;
                }

                uiElement = GetTemplateChild("ScrollLeft");
                if (uiElement != null)
                {
                    m_scrollLeft = (Button)uiElement;
                    m_scrollLeft.Click += OnScrollLeftClick;
                }

                uiElement = GetTemplateChild("ScrollRight");
                if (uiElement != null)
                {
                    m_scrollRight = (Button)uiElement;
                    m_scrollRight.Click += OnScrollRightClick;
                }

                uiElement = GetTemplateChild("TokenList");
                if (uiElement != null)
                {
                    m_itemsControl = (ItemsControl)uiElement;
                }

                UpdateAllState();

            }

            protected override AutomationPeer OnCreateAutomationPeer()
            {
                return new OverflowTextBlockAutomationPeer(this);
            }

            private void OnScrollLeftClick(object sender, RoutedEventArgs e)
            {
                ScrollLeft();
            }

            private void OnScrollRightClick(object sender, RoutedEventArgs e)
            {
                ScrollRight();
            }

            private void OnViewChanged(object sender, ScrollViewerViewChangedEventArgs args)
            {
                UpdateScrollButtons();
            }

            private void UpdateVisualState()
            {
                VisualStateManager.GoToState(this, IsActive ? "Active" : "Normal", true);
            }

            private void UpdateAllState()
            {
                UpdateVisualState();
            }

            private void ScrollLeft()
            {
                if (m_expressionContainer != null && m_expressionContainer.HorizontalOffset > 0)
                {
                    double offset = m_expressionContainer.HorizontalOffset - (SCROLL_RATIO * m_expressionContainer.ViewportWidth);
                    m_expressionContainer.ChangeView(offset, null, null);
                    m_expressionContainer.UpdateLayout();
                    UpdateScrollButtons();
                }
            }

            private void ScrollRight()
            {
                if (m_expressionContainer != null && m_expressionContent != null)
                {
                    var realOffset = m_expressionContainer.HorizontalOffset + m_expressionContainer.Padding.Left + m_expressionContent.Margin.Left;
                    if (realOffset + m_expressionContainer.ActualWidth < m_expressionContent.ActualWidth)
                    {
                        double offset = m_expressionContainer.HorizontalOffset + (SCROLL_RATIO * m_expressionContainer.ViewportWidth);
                        m_expressionContainer.ChangeView(offset, null, null);
                        m_expressionContainer.UpdateLayout();
                        UpdateScrollButtons();
                    }
                }
            }

            private const uint SCROLL_BUTTONS_APPROXIMATION_RANGE = 4;
            private const double SCROLL_RATIO = 0.7;

            private bool m_isAccessibilityViewControl;
            private Windows.UI.Xaml.FrameworkElement m_expressionContent;
            private Windows.UI.Xaml.Controls.ItemsControl m_itemsControl;
            private Windows.UI.Xaml.Controls.ScrollViewer m_expressionContainer;
            private Windows.UI.Xaml.Controls.Button m_scrollLeft;
            private Windows.UI.Xaml.Controls.Button m_scrollRight;
        }

    }
}
