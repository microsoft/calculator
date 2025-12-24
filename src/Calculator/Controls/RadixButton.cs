// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using CalculatorApp.ViewModel.Common;
using Windows.UI.Xaml;

namespace CalculatorApp
{
    namespace Controls
    {
        public sealed class RadixButton : Windows.UI.Xaml.Controls.RadioButton
        {
            public RadixButton()
            { }

            internal string GetRawDisplayValue()
            {
                string radixContent = Content?.ToString();
                return LocalizationSettings.GetInstance().RemoveGroupSeparators(radixContent);
            }

            public string Label
            {
                get => (string)GetValue(LabelProperty);
                set => SetValue(LabelProperty, value);
            }

            public static readonly DependencyProperty LabelProperty =
                DependencyProperty.Register(nameof(Label), typeof(string), typeof(RadixButton), new PropertyMetadata(default(string)));
        }
    }
}
