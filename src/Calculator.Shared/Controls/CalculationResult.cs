// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using CalculatorApp.Common;
using System;
using System.Diagnostics;
using Windows.Devices.Input;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Automation.Peers;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;

namespace CalculatorApp
{
    namespace Controls
    {
        public delegate void SelectedEventHandler(object sender);

        public sealed partial class CalculationResult : Windows.UI.Xaml.Controls.Control
        {
            public Visibility ExpressionVisibility
            {
                get { return (Visibility)GetValue(ExpressionVisibilityProperty); }
                set { SetValue(ExpressionVisibilityProperty, value); }
            }

            public static readonly DependencyProperty ExpressionVisibilityProperty =
                DependencyProperty.Register("ExpressionVisibility", typeof(Visibility), typeof(CalculationResult), new PropertyMetadata(Visibility.Collapsed));



            public double MinFontSize
            {
                get { return (double)GetValue(MinFontSizeProperty); }
                set { SetValue(MinFontSizeProperty, value); }
            }

            public static readonly DependencyProperty MinFontSizeProperty =
                DependencyProperty.Register("MinFontSize", typeof(double), typeof(CalculationResult), new PropertyMetadata(0.0));



            public double MaxFontSize
            {
                get { return (double)GetValue(MaxFontSizeProperty); }
                set { SetValue(MaxFontSizeProperty, value); }
            }

            public static readonly DependencyProperty MaxFontSizeProperty =
                DependencyProperty.Register("MaxFontSize", typeof(double), typeof(CalculationResult), new PropertyMetadata(30.0));



            public Thickness DisplayMargin
            {
                get { return (Thickness)GetValue(DisplayMarginProperty); }
                set { SetValue(DisplayMarginProperty, value); }
            }

            public static readonly DependencyProperty DisplayMarginProperty =
                DependencyProperty.Register("DisplayMargin", typeof(Thickness), typeof(CalculationResult), new PropertyMetadata(default(Thickness)));



            public int MaxExpressionHistoryCharacters
            {
                get { return (int)GetValue(MaxExpressionHistoryCharactersProperty); }
                set { SetValue(MaxExpressionHistoryCharactersProperty, value); }
            }

            public static readonly DependencyProperty MaxExpressionHistoryCharactersProperty =
                DependencyProperty.Register("MaxExpressionHistoryCharacters", typeof(int), typeof(CalculationResult), new PropertyMetadata(0));



            public bool IsActive
            {
                get { return (bool)GetValue(IsActiveProperty); }
                set { SetValue(IsActiveProperty, value); }
            }

            public static readonly DependencyProperty IsActiveProperty =
                DependencyProperty.Register("IsActive", typeof(bool), typeof(CalculationResult), new PropertyMetadata(false));




            public string DisplayValue
            {
                get { return (string)GetValue(DisplayValueProperty); }
                set { SetValue(DisplayValueProperty, value); }
            }

            // Using a DependencyProperty as the backing store for DisplayValue.  This enables animation, styling, binding, etc...
            public static readonly DependencyProperty DisplayValueProperty =
                DependencyProperty.Register("DisplayValue", typeof(string), typeof(CalculationResult), new PropertyMetadata(""));



            public string DisplayStringExpression
            {
                get { return (string)GetValue(DisplayStringExpressionProperty); }
                set { SetValue(DisplayStringExpressionProperty, value); }
            }

            // Using a DependencyProperty as the backing store for DisplayStringExpression.  This enables animation, styling, binding, etc...
            public static readonly DependencyProperty DisplayStringExpressionProperty =
                DependencyProperty.Register("DisplayStringExpression", typeof(string), typeof(CalculationResult), new PropertyMetadata(""));

            public bool IsInError
            {
                get { return (bool)GetValue(IsInErrorProperty); }
                set { SetValue(IsInErrorProperty, value); }
            }

            // Using a DependencyProperty as the backing store for IsInError.  This enables animation, styling, binding, etc...
            public static readonly DependencyProperty IsInErrorProperty =
                DependencyProperty.Register("IsInError", typeof(bool), typeof(CalculationResult), new PropertyMetadata(false));



            public bool IsOperatorCommand
            {
                get { return (bool)GetValue(IsOperatorCommandProperty); }
                set { SetValue(IsOperatorCommandProperty, value); }
            }

            // Using a DependencyProperty as the backing store for IsOperatorCommand.  This enables animation, styling, binding, etc...
            public static readonly DependencyProperty IsOperatorCommandProperty =
                DependencyProperty.Register("IsOperatorCommand", typeof(bool), typeof(CalculationResult), new PropertyMetadata(false));

            event SelectedEventHandler Selected;


            private Windows.UI.Xaml.Controls.ScrollViewer m_textContainer;
            private Windows.UI.Xaml.Controls.TextBlock m_textBlock;
            private Windows.UI.Xaml.Controls.HyperlinkButton m_scrollLeft;
            private Windows.UI.Xaml.Controls.HyperlinkButton m_scrollRight;
            private double scrollRatio = 0.7;
            private bool m_isScalingText;
            private bool m_haveCalculatedMax;



