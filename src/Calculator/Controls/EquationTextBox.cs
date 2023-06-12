// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using CalculatorApp.ViewModel.Common;

using Windows.UI.Text;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Automation;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Input;

namespace CalculatorApp
{
    namespace Controls
    {
        public sealed class EquationTextBox : Windows.UI.Xaml.Controls.Control
        {
            public EquationTextBox()
            {
            }

            public Windows.UI.Xaml.Media.SolidColorBrush EquationColor
            {
                get => (Windows.UI.Xaml.Media.SolidColorBrush)GetValue(EquationColorProperty);
                set => SetValue(EquationColorProperty, value);
            }

            // Using a DependencyProperty as the backing store for EquationColor.  This enables animation, styling, binding, etc...
            public static readonly DependencyProperty EquationColorProperty =
                DependencyProperty.Register(nameof(EquationColor), typeof(Windows.UI.Xaml.Media.SolidColorBrush), typeof(EquationTextBox), new PropertyMetadata(default(Windows.UI.Xaml.Media.SolidColorBrush)));

            public Windows.UI.Xaml.Media.SolidColorBrush EquationButtonForegroundColor
            {
                get => (Windows.UI.Xaml.Media.SolidColorBrush)GetValue(EquationButtonForegroundColorProperty);
                set => SetValue(EquationButtonForegroundColorProperty, value);
            }

            // Using a DependencyProperty as the backing store for EquationButtonForegroundColor.  This enables animation, styling, binding, etc...
            public static readonly DependencyProperty EquationButtonForegroundColorProperty =
                DependencyProperty.Register(nameof(EquationButtonForegroundColor), typeof(Windows.UI.Xaml.Media.SolidColorBrush), typeof(EquationTextBox), new PropertyMetadata(default(Windows.UI.Xaml.Media.SolidColorBrush)));

            public Windows.UI.Xaml.Controls.Flyout ColorChooserFlyout
            {
                get => (Windows.UI.Xaml.Controls.Flyout)GetValue(ColorChooserFlyoutProperty);
                set => SetValue(ColorChooserFlyoutProperty, value);
            }

            // Using a DependencyProperty as the backing store for ColorChooserFlyout.  This enables animation, styling, binding, etc...
            public static readonly DependencyProperty ColorChooserFlyoutProperty =
                DependencyProperty.Register(nameof(ColorChooserFlyout), typeof(Windows.UI.Xaml.Controls.Flyout), typeof(EquationTextBox), new PropertyMetadata(default(Windows.UI.Xaml.Controls.Flyout)));

            public string EquationButtonContentIndex
            {
                get => (string)GetValue(EquationButtonContentIndexProperty);
                set => SetValue(EquationButtonContentIndexProperty, value);
            }

            // Using a DependencyProperty as the backing store for EquationButtonContentIndex.  This enables animation, styling, binding, etc...
            public static readonly DependencyProperty EquationButtonContentIndexProperty =
                DependencyProperty.Register(nameof(EquationButtonContentIndex), typeof(string), typeof(EquationTextBox), new PropertyMetadata(string.Empty, (sender, args) =>
                {
                    var self = (EquationTextBox)sender;
                    self.OnEquationButtonContentIndexPropertyChanged((string)args.OldValue, (string)args.NewValue);
                }));

            public string MathEquation
            {
                get => (string)GetValue(MathEquationProperty);
                set => SetValue(MathEquationProperty, value);
            }

            // Using a DependencyProperty as the backing store for MathEquation.  This enables animation, styling, binding, etc...
            public static readonly DependencyProperty MathEquationProperty =
                DependencyProperty.Register(nameof(MathEquation), typeof(string), typeof(EquationTextBox), new PropertyMetadata(string.Empty));

            public bool HasError
            {
                get => (bool)GetValue(HasErrorProperty);
                set => SetValue(HasErrorProperty, value);
            }

            // Using a DependencyProperty as the backing store for HasError.  This enables animation, styling, binding, etc...
            public static readonly DependencyProperty HasErrorProperty =
                DependencyProperty.Register(nameof(HasError), typeof(bool), typeof(EquationTextBox), new PropertyMetadata(default(bool), (sender, args) =>
                {
                    var self = (EquationTextBox)sender;
                    self.OnHasErrorPropertyChanged((bool)args.OldValue, (bool)args.NewValue);
                }));

            public bool IsAddEquationMode
            {
                get => (bool)GetValue(IsAddEquationModeProperty);
                set => SetValue(IsAddEquationModeProperty, value);
            }

