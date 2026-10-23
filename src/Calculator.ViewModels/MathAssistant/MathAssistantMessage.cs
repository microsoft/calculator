// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

namespace CalculatorApp.ViewModel.MathAssistant
{
    /// <summary>
    /// One message in the Math Assistant chat: either typed by the user or written by the AI.
    /// </summary>
    [Windows.UI.Xaml.Data.Bindable]
    public sealed class MathAssistantMessage
    {
        public MathAssistantMessage(string text, bool isFromUser)
        {
            Text = text;
            IsFromUser = isFromUser;
        }

        public string Text { get; }

        public bool IsFromUser { get; }
    }
}
