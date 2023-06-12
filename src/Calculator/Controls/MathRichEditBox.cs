// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Runtime.InteropServices;

using Windows.ApplicationModel;
using Windows.System;
using Windows.UI.Core;
using Windows.UI.Text;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Input;

namespace CalculatorApp
{
    namespace Controls
    {
        namespace Windows_2004_Prerelease
        {
            public enum RichEditMathMode
            {
                NoMath,
                MathOnly
            }

            [Guid("619c20f2-cb3b-4521-981f-2865b1b93f04")]
            internal interface ITextDocument4
            {
                int SetMath(string value);
                int GetMath(out string value);
                int SetMathMode(RichEditMathMode mathMode);
            }
        }

        public enum EquationSubmissionSource
        {
            FOCUS_LOST,
            ENTER_KEY,
            PROGRAMMATIC
        }

        public sealed class MathRichEditBoxSubmission
        {
            public bool HasTextChanged { get; }

            public EquationSubmissionSource Source { get; }

            public MathRichEditBoxSubmission(bool hasTextChanged, EquationSubmissionSource source)
            {
                HasTextChanged = hasTextChanged;
                Source = source;
            }

        }

        public sealed class MathRichEditBoxFormatRequest
        {
            public string OriginalText { get; }

            public string FormattedText { get; set; }

            public MathRichEditBoxFormatRequest(string originalText)
            {
                OriginalText = originalText;
            }

        }

        public sealed class MathRichEditBox : Windows.UI.Xaml.Controls.RichEditBox
        {
            public MathRichEditBox()
            {
                string packageName = Package.Current.Id.Name;

                if (packageName == "Microsoft.WindowsCalculator.Dev")
                {
                    LimitedAccessFeatures.TryUnlockFeature(
                                "com.microsoft.windows.richeditmath",
                                "BeDD/jxKhz/yfVNA11t4uA==", // Microsoft.WindowsCalculator.Dev
                                "8wekyb3d8bbwe has registered their use of com.microsoft.windows.richeditmath with Microsoft and agrees to the terms of use.");
                }
                else if (packageName == "Microsoft.WindowsCalculator")
                {
                    LimitedAccessFeatures.TryUnlockFeature(
                                "com.microsoft.windows.richeditmath",
                                "pfanNuxnzo+mAkBQ3N/rGQ==", // Microsoft.WindowsCalculator
                                "8wekyb3d8bbwe has registered their use of com.microsoft.windows.richeditmath with Microsoft and agrees to the terms of use.");
                }

                TextDocument.SetMathMode(RichEditMathMode.MathOnly);
                LosingFocus += OnLosingFocus;
                KeyUp += OnKeyUp;
            }

            public string MathText
            {
                get => (string)GetValue(MathTextProperty);
                set => SetValue(MathTextProperty, value);
            }

            // Using a DependencyProperty as the backing store for MathText.  This enables animation, styling, binding, etc...
            public static readonly DependencyProperty MathTextProperty =
                DependencyProperty.Register(nameof(MathText), typeof(string), typeof(MathRichEditBox), new PropertyMetadata(string.Empty, (sender, args) =>
                {
                    var self = (MathRichEditBox)sender;
                    self.OnMathTextPropertyChanged((string)args.OldValue, (string)args.NewValue);
                }));

            public event EventHandler<MathRichEditBoxFormatRequest> FormatRequest;
            public event EventHandler<MathRichEditBoxSubmission> EquationSubmitted;

            public void OnMathTextPropertyChanged(string oldValue, string newValue)
            {
                SetMathTextProperty(newValue);

                // Get the new math text directly from the TextBox since the textbox may have changed its formatting
                SetValue(MathTextProperty, GetMathTextProperty());
            }

