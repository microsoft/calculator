// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using CalculatorApp.ViewModel.Common;

using System;
using System.Diagnostics;

using Windows.UI.Xaml;
using Windows.UI.Xaml.Automation.Peers;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Input;

namespace CalculatorApp
{
    namespace Controls
    {
        public delegate void SelectedEventHandler(object sender);

        public sealed class CalculationResult : Windows.UI.Xaml.Controls.Control
        {
            public CalculationResult()
            {
                m_isScalingText = false;
                m_haveCalculatedMax = false;
            }

            public double MinFontSize
            {
                get => (double)GetValue(MinFontSizeProperty);
                set => SetValue(MinFontSizeProperty, value);
            }

            // Using a DependencyProperty as the backing store for MinFontSize.  This enables animation, styling, binding, etc...
            public static readonly DependencyProperty MinFontSizeProperty =
                DependencyProperty.Register(nameof(MinFontSize), typeof(double), typeof(CalculationResult), new PropertyMetadata(0.0, (sender, args) =>
                {
                    var self = (CalculationResult)sender;
                    self.OnMinFontSizePropertyChanged((double)args.OldValue, (double)args.NewValue);
                }));

            public double MaxFontSize
            {
                get => (double)GetValue(MaxFontSizeProperty);
                set => SetValue(MaxFontSizeProperty, value);
            }

            // Using a DependencyProperty as the backing store for MaxFontSize.  This enables animation, styling, binding, etc...
            public static readonly DependencyProperty MaxFontSizeProperty =
                DependencyProperty.Register(nameof(MaxFontSize), typeof(double), typeof(CalculationResult), new PropertyMetadata(30.0, (sender, args) =>
                {
                    var self = (CalculationResult)sender;
                    self.OnMaxFontSizePropertyChanged((double)args.OldValue, (double)args.NewValue);
                }));

            public Thickness DisplayMargin
            {
                get => (Thickness)GetValue(DisplayMarginProperty);
                set => SetValue(DisplayMarginProperty, value);
            }

            // Using a DependencyProperty as the backing store for DisplayMargin.  This enables animation, styling, binding, etc...
            public static readonly DependencyProperty DisplayMarginProperty =
                DependencyProperty.Register(nameof(DisplayMargin), typeof(Thickness), typeof(CalculationResult), new PropertyMetadata(default(Thickness)));

            public bool IsActive
            {
                get => (bool)GetValue(IsActiveProperty);
                set => SetValue(IsActiveProperty, value);
            }

            // Using a DependencyProperty as the backing store for IsActive.  This enables animation, styling, binding, etc...
            public static readonly DependencyProperty IsActiveProperty =
                DependencyProperty.Register(nameof(IsActive), typeof(bool), typeof(CalculationResult), new PropertyMetadata(default(bool), (sender, args) =>
                {
                    var self = (CalculationResult)sender;
                    self.OnIsActivePropertyChanged((bool)args.OldValue, (bool)args.NewValue);
                }));

            public string DisplayValue
            {
                get => (string)GetValue(DisplayValueProperty);
                set => SetValue(DisplayValueProperty, value);
            }

            // Using a DependencyProperty as the backing store for DisplayValue.  This enables animation, styling, binding, etc...
            public static readonly DependencyProperty DisplayValueProperty =
                DependencyProperty.Register(nameof(DisplayValue), typeof(string), typeof(CalculationResult), new PropertyMetadata(string.Empty, (sender, args) =>
                {
                    var self = (CalculationResult)sender;
                    self.OnDisplayValuePropertyChanged((string)args.OldValue, (string)args.NewValue);
                }));

            public bool IsInError
            {
                get => (bool)GetValue(IsInErrorProperty);
                set => SetValue(IsInErrorProperty, value);
            }

            // Using a DependencyProperty as the backing store for IsInError.  This enables animation, styling, binding, etc...
            public static readonly DependencyProperty IsInErrorProperty =
                DependencyProperty.Register(nameof(IsInError), typeof(bool), typeof(CalculationResult), new PropertyMetadata(default(bool), (sender, args) =>
                {
                    var self = (CalculationResult)sender;
                    self.OnIsInErrorPropertyChanged((bool)args.OldValue, (bool)args.NewValue);
                }));