            // Using a DependencyProperty as the backing store for IsAddEquationMode.  This enables animation, styling, binding, etc...
            public static readonly DependencyProperty IsAddEquationModeProperty =
                DependencyProperty.Register(nameof(IsAddEquationMode), typeof(bool), typeof(EquationTextBox), new PropertyMetadata(default(bool), (sender, args) =>
                {
                    var self = (EquationTextBox)sender;
                    self.OnIsAddEquationModePropertyChanged((bool)args.OldValue, (bool)args.NewValue);
                }));

            public string ErrorText
            {
                get => (string)GetValue(ErrorTextProperty);
                set => SetValue(ErrorTextProperty, value);
            }

            // Using a DependencyProperty as the backing store for ErrorText.  This enables animation, styling, binding, etc...
            public static readonly DependencyProperty ErrorTextProperty =
                DependencyProperty.Register(nameof(ErrorText), typeof(string), typeof(EquationTextBox), new PropertyMetadata(string.Empty));

            public bool IsEquationLineDisabled
            {
                get => (bool)GetValue(IsEquationLineDisabledProperty);
                set => SetValue(IsEquationLineDisabledProperty, value);
            }

            // Using a DependencyProperty as the backing store for IsEquationLineDisabled.  This enables animation, styling, binding, etc...
            public static readonly DependencyProperty IsEquationLineDisabledProperty =
                DependencyProperty.Register(nameof(IsEquationLineDisabled), typeof(bool), typeof(EquationTextBox), new PropertyMetadata(default(bool)));

            private bool HasFocus { get; set; }

            public event Windows.UI.Xaml.RoutedEventHandler RemoveButtonClicked;
            public event Windows.UI.Xaml.RoutedEventHandler KeyGraphFeaturesButtonClicked;
            public event System.EventHandler<MathRichEditBoxSubmission> EquationSubmitted;
            public event System.EventHandler<MathRichEditBoxFormatRequest> EquationFormatRequested;
            public event Windows.UI.Xaml.RoutedEventHandler EquationButtonClicked;

            public void SetEquationText(string equationText)
            {
                if (m_richEditBox != null)
                {
                    m_richEditBox.MathText = equationText;
                }
            }

            public void FocusTextBox()
            {
                if (m_richEditBox != null)
                {
                    _ = FocusManager.TryFocusAsync(m_richEditBox, FocusState.Programmatic);
                }
            }

