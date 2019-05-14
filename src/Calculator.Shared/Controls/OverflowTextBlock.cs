// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Collections.Generic;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Text;
using Windows.Devices.Input;
using Windows.System;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Automation;
using Windows.UI.Xaml.Automation.Peers;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;

namespace CalculatorApp.Controls
{
    public partial class OverflowTextBlock : Control
    {
        const double scrollRatio = 0.7;

        bool m_scrollingLeft;
        bool m_scrollingRight;

        ListView m_listView;
        ScrollViewer m_expressionContainer;
        Button m_scrollLeft;
        Button m_scrollRight;

        EventRegistrationToken m_scrollLeftClickEventToken;
        EventRegistrationToken m_scrollRightClickEventToken;
        EventRegistrationToken m_pointerEnteredEventToken;
        EventRegistrationToken m_pointerExitedEventToken;
        private ItemsControl m_itemsControl;
        private bool m_isAccessibilityViewControl;



        public bool TokensUpdated
        {
            get { return (bool)GetValue(TokensUpdatedProperty); }
            set { SetValue(TokensUpdatedProperty, value); }
        }

        public static readonly DependencyProperty TokensUpdatedProperty =
            DependencyProperty.Register("TokensUpdated", typeof(bool), typeof(OverflowTextBlock), new PropertyMetadata(false));



        public bool IsActive
        {
            get { return (bool)GetValue(IsActiveProperty); }
            set { SetValue(IsActiveProperty, value); }
        }

        public static readonly DependencyProperty IsActiveProperty =
            DependencyProperty.Register("IsActive", typeof(bool), typeof(OverflowTextBlock), new PropertyMetadata(false));



        public Style TextStyle
        {
            get { return (Style)GetValue(TextStyleProperty); }
            set { SetValue(TextStyleProperty, value); }
        }

        public static readonly DependencyProperty TextStyleProperty =
            DependencyProperty.Register("TextStyle", typeof(Style), typeof(OverflowTextBlock), new PropertyMetadata(null));


        protected override void OnApplyTemplate()
        {
            UnregisterEventHandlers();

            var uiElement = GetTemplateChild("ExpressionContainer");
            if (uiElement != null)
            {
                m_expressionContainer = uiElement as ScrollViewer;
                m_expressionContainer.ChangeView(m_expressionContainer.ExtentWidth - m_expressionContainer.ViewportWidth, null, null);
                m_expressionContainer.ViewChanged += OnViewChanged;
            }

            uiElement = GetTemplateChild("ScrollLeft");
            if (uiElement != null)
            {
                m_scrollLeft = (uiElement as Button);
                m_scrollLeft.Click += OnScrollClick;
            }

            uiElement = GetTemplateChild("ScrollRight");
            if (uiElement != null)
            {
                m_scrollRight = (uiElement as Button);
                m_scrollRight.Click += OnScrollClick;
            }

            m_scrollingLeft = false;
            m_scrollingRight = false;

            uiElement = GetTemplateChild("TokenList");
            if (uiElement != null)
            {
                m_itemsControl = (uiElement as ItemsControl);
            }

            UpdateAllState();
        }

        AutomationPeer OnCreateAutomationPeer()
        {
            return new OverflowTextBlockAutomationPeer(this);
        }

        void OnTokensUpdatedPropertyChanged(bool oldValue, bool newValue)
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

        void UpdateAllState()
        {
            UpdateVisualState();
        }

        void UpdateVisualState()
        {
            if (IsActive)
            {
                VisualStateManager.GoToState(this, "Active", true);
            }
            else
            {
                VisualStateManager.GoToState(this, "Normal", true);
            }
        }

        void ScrollLeft()
        {
            if (m_expressionContainer != null && m_expressionContainer.HorizontalOffset > 0)
            {
                m_scrollingLeft = true;
                double offset = m_expressionContainer.HorizontalOffset - (scrollRatio * m_expressionContainer.ViewportWidth);
                m_expressionContainer.ChangeView(offset, null, null);
                m_expressionContainer.UpdateLayout();
                UpdateScrollButtons();
            }
        }

        void ScrollRight()
        {
            if (m_expressionContainer != null && m_expressionContainer.HorizontalOffset < m_expressionContainer.ExtentWidth - m_expressionContainer.ViewportWidth)
            {
                m_scrollingRight = true;
                double offset = m_expressionContainer.HorizontalOffset + (scrollRatio * m_expressionContainer.ViewportWidth);
                m_expressionContainer.ChangeView(offset, null, null);
                m_expressionContainer.UpdateLayout();
                UpdateScrollButtons();
            }
        }

        void OnScrollClick(object sender, RoutedEventArgs args)
        {
            var clicked = (sender as Button);
            if (clicked == m_scrollLeft)
            {
                ScrollLeft();
            }
            else
            {
                ScrollRight();
            }
        }

        void UpdateScrollButtons()
        {
            if (m_itemsControl == null || m_expressionContainer == null)
            {
                return;
            }

            // When the width is smaller than the container, don't show any
            if (m_itemsControl.ActualWidth <= m_expressionContainer.ActualWidth)
            {
                ShowHideScrollButtons(Visibility.Collapsed, Visibility.Collapsed);
            }
            // We have more number on both side. Show both arrows
            else if (
                (m_expressionContainer.HorizontalOffset > 0)
                && (m_expressionContainer.HorizontalOffset < (m_expressionContainer.ExtentWidth - m_expressionContainer.ViewportWidth)))
            {
                ShowHideScrollButtons(Visibility.Visible, Visibility.Visible);
            }
            // Width is larger than the container and left most part of the number is shown. Should be able to scroll left.
            else if (m_expressionContainer.HorizontalOffset == 0)
            {
                ShowHideScrollButtons(Visibility.Collapsed, Visibility.Visible);
                if (m_scrollingLeft)
                {
                    m_scrollingLeft = false;
                    if (m_scrollRight != null)
                    {
                        m_scrollRight.Focus(FocusState.Programmatic);
                    }
                }
            }
            else // Width is larger than the container and right most part of the number is shown. Should be able to scroll left.
            {
                ShowHideScrollButtons(Visibility.Visible, Visibility.Collapsed);
                if (m_scrollingRight)
                {
                    m_scrollingRight = false;
                    if (m_scrollLeft != null)
                    {
                        m_scrollLeft.Focus(FocusState.Programmatic);
                    }
                }
            }
        }

        void ShowHideScrollButtons(Visibility vLeft, Visibility vRight)
        {
            if (m_scrollLeft != null && m_scrollRight != null)
            {
                m_scrollLeft.Visibility = vLeft;
                m_scrollRight.Visibility = vRight;
            }
        }

        public void UnregisterEventHandlers()
        {
            // UNO TODO Unregister

            // Unregister the event handlers
            //if (m_scrollLeft != null)
            //{
            //    m_scrollLeft.Click -= m_scrollLeftClickEventToken;
            //}

            //if (m_scrollRight != null)
            //{
            //    m_scrollRight.Click -= m_scrollRightClickEventToken;
            //}

            //if (m_expressionContainer != null)
            //{
            //    m_expressionContainer.ViewChanged -= m_containerViewChangedToken;
            //}
        }

        void OnViewChanged(object sender, ScrollViewerViewChangedEventArgs args)
        {
            UpdateScrollButtons();
        }

    }
}