            public bool IsOperatorCommand
            {
                get => (bool)GetValue(IsOperatorCommandProperty);
                set => SetValue(IsOperatorCommandProperty, value);
            }

            // Using a DependencyProperty as the backing store for IsOperatorCommand.  This enables animation, styling, binding, etc...
            public static readonly DependencyProperty IsOperatorCommandProperty =
                DependencyProperty.Register(nameof(IsOperatorCommand), typeof(bool), typeof(CalculationResult), new PropertyMetadata(false));

            public event SelectedEventHandler Selected;

            public void ProgrammaticSelect()
            {
                RaiseSelectedEvent();
            }

            internal void UpdateTextState()
            {
                if ((m_textContainer == null) || (m_textBlock == null))
                {
                    return;
                }

                var containerSize = m_textContainer.ActualWidth;
                string oldText = m_textBlock.Text;
                string newText = DisplayValue;

                // Initiate the scaling operation
                // UpdateLayout will keep calling us until we make it through the below 2 if-statements
                if (!m_isScalingText || oldText != newText)
                {
                    m_textBlock.Text = newText;

                    m_isScalingText = true;
                    m_haveCalculatedMax = false;
                    m_textBlock.InvalidateArrange();
                    return;
                }
                if (containerSize > 0)
                {
                    double widthDiff = Math.Abs(m_textBlock.ActualWidth - containerSize);
                    double fontSizeChange = INCREMENTOFFSET;

                    if (widthDiff > WIDTHCUTOFF)
                    {
                        fontSizeChange = Math.Min(Math.Max(Math.Floor(WIDTHTOFONTSCALAR * widthDiff) - WIDTHTOFONTOFFSET, INCREMENTOFFSET), MAXFONTINCREMENT);
                    }
                    if (m_textBlock.ActualWidth < containerSize && Math.Abs(m_textBlock.FontSize - MaxFontSize) > FONTTOLERANCE && !m_haveCalculatedMax)
                    {
                        ModifyFontAndMargin(m_textBlock, fontSizeChange);
                        m_textBlock.InvalidateArrange();
                        return;
                    }
                    if (fontSizeChange < 5)
                    {
                        m_haveCalculatedMax = true;
                    }
                    if (m_textBlock.ActualWidth >= containerSize && Math.Abs(m_textBlock.FontSize - MinFontSize) > FONTTOLERANCE)
                    {
                        ModifyFontAndMargin(m_textBlock, -1 * fontSizeChange);
                        m_textBlock.InvalidateArrange();
                        return;
                    }
                    Debug.Assert(m_textBlock.FontSize >= MinFontSize && m_textBlock.FontSize <= MaxFontSize);
                    m_isScalingText = false;
                    if (IsOperatorCommand)
                    {
                        m_textContainer.ChangeView(0.0, null, null);
                    }
                    else
                    {
                        m_textContainer.ChangeView(m_textContainer.ExtentWidth - m_textContainer.ViewportWidth, null, null);
                    }
                }
            }
            public string GetRawDisplayValue()
            {
                return LocalizationSettings.GetInstance().RemoveGroupSeparators(DisplayValue);
            }

            protected override void OnKeyDown(KeyRoutedEventArgs e)
            {
                switch (e.Key)
                {
                    case Windows.System.VirtualKey.Left:
                        this.ScrollLeft();
                        break;
                    case Windows.System.VirtualKey.Right:
                        this.ScrollRight();
                        break;
                }
            }