            const double SCALEFACTOR = 0.357143;
            const int SMALLHEIGHTSCALEFACTOR = 0;
            const int HEIGHTCUTOFF = 100;
            const int INCREMENTOFFSET = 1;
            const int MAXFONTINCREMENT = 5;
            const double WIDTHTOFONTSCALAR = 0.0556513;
            const int WIDTHTOFONTOFFSET = 3;
            const int WIDTHCUTOFF = 50;
            const double FONTTOLERANCE = 0.001;

            // Visual states for focused
            static string s_FocusedState = "Focused";
            static string s_UnfocusedState = "Unfocused";

            public CalculationResult()
            {
                m_isScalingText = false;
                m_haveCalculatedMax = false;
            }

            string GetRawDisplayValue()
            {
                string rawValue = null;

                LocalizationSettings.GetInstance().RemoveGroupSeparators(DisplayValue, DisplayValue.Length, ref rawValue);

                return rawValue;
            }

            protected override void OnApplyTemplate()
            {
                System.Diagnostics.Debug.Assert((m_scrollLeft == null && m_scrollRight == null) || (m_scrollLeft != null && m_scrollRight != null));
                if (m_textContainer != null)
                {
                    // UNO TODO
                    // m_textContainer.LayoutUpdated -= m_textContainerLayoutChangedToken;
                }
                m_textContainer = (ScrollViewer)(GetTemplateChild("TextContainer"));
                if (m_textContainer != null)
                {
                    m_textContainer.SizeChanged += TextContainerSizeChanged;
                    // We want to know when the size of the container changes so
                    // we can rescale the textbox
                    m_textContainer.LayoutUpdated += OnTextContainerLayoutUpdated;

                    m_textContainer.ChangeView(m_textContainer.ExtentWidth - m_textContainer.ViewportWidth, null, null);
                    m_scrollLeft = (HyperlinkButton)(GetTemplateChild("ScrollLeft"));
                    m_scrollRight = (HyperlinkButton)(GetTemplateChild("ScrollRight"));
                    var borderContainer = (UIElement)(GetTemplateChild("Border"));
                    if (m_scrollLeft != null && m_scrollRight != null)
                    {
                        m_scrollLeft.Click += OnScrollClick;
                        m_scrollRight.Click += OnScrollClick;
                        borderContainer.PointerEntered += OnPointerEntered;
                        borderContainer.PointerExited += OnPointerExited;
                    }
                    m_textBlock = (TextBlock)(m_textContainer.FindName("NormalOutput"));
                    if (m_textBlock != null)
                    {
                        m_textBlock.Visibility = Visibility.Visible;
                    }
                }
                UpdateAllState();
                VisualStateManager.GoToState(this, s_UnfocusedState, false);
            }

            void OnPointerPressed(PointerRoutedEventArgs e)
            {
                if (m_scrollLeft != null && m_scrollRight != null && e.Pointer.PointerDeviceType == PointerDeviceType.Touch)
                {
                    ShowHideScrollButtons(Visibility.Collapsed, Visibility.Collapsed);
                }
            }

            void OnTextContainerLayoutUpdated(object sender, object e)
            {
                if (m_isScalingText)
                {
                    UpdateTextState();
                }
            }

            void TextContainerSizeChanged(object sender, SizeChangedEventArgs e)
            {
                UpdateTextState();
            }

            void OnIsActivePropertyChanged(bool oldValue, bool newValue)
            {
                UpdateVisualState();
            }

            void OnAccentColorPropertyChanged(Brush oldValue, Brush newValue)
            {
                // Force the "Active" transition to happen again
                if (IsActive)
                {
                    VisualStateManager.GoToState(this, "Normal", true);
                    VisualStateManager.GoToState(this, "Active", true);
                }
            }

            void OnDisplayValuePropertyChanged(string oldValue, string newValue)
            {
                UpdateTextState();
            }

            void OnMinFontSizePropertyChanged(double oldValue, double newValue)
            {
                UpdateTextState();
            }

            void OnMaxFontSizePropertyChanged(double oldValue, double newValue)
            {
                UpdateTextState();
            }

            void OnIsInErrorPropertyChanged(bool oldValue, bool newValue)
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

            void UpdateTextState()
            {
                if ((m_textContainer == null) || (m_textBlock == null))
                {
                    return;
                }

                var containerSize = m_textContainer.ActualWidth;
                string oldText = m_textBlock.Text;
                string newText = Utils.LRO + DisplayValue + Utils.PDF;

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
                    System.Diagnostics.Debug.Assert(m_textBlock.FontSize >= MinFontSize && m_textBlock.FontSize <= MaxFontSize);
                    m_isScalingText = false;
                    if (IsOperatorCommand)
                    {
                        m_textContainer.ChangeView(0.0, null, null);
                    }
                    else
                    {
                        m_textContainer.ChangeView(m_textContainer.ExtentWidth - m_textContainer.ViewportWidth, null, null);
                    }

                    if (m_scrollLeft != null && m_scrollRight != null)
                    {
                        if (m_textBlock.ActualWidth < containerSize)
                        {
                            ShowHideScrollButtons(Visibility.Collapsed, Visibility.Collapsed);
                        }
                        else
                        {
                            if (IsOperatorCommand)
                            {
                                ShowHideScrollButtons(Visibility.Collapsed, Visibility.Visible);
                            }
                            else
                            {
                                ShowHideScrollButtons(Visibility.Visible, Visibility.Collapsed);
                            }
                        }
                    }
                    m_textBlock.InvalidateArrange();
                }
            }

