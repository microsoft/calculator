// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using Windows.Devices.Input;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Input;

namespace CalculatorApp
{
    namespace Controls
    {
        public sealed class OperatorPanelListView : Windows.UI.Xaml.Controls.ListView
        {
            public OperatorPanelListView()
            {
            }

            protected override void OnApplyTemplate()
            {
                m_scrollViewer = GetTemplateChild("ScrollViewer") as ScrollViewer;
                m_scrollLeft = GetTemplateChild("ScrollLeft") as Button;
                m_scrollRight = GetTemplateChild("ScrollRight") as Button;
                m_content = GetTemplateChild("Content") as ItemsPresenter;

                if (m_scrollLeft != null)
                {
                    m_scrollLeft.Click += OnScrollClick;
                    m_scrollLeft.PointerExited += OnButtonPointerExited;
                }

                if (m_scrollRight != null)
                {
                    m_scrollRight.Click += OnScrollClick;
                    m_scrollRight.PointerExited += OnButtonPointerExited;
                }

                if (m_scrollViewer != null)
                {
                    m_scrollViewer.ViewChanged += ScrollViewChanged;
                }

                this.PointerEntered += OnPointerEntered;
                this.PointerExited += OnPointerExited;

                base.OnApplyTemplate();
            }

            private void OnScrollClick(object sender, RoutedEventArgs e)
            {
                var clicked = sender as Button;
                if (clicked == m_scrollLeft)
                {
                    ScrollLeft();
                }
                else
                {
                    ScrollRight();
                }
            }

            private void OnPointerEntered(object sender, PointerRoutedEventArgs e)
            {
                if (e.Pointer.PointerDeviceType == PointerDeviceType.Mouse)
                {
                    UpdateScrollButtons();
                    m_isPointerEntered = true;
                }
            }

            private void OnPointerExited(object sender, PointerRoutedEventArgs e)
            {
                m_scrollLeft.Visibility = Visibility.Collapsed;
                m_scrollRight.Visibility = Visibility.Collapsed;
                m_isPointerEntered = false;
            }

            private void OnButtonPointerExited(object sender, PointerRoutedEventArgs e)
            {
                var button = sender as Button;

                // Do not bubble up the pointer exit event to the control if the button being exited was not visible
                if (button.Visibility == Visibility.Collapsed)
                {
                    e.Handled = true;
                }
            }

            private void ScrollViewChanged(object sender, ScrollViewerViewChangedEventArgs e)
            {
                if (m_isPointerEntered && !e.IsIntermediate)
                {
                    UpdateScrollButtons();
                }
            }

            private void ShowHideScrollButtons(Visibility vLeft, Visibility vRight)
            {
                if (m_scrollLeft != null && m_scrollRight != null)
                {
                    m_scrollLeft.Visibility = vLeft;
                    m_scrollRight.Visibility = vRight;
                }
            }

            private void UpdateScrollButtons()
            {
                if (m_content == null || m_scrollViewer == null)
                {
                    return;
                }

                // When the width is smaller than the container, don't show any
                if (m_content.ActualWidth <= m_scrollViewer.ActualWidth)
                {
                    ShowHideScrollButtons(Visibility.Collapsed, Visibility.Collapsed);
                }
                // We have more number on both side. Show both arrows
                else if ((m_scrollViewer.HorizontalOffset > 0) && (m_scrollViewer.HorizontalOffset < (m_scrollViewer.ExtentWidth - m_scrollViewer.ViewportWidth)))
                {
                    ShowHideScrollButtons(Visibility.Visible, Visibility.Visible);
                }
                // Width is larger than the container and left most part of the number is shown. Should be able to scroll left.
                else if (m_scrollViewer.HorizontalOffset == 0)
                {
                    ShowHideScrollButtons(Visibility.Collapsed, Visibility.Visible);
                }
                else // Width is larger than the container and right most part of the number is shown. Should be able to scroll left.
                {
                    ShowHideScrollButtons(Visibility.Visible, Visibility.Collapsed);
                }
            }

            private void ScrollLeft()
            {
                double offset = m_scrollViewer.HorizontalOffset - (scrollRatio * m_scrollViewer.ViewportWidth);
                m_scrollViewer.ChangeView(offset, null, null);
            }

            private void ScrollRight()
            {
                double offset = m_scrollViewer.HorizontalOffset + (scrollRatio * m_scrollViewer.ViewportWidth);
                m_scrollViewer.ChangeView(offset, null, null);
            }

            private readonly double scrollRatio = 0.7;

            private bool m_isPointerEntered;

            private Windows.UI.Xaml.Controls.ItemsPresenter m_content;
            private Windows.UI.Xaml.Controls.ScrollViewer m_scrollViewer;
            private Windows.UI.Xaml.Controls.Button m_scrollLeft;
            private Windows.UI.Xaml.Controls.Button m_scrollRight;
        }
    }
}