            protected override void OnApplyTemplate()
            {
                if (m_textContainer != null)
                {
                    m_textContainer.LayoutUpdated -= OnTextContainerLayoutUpdated;
                    m_textContainer.SizeChanged -= OnTextContainerSizeChanged;
                    m_textContainer.ViewChanged -= OnTextContainerOnViewChanged;
                }

                if (m_textBlock != null)
                {
                    m_textBlock.SizeChanged -= OnTextBlockSizeChanged;
                }

                if (m_scrollLeft != null)
                {
                    m_scrollLeft.Click -= OnScrollLeftClick;
                }

                if (m_scrollRight != null)
                {
                    m_scrollRight.Click -= OnScrollRightClick;
                }

                m_textContainer = GetTemplateChild("TextContainer") as ScrollViewer;
                if (m_textContainer != null)
                {
                    // We want to know when the size of the container changes so
                    // we can rescale the textbox
                    m_textContainer.SizeChanged += OnTextContainerSizeChanged;

                    m_textContainer.ViewChanged += OnTextContainerOnViewChanged;

                    m_textContainer.LayoutUpdated += OnTextContainerLayoutUpdated;

                    m_textContainer.ChangeView(m_textContainer.ExtentWidth - m_textContainer.ViewportWidth, null, null);
                    m_scrollLeft = GetTemplateChild("ScrollLeft") as HyperlinkButton;
                    if (m_scrollLeft != null)
                    {
                        m_scrollLeft.Click += OnScrollLeftClick;
                    }
                    m_scrollRight = GetTemplateChild("ScrollRight") as HyperlinkButton;
                    if (m_scrollRight != null)
                    {
                        m_scrollRight.Click += OnScrollRightClick;
                    }
                    m_textBlock = GetTemplateChild("NormalOutput") as TextBlock;
                    if (m_textBlock != null)
                    {
                        m_textBlock.Visibility = Visibility.Visible;
                        m_textBlock.SizeChanged += OnTextBlockSizeChanged;
                    }
                }
                UpdateVisualState();
                UpdateTextState();
            }

            protected override void OnTapped(TappedRoutedEventArgs e)
            {
                this.Focus(FocusState.Programmatic);
                RaiseSelectedEvent();
            }

            protected override void OnRightTapped(RightTappedRoutedEventArgs e)
            {
                var requestedElement = e.OriginalSource;

                if (requestedElement.Equals(m_textBlock))
                {
                    m_textBlock.Focus(FocusState.Programmatic);
                }
                else
                {
                    this.Focus(FocusState.Programmatic);
                }
            }

            protected override AutomationPeer OnCreateAutomationPeer()
            {
                return new CalculationResultAutomationPeer(this);
            }

            private void OnIsActivePropertyChanged(bool oldValue, bool newValue)
            {
                UpdateVisualState();
            }

            private void OnDisplayValuePropertyChanged(string oldValue, string newValue)
            {
                UpdateTextState();
            }

            private void OnIsInErrorPropertyChanged(bool oldValue, bool newValue)
            {
                // We need to have a good template for this to work
                if (m_textBlock == null)
                {
                    return;
                }

                if (newValue)
                {
                    // If there's an error message we need to override the normal display font
                    // with the font appropriate for this language. This is because the error
                    // message is localized and therefore can contain characters that are not
                    // available in the normal font.
                    // We use UIText as the font type because this is the most common font type to use
                    m_textBlock.FontFamily = LocalizationService.GetInstance().GetLanguageFontFamilyForType(LanguageFontType.UIText);
                }
                else
                {
                    // The error result is no longer an error so we will restore the
                    // value to FontFamily property to the value provided in the style
                    // for the TextBlock in the template.
                    m_textBlock.ClearValue(TextBlock.FontFamilyProperty);
                }
            }

            private void OnMinFontSizePropertyChanged(double oldValue, double newValue)
            {
                UpdateTextState();
            }

            private void OnMaxFontSizePropertyChanged(double oldValue, double newValue)
            {
                UpdateTextState();
            }

            private void OnTextContainerSizeChanged(object sender, SizeChangedEventArgs e)
            {
                UpdateTextState();
            }

            private void OnTextBlockSizeChanged(object sender, SizeChangedEventArgs e)
            {
                UpdateScrollButtons();
            }

            private void OnTextContainerLayoutUpdated(object sender, object e)
            {
                if (m_isScalingText)
                {
                    UpdateTextState();
                }
            }

            private void OnTextContainerOnViewChanged(object sender, ScrollViewerViewChangedEventArgs e)
            {
                UpdateScrollButtons();
            }

            private void UpdateVisualState()
            {
                VisualStateManager.GoToState(this, IsActive ? "Active" : "Normal", true);
            }

            private void OnScrollLeftClick(object sender, RoutedEventArgs e)
            {
                ScrollLeft();
            }

            private void OnScrollRightClick(object sender, RoutedEventArgs e)
            {
                ScrollRight();
            }