            protected override void OnApplyTemplate()
            {
                m_equationButton = GetTemplateChild("EquationButton") as ToggleButton;
                m_richEditBox = GetTemplateChild("MathRichEditBox") as MathRichEditBox;
                m_deleteButton = GetTemplateChild("DeleteButton") as Button;
                m_removeButton = GetTemplateChild("RemoveButton") as Button;
                m_functionButton = GetTemplateChild("FunctionButton") as Button;
                m_colorChooserButton = GetTemplateChild("ColorChooserButton") as ToggleButton;
                m_richEditContextMenu = GetTemplateChild("MathRichEditContextMenu") as MenuFlyout;
                m_kgfEquationMenuItem = GetTemplateChild("FunctionAnalysisMenuItem") as MenuFlyoutItem;
                m_removeMenuItem = GetTemplateChild("RemoveFunctionMenuItem") as MenuFlyoutItem;
                m_colorChooserMenuItem = GetTemplateChild("ChangeFunctionStyleMenuItem") as MenuFlyoutItem;
                m_cutMenuItem = GetTemplateChild("CutMenuItem") as MenuFlyoutItem;
                m_copyMenuItem = GetTemplateChild("CopyMenuItem") as MenuFlyoutItem;
                m_pasteMenuItem = GetTemplateChild("PasteMenuItem") as MenuFlyoutItem;
                m_undoMenuItem = GetTemplateChild("UndoMenuItem") as MenuFlyoutItem;
                m_selectAllMenuItem = GetTemplateChild("SelectAllMenuItem") as MenuFlyoutItem;

                var resProvider = AppResourceProvider.GetInstance();

                if (m_richEditBox != null)
                {
                    m_richEditBox.GotFocus += OnRichEditBoxGotFocus;
                    m_richEditBox.LostFocus += OnRichEditBoxLostFocus;
                    m_richEditBox.TextChanged += OnRichEditTextChanged;
                    m_richEditBox.SelectionFlyout = null;
                    m_richEditBox.EquationSubmitted += OnEquationSubmitted;
                    m_richEditBox.FormatRequest += OnEquationFormatRequested;
                }

                if (m_equationButton != null)
                {
                    m_equationButton.Click += OnEquationButtonClicked;
                }

                if (m_richEditContextMenu != null)
                {
                    m_richEditContextMenu.Opened += OnRichEditMenuOpened;
                }

                if (m_deleteButton != null)
                {
                    m_deleteButton.Click += OnDeleteButtonClicked;
                }

                if (m_removeButton != null)
                {
                    m_removeButton.Click += OnRemoveButtonClicked;
                }

                if (m_removeMenuItem != null)
                {
                    m_removeMenuItem.Text = resProvider.GetResourceString("removeMenuItem");
                    m_removeMenuItem.Click += OnRemoveButtonClicked;
                }

                if (m_colorChooserButton != null)
                {
                    m_colorChooserButton.Click += OnColorChooserButtonClicked;
                }

                if (m_colorChooserMenuItem != null)
                {
                    m_colorChooserMenuItem.Text = resProvider.GetResourceString("colorChooserMenuItem");
                    m_colorChooserMenuItem.Click += OnColorChooserButtonClicked;
                }

                if (m_functionButton != null)
                {
                    m_functionButton.Click += OnFunctionButtonClicked;
                    m_functionButton.IsEnabled = false;
                }

                if (m_kgfEquationMenuItem != null)
                {
                    m_kgfEquationMenuItem.Text = resProvider.GetResourceString("functionAnalysisMenuItem");
                    m_kgfEquationMenuItem.Click += OnFunctionMenuButtonClicked;
                }

                if (ColorChooserFlyout != null)
                {
                    ColorChooserFlyout.Opened += OnColorFlyoutOpened;
                    ColorChooserFlyout.Closed += OnColorFlyoutClosed;
                }

                if (m_cutMenuItem != null)
                {
                    m_cutMenuItem.Click += OnCutClicked;
                }

                if (m_copyMenuItem != null)
                {
                    m_copyMenuItem.Click += OnCopyClicked;
                }

                if (m_pasteMenuItem != null)
                {
                    m_pasteMenuItem.Click += OnPasteClicked;
                }

                if (m_undoMenuItem != null)
                {
                    m_undoMenuItem.Click += OnUndoClicked;
                }

                if (m_selectAllMenuItem != null)
                {
                    m_selectAllMenuItem.Click += OnSelectAllClicked;
                }

                UpdateCommonVisualState();
                UpdateButtonsVisualState();
            }

            protected override void OnPointerEntered(PointerRoutedEventArgs e)
            {
                m_isPointerOver = true;
                UpdateCommonVisualState();
            }

            protected override void OnPointerExited(PointerRoutedEventArgs e)
            {
                m_isPointerOver = false;
                UpdateCommonVisualState();
            }

            protected override void OnPointerCanceled(PointerRoutedEventArgs e)
            {
                m_isPointerOver = false;
                UpdateCommonVisualState();
            }

            protected override void OnPointerCaptureLost(PointerRoutedEventArgs e)
            {
                m_isPointerOver = false;
                UpdateCommonVisualState();
            }

            private void OnIsAddEquationModePropertyChanged(bool oldValue, bool newValue)
            {
                UpdateCommonVisualState();
                UpdateButtonsVisualState();
            }

            private void UpdateCommonVisualState()
            {
                string state;
                bool richEditHasContent = RichEditHasContent();

                if (HasFocus && HasError)
                {
                    state = "FocusedError";
                }
                else if (IsAddEquationMode && HasFocus && !richEditHasContent)
                {
                    state = "AddEquationFocused";
                }
                else if (HasFocus)
                {
                    state = "Focused";
                }
                else if (IsAddEquationMode && m_isPointerOver && !richEditHasContent)
                {
                    state = "AddEquation";
                }
                else if (HasError && (m_isPointerOver || m_isColorChooserFlyoutOpen))
                {
                    state = "PointerOverError";
                }
                else if (m_isPointerOver || m_isColorChooserFlyoutOpen)
                {
                    state = "PointerOver";
                }
                else if (HasError)
                {
                    state = "Error";
                }
                else if (IsAddEquationMode)
                {
                    state = "AddEquation";
                }
                else
                {
                    state = "Normal";
                }
                VisualStateManager.GoToState(this, state, false);
            }

            private void UpdateButtonsVisualState()
            {
                string state;

                if (HasFocus && RichEditHasContent())
                {
                    state = "ButtonVisible";
                }
                else if (IsAddEquationMode)
                {
                    state = "ButtonHideRemove";
                }
                else
                {
                    state = "ButtonCollapsed";
                }

                VisualStateManager.GoToState(this, state, true);
            }