            public void InsertText(string text, int cursorOffSet, int selectionLength)
            {
                // If the rich edit is empty, the math zone may not exist, and so selection (and thus the resulting text) will not be in a math zone.
                // If the rich edit has content already, then the mathzone will already be created due to mathonly mode being set and the selection will exist inside the
                // math zone. To handle this, we will force a math zone to be created in teh case of the text being empty and then replacing the text inside of the math
                // zone with the newly inserted text.
                if (string.IsNullOrEmpty(GetMathTextProperty()))
                {
                    SetMathTextProperty("<math xmlns=\"http://www.w3.org/1998/Math/MathML\"><mi>x</mi></math>");
                    TextDocument.Selection.StartPosition = 0;
                    TextDocument.Selection.EndPosition = 1;
                }

                // insert the text in place of selection
                TextDocument.Selection.SetText(Windows.UI.Text.TextSetOptions.FormatRtf, text);

                // Move the cursor to the next logical place for users to enter text.
                TextDocument.Selection.StartPosition += cursorOffSet;
                TextDocument.Selection.EndPosition = TextDocument.Selection.StartPosition + selectionLength;
            }

            public void SubmitEquation(EquationSubmissionSource source)
            {
                // Clear formatting since the graph control doesn't work with bold/underlines
                var range = TextDocument.GetRange(0, TextDocument.Selection.EndPosition);

                if (range != null)
                {
                    range.CharacterFormat.Underline = UnderlineType.None;
                }

                var newVal = GetMathTextProperty();
                if (MathText != newVal)
                {
                    // Request the final formatting of the text
                    var formatRequest = new MathRichEditBoxFormatRequest(newVal);
                    FormatRequest?.Invoke(this, formatRequest);

                    if (!string.IsNullOrEmpty(formatRequest.FormattedText))
                    {
                        newVal = formatRequest.FormattedText;
                    }

                    SetValue(MathTextProperty, newVal);
                    EquationSubmitted?.Invoke(this, new MathRichEditBoxSubmission(true, source));
                }
                else
                {
                    EquationSubmitted?.Invoke(this, new MathRichEditBoxSubmission(false, source));
                }
            }

            public void BackSpace()
            {
                // if anything is selected, just delete the selection.  Note: EndPosition can be before start position.
                if (TextDocument.Selection.StartPosition != TextDocument.Selection.EndPosition)
                {
                    TextDocument.Selection.SetText(Windows.UI.Text.TextSetOptions.None, "");
                    return;
                }

                // if we are at the start of the string, do nothing
                if (TextDocument.Selection.StartPosition == 0)
                {
                    return;
                }

                // Select the previous group.
                TextDocument.Selection.EndPosition = TextDocument.Selection.StartPosition;
                TextDocument.Selection.StartPosition -= 1;

                // If the group contains anything complex, we want to give the user a chance to preview the deletion.
                // If it's a single character, then just delete it.  Otherwise do nothing until the user triggers backspace again.
                var text = TextDocument.Selection.Text;
                if (text.Length == 1)
                {
                    TextDocument.Selection.SetText(Windows.UI.Text.TextSetOptions.None, "");
                }
            }

            protected override void OnKeyDown(KeyRoutedEventArgs e)
            {
                // suppress control + B to prevent bold input from being entered
                if ((Window.Current.CoreWindow.GetKeyState(VirtualKey.Control) & CoreVirtualKeyStates.Down) != CoreVirtualKeyStates.Down ||
                    e.Key != VirtualKey.B)
                {
                    base.OnKeyDown(e);
                }
            }

            private string GetMathTextProperty()
            {
                TextDocument.GetMath(out string math);
                return math;
            }

            private void SetMathTextProperty(string newValue)
            {
                bool readOnlyState = IsReadOnly;
                IsReadOnly = false;

                TextDocument.SetMath(newValue);

                IsReadOnly = readOnlyState;
            }

            private void OnLosingFocus(UIElement sender, LosingFocusEventArgs args)
            {
                if (IsReadOnly || ContextFlyout.IsOpen)
                {
                    return;
                }

                SubmitEquation(EquationSubmissionSource.FOCUS_LOST);
            }

            private void OnKeyUp(object sender, KeyRoutedEventArgs e)
            {
                if (!IsReadOnly && e.Key == VirtualKey.Enter)
                {
                    SubmitEquation(EquationSubmissionSource.ENTER_KEY);
                }
            }
        }
    }
}