            private void ModifyFontAndMargin(TextBlock textBox, double fontChange)
            {
                double cur = textBox.FontSize;
                double scaleFactor = SCALEFACTOR;
                if (m_textContainer.ActualHeight <= HEIGHTCUTOFF)
                {
                    scaleFactor = SMALLHEIGHTSCALEFACTOR;
                }

                double newFontSize = Math.Min(Math.Max(cur + fontChange, MinFontSize), MaxFontSize);
                m_textContainer.Padding = new Thickness(0, 0, 0, scaleFactor * Math.Abs(cur - newFontSize));
                textBox.FontSize = newFontSize;
            }

            private void UpdateScrollButtons()
            {
                if (m_textContainer == null)
                {
                    return;
                }

                bool shouldTryFocusScrollRight = false;
                if (m_scrollLeft != null)
                {
                    var scrollLeftVisibility = m_textContainer.HorizontalOffset > SCROLL_BUTTONS_APPROXIMATION_RANGE ? Visibility.Visible : Visibility.Collapsed;

                    if (scrollLeftVisibility == Visibility.Collapsed)
                    {
                        shouldTryFocusScrollRight = m_scrollLeft.Equals(FocusManager.GetFocusedElement());
                    }

                    m_scrollLeft.Visibility = scrollLeftVisibility;
                }

                if (m_scrollRight != null)
                {
                    var scrollRightVisibility =
                        m_textContainer.HorizontalOffset + m_textContainer.ViewportWidth + SCROLL_BUTTONS_APPROXIMATION_RANGE < m_textContainer.ExtentWidth
                            ? Visibility.Visible
                            : Visibility.Collapsed;

                    if (scrollRightVisibility == Visibility.Collapsed && m_scrollLeft != null && m_scrollLeft.Visibility == Visibility.Visible
                        && m_scrollRight.Equals(FocusManager.GetFocusedElement()))
                    {
                        // ScrollRight had the focus and will be collapsed, ScrollLeft should get the focus
                        m_scrollLeft.Focus(FocusState.Programmatic);
                    }
                    m_scrollRight.Visibility = scrollRightVisibility;

                    if (shouldTryFocusScrollRight && scrollRightVisibility == Visibility.Visible)
                    {
                        m_scrollRight.Focus(FocusState.Programmatic);
                    }
                }
            }

            private void ScrollLeft()
            {
                if (m_textContainer == null)
                {
                    return;
                }
                if (m_textContainer.HorizontalOffset > 0)
                {
                    double offset = m_textContainer.HorizontalOffset - (SCROLL_RATIO * m_textContainer.ViewportWidth);
                    m_textContainer.ChangeView(offset, null, null);
                }
            }

            private void ScrollRight()
            {
                if (m_textContainer == null)
                {
                    return;
                }

                if (m_textContainer.HorizontalOffset < m_textContainer.ExtentWidth - m_textContainer.ViewportWidth)
                {
                    double offset = m_textContainer.HorizontalOffset + (SCROLL_RATIO * m_textContainer.ViewportWidth);
                    m_textContainer.ChangeView(offset, null, null);
                }
            }

            private void RaiseSelectedEvent()
            {
                Selected?.Invoke(this);
            }

            private const double SCALEFACTOR = 0.357143;
            private const double SMALLHEIGHTSCALEFACTOR = 0;
            private const double HEIGHTCUTOFF = 100;
            private const double INCREMENTOFFSET = 1;
            private const double MAXFONTINCREMENT = 5;
            private const double WIDTHTOFONTSCALAR = 0.0556513;
            private const double WIDTHTOFONTOFFSET = 3;
            private const double WIDTHCUTOFF = 50;
            private const double FONTTOLERANCE = 0.001;
            private const double SCROLL_RATIO = 0.7;

            // We need a safety margin to guarantee we correctly always show/hide ScrollLeft and ScrollRight buttons when necessary.
            // In rare cases, ScrollViewer::HorizontalOffset is a little low by a few (sub)pixels when users scroll to one of the extremity
            // and no events are launched when they scroll again in the same direction
            private const double SCROLL_BUTTONS_APPROXIMATION_RANGE = 4;

            private Windows.UI.Xaml.Controls.ScrollViewer m_textContainer;
            private Windows.UI.Xaml.Controls.TextBlock m_textBlock;
            private Windows.UI.Xaml.Controls.HyperlinkButton m_scrollLeft;
            private Windows.UI.Xaml.Controls.HyperlinkButton m_scrollRight;
            private bool m_isScalingText;
            private bool m_haveCalculatedMax;
        }
    }
}