            private bool RichEditHasContent()
            {
                string text = null;
                m_richEditBox?.TextDocument.GetText(Windows.UI.Text.TextGetOptions.NoHidden, out text);
                return !string.IsNullOrEmpty(text);
            }

            private void OnRichEditBoxGotFocus(object sender, RoutedEventArgs e)
            {
                HasFocus = true;
                UpdateCommonVisualState();
                UpdateButtonsVisualState();
            }

            private void OnRichEditBoxLostFocus(object sender, RoutedEventArgs e)
            {
                if (!m_richEditBox.ContextFlyout.IsOpen)
                {
                    HasFocus = false;
                }

                UpdateCommonVisualState();
                UpdateButtonsVisualState();
            }

            private void OnRichEditTextChanged(object sender, RoutedEventArgs e)
            {
                UpdateCommonVisualState();
                UpdateButtonsVisualState();
            }

            private void OnDeleteButtonClicked(object sender, RoutedEventArgs e)
            {
                if (m_richEditBox != null)
                {
                    m_richEditBox.TextDocument.SetText(TextSetOptions.None, "");
                    if (m_functionButton != null)
                    {
                        m_functionButton.IsEnabled = false;
                    }
                }
            }

            private void OnEquationButtonClicked(object sender, RoutedEventArgs e)
            {
                EquationButtonClicked?.Invoke(this, new RoutedEventArgs());

                SetEquationButtonTooltipAndAutomationName();
            }

            private void OnRemoveButtonClicked(object sender, RoutedEventArgs e)
            {
                if (IsAddEquationMode)
                {
                    // Don't remove the last equation
                    return;
                }

                if (m_richEditBox != null)
                {
                    m_richEditBox.MathText = "";
                }

                RemoveButtonClicked?.Invoke(this, new RoutedEventArgs());

                if (m_functionButton != null)
                {
                    m_functionButton.IsEnabled = false;
                }

                if (m_equationButton != null)
                {
                    IsEquationLineDisabled = false;
                }

                TraceLogger.GetInstance().LogGraphButtonClicked(GraphButton.RemoveFunction, GraphButtonValue.None);

                VisualStateManager.GoToState(this, "Normal", true);
            }

            private void OnColorChooserButtonClicked(object sender, RoutedEventArgs e)
            {
                if (ColorChooserFlyout != null && m_richEditBox != null)
                {
                    ColorChooserFlyout.ShowAt(m_richEditBox);
                    TraceLogger.GetInstance().LogGraphButtonClicked(GraphButton.StylePicker, GraphButtonValue.None);
                }
            }

            private void OnFunctionButtonClicked(object sender, RoutedEventArgs e)
            {
                KeyGraphFeaturesButtonClicked?.Invoke(this, new RoutedEventArgs());
            }

            private void OnFunctionMenuButtonClicked(object sender, RoutedEventArgs e)
            {
                // Submit the equation before trying to analyze it if invoked from context menu
                m_richEditBox?.SubmitEquation(EquationSubmissionSource.FOCUS_LOST);

                KeyGraphFeaturesButtonClicked?.Invoke(this, new RoutedEventArgs());
            }

            private void OnRichEditMenuOpened(object sender, object args)
            {
                if (m_removeMenuItem != null)
                {
                    m_removeMenuItem.IsEnabled = !IsAddEquationMode;
                }

                if (m_kgfEquationMenuItem != null)
                {
                    m_kgfEquationMenuItem.IsEnabled = HasFocus && !HasError && RichEditHasContent();
                }

                if (m_colorChooserMenuItem != null)
                {
                    m_colorChooserMenuItem.IsEnabled = !HasError && !IsAddEquationMode;
                }

                if (m_richEditBox != null && m_cutMenuItem != null)
                {
                    m_cutMenuItem.IsEnabled = m_richEditBox.TextDocument.CanCopy();
                }

                if (m_richEditBox != null && m_copyMenuItem != null)
                {
                    m_copyMenuItem.IsEnabled = m_richEditBox.TextDocument.CanCopy();
                }

                if (m_richEditBox != null && m_pasteMenuItem != null)
                {
                    m_pasteMenuItem.IsEnabled = m_richEditBox.TextDocument.CanPaste();
                }

                if (m_richEditBox != null && m_undoMenuItem != null)
                {
                    m_undoMenuItem.IsEnabled = m_richEditBox.TextDocument.CanUndo();
                }
            }