            void ScrollLeft()
            {
                if (m_textContainer.HorizontalOffset > 0)
                {
                    double offset = m_textContainer.HorizontalOffset - (scrollRatio * m_textContainer.ViewportWidth);
                    m_textContainer.ChangeView(offset, null, null);
                    m_textContainer.UpdateLayout();
                    UpdateScrollButtons();
                }
            }

            void ScrollRight()
            {
                if (m_textContainer.HorizontalOffset < m_textContainer.ExtentWidth - m_textContainer.ViewportWidth)
                {
                    double offset = m_textContainer.HorizontalOffset + (scrollRatio * m_textContainer.ViewportWidth);
                    m_textContainer.ChangeView(offset, null, null);
                    m_textContainer.UpdateLayout();
                    UpdateScrollButtons();
                }
            }

            void OnKeyDown(KeyRoutedEventArgs e)
            {
                if (m_scrollLeft != null && m_scrollRight != null)
                {
                    var key = e.Key;
                    if (key == Windows.System.VirtualKey.Left)
                    {
                        this.ScrollLeft();
                    }
                    else if (key == Windows.System.VirtualKey.Right)
                    {
                        this.ScrollRight();
                    }
                }
            }

            void OnScrollClick(object sender, RoutedEventArgs e)
            {
                var clicked = (HyperlinkButton)(sender);
                if (clicked == m_scrollLeft)
                {
                    this.ScrollLeft();
                }
                else
                {
                    this.ScrollRight();
                }
            }

            void OnPointerEntered(object sender, PointerRoutedEventArgs e)
            {
                if (e.Pointer.PointerDeviceType == PointerDeviceType.Mouse && m_textBlock.ActualWidth >= m_textContainer.ActualWidth)
                {
                    UpdateScrollButtons();
                }
            }

            void ShowHideScrollButtons(Visibility vLeft, Visibility vRight)
            {
                m_scrollLeft.Visibility = vLeft;
                m_scrollRight.Visibility = vRight;
            }

            void UpdateScrollButtons()
            {
                // When the width is smaller than the container, don't show any
                if (m_textBlock.ActualWidth < m_textContainer.ActualWidth)
                {
                    ShowHideScrollButtons(Visibility.Collapsed, Visibility.Collapsed);
                }
                // We have more number on both side. Show both arrows
                else if (m_textContainer.HorizontalOffset > 0 && m_textContainer.HorizontalOffset < (m_textContainer.ExtentWidth - m_textContainer.ViewportWidth))
                {
                    ShowHideScrollButtons(Visibility.Visible, Visibility.Visible);
                }
                // Width is larger than the container and left most part of the number is shown. Should be able to scroll left.
                else if (m_textContainer.HorizontalOffset == 0)
                {
                    ShowHideScrollButtons(Visibility.Collapsed, Visibility.Visible);
                }
                else // Width is larger than the container and right most part of the number is shown. Should be able to scroll left.
                {
                    ShowHideScrollButtons(Visibility.Visible, Visibility.Collapsed);
                }
            }

            void OnPointerExited(object sender, PointerRoutedEventArgs e)
            {
                if (e.Pointer.PointerDeviceType == PointerDeviceType.Mouse)
                {
                    UpdateScrollButtons();
                }
            }

            void ModifyFontAndMargin(TextBlock textBox, double fontChange)
            {
                double cur = textBox.FontSize;
                double newFontSize = 0.0;
                double scaleFactor = SCALEFACTOR;
                if (m_textContainer.ActualHeight <= HEIGHTCUTOFF)
                {
                    scaleFactor = SMALLHEIGHTSCALEFACTOR;
                }

                newFontSize = Math.Min(Math.Max(cur + fontChange, MinFontSize), MaxFontSize);
                m_textContainer.Padding = new Thickness(0, 0, 0, scaleFactor * Math.Abs(cur - newFontSize));
                textBox.FontSize = newFontSize;
            }

            void UpdateAllState()
            {
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
                this.Focus(FocusState.Programmatic);
            }

            protected override void OnGotFocus(RoutedEventArgs e)
            {
                if (this.FocusState == FocusState.Keyboard)
                {
                    VisualStateManager.GoToState(this, s_FocusedState, true);
                }
            }

            protected override void OnLostFocus(RoutedEventArgs e)
            {
                VisualStateManager.GoToState(this, s_UnfocusedState, true);
            }

            protected override AutomationPeer OnCreateAutomationPeer()
            {
                return new CalculationResultAutomationPeer(this);
            }

            void ProgrammaticSelect()
            {
                RaiseSelectedEvent();
            }

            void RaiseSelectedEvent()
            {
                Selected?.Invoke(this);
            }
        };
    }
}
