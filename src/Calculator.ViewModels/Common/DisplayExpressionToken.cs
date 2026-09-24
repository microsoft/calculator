// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.ComponentModel;
using CommunityToolkit.Mvvm.ComponentModel;
using Windows.UI.Xaml.Data;

namespace CalculatorApp.ViewModel.Common
{
    [Windows.UI.Xaml.Data.Bindable]
    public sealed partial class DisplayExpressionToken : ObservableObject
    {
        [ObservableProperty]
        private string _token;

        [ObservableProperty]
        private int _tokenPosition;

        [ObservableProperty]
        private bool _isTokenEditable;

        [ObservableProperty]
        private int _commandIndex;

        [ObservableProperty]
        private TokenType _type;

        private string _originalToken;
        private bool _inEditMode;

        internal DisplayExpressionToken(string token, int tokenPosition, bool isEditable, TokenType type)
        {
            _token = token;
            _tokenPosition = tokenPosition;
            _isTokenEditable = isEditable;
            _type = type;
            _originalToken = token;
            _inEditMode = false;
        }

        public string OriginalToken
        {
            get => _originalToken;
            private set => SetProperty(ref _originalToken, value);
        }

        public bool IsTokenInEditMode
        {
            get => _inEditMode;
            set
            {
                if (!value)
                {
                    OriginalToken = Token;
                }
                _inEditMode = value;
            }
        }
    }
}