            private void OnCutClicked(object sender, RoutedEventArgs e)
            {
                m_richEditBox?.TextDocument.Selection.Cut();
            }

            private void OnCopyClicked(object sender, RoutedEventArgs e)
            {
                m_richEditBox?.TextDocument.Selection.Copy();
            }

            private void OnPasteClicked(object sender, RoutedEventArgs e)
            {
                m_richEditBox?.TextDocument.Selection.Paste(0);
            }

            private void OnUndoClicked(object sender, RoutedEventArgs e)
            {
                m_richEditBox?.TextDocument.Undo();
            }

            private void OnSelectAllClicked(object sender, RoutedEventArgs e)
            {
                m_richEditBox?.TextDocument.Selection.SetRange(0, m_richEditBox.TextDocument.Selection.EndPosition);
            }

            private void OnColorFlyoutOpened(object sender, object e)
            {
                m_isColorChooserFlyoutOpen = true;
                UpdateCommonVisualState();
            }

            private void OnColorFlyoutClosed(object sender, object e)
            {
                m_colorChooserButton.IsChecked = false;
                m_isColorChooserFlyoutOpen = false;
                UpdateCommonVisualState();
            }

            private void OnHasErrorPropertyChanged(bool oldValue, bool newValue)
            {
                UpdateCommonVisualState();
            }

            private void OnEquationButtonContentIndexPropertyChanged(string oldValue, string newValue)
            {
                SetEquationButtonTooltipAndAutomationName();
            }

            private void SetEquationButtonTooltipAndAutomationName()
            {
                var toolTip = new ToolTip();
                var resProvider = AppResourceProvider.GetInstance();

                var equationButtonMessage = LocalizationStringUtil.GetLocalizedString(
                    IsEquationLineDisabled ? resProvider.GetResourceString("showEquationButtonAutomationName")
                                           : resProvider.GetResourceString("hideEquationButtonAutomationName"),
                    EquationButtonContentIndex);

                var equationButtonTooltip = LocalizationStringUtil.GetLocalizedString(
                    IsEquationLineDisabled ? resProvider.GetResourceString("showEquationButtonToolTip") : resProvider.GetResourceString("hideEquationButtonToolTip"));

                toolTip.Content = equationButtonTooltip;
                ToolTipService.SetToolTip(m_equationButton, toolTip);
                AutomationProperties.SetName(m_equationButton, equationButtonMessage);
            }

            private CalculatorApp.Controls.MathRichEditBox m_richEditBox;
            private Windows.UI.Xaml.Controls.Primitives.ToggleButton m_equationButton;
            private Windows.UI.Xaml.Controls.Button m_deleteButton;
            private Windows.UI.Xaml.Controls.Button m_removeButton;
            private Windows.UI.Xaml.Controls.Button m_functionButton;
            private Windows.UI.Xaml.Controls.Primitives.ToggleButton m_colorChooserButton;

            private Windows.UI.Xaml.Controls.MenuFlyout m_richEditContextMenu;
            private Windows.UI.Xaml.Controls.MenuFlyoutItem m_cutMenuItem;
            private Windows.UI.Xaml.Controls.MenuFlyoutItem m_copyMenuItem;
            private Windows.UI.Xaml.Controls.MenuFlyoutItem m_pasteMenuItem;
            private Windows.UI.Xaml.Controls.MenuFlyoutItem m_undoMenuItem;
            private Windows.UI.Xaml.Controls.MenuFlyoutItem m_selectAllMenuItem;
            private Windows.UI.Xaml.Controls.MenuFlyoutItem m_kgfEquationMenuItem;
            private Windows.UI.Xaml.Controls.MenuFlyoutItem m_removeMenuItem;
            private Windows.UI.Xaml.Controls.MenuFlyoutItem m_colorChooserMenuItem;

            private bool m_isPointerOver;
            private bool m_isColorChooserFlyoutOpen;
            private void OnEquationSubmitted(object sender, MathRichEditBoxSubmission args)
            {
                if (args.HasTextChanged)
                {
                    if (m_functionButton != null && m_richEditBox.MathText != "")
                    {
                        m_functionButton.IsEnabled = true;
                    }
                }

                EquationSubmitted?.Invoke(this, args);
            }

            private void OnEquationFormatRequested(object sender, MathRichEditBoxFormatRequest args)
            {
                EquationFormatRequested?.Invoke(this, args);
            }
        }
    }
}
