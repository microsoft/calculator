// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.ComponentModel;
using Windows.UI.Xaml.Data;

namespace CalculatorApp.ViewModel.Common
{
    [Windows.UI.Xaml.Data.Bindable]
    public sealed class DisplayExpressionToken : INotifyPropertyChanged
    {
        private string _token;
        private int _tokenPosition;
        private bool _isTokenEditable;
        private int _commandIndex;
        private TokenType _type;
        private string _originalToken;
        private bool _inEditMode;

        public event PropertyChangedEventHandler PropertyChanged;

        internal DisplayExpressionToken(string token, int tokenPosition, bool isEditable, TokenType type)
        {
            _token = token;
            _tokenPosition = tokenPosition;
            _isTokenEditable = isEditable;
            _type = type;
            _originalToken = token;
            _inEditMode = false;
        }

        public string Token
        {
            get => _token;
            set
            {
                if (_token != value)
                {
                    _token = value;
                    RaisePropertyChanged(nameof(Token));
                }
            }
        }

        public int TokenPosition
        {
            get => _tokenPosition;
            set
            {
                if (_tokenPosition != value)
                {
                    _tokenPosition = value;
                    RaisePropertyChanged(nameof(TokenPosition));
                }
            }
        }

        public bool IsTokenEditable
        {
            get => _isTokenEditable;
            set
            {
                if (_isTokenEditable != value)
                {
                    _isTokenEditable = value;
                    RaisePropertyChanged(nameof(IsTokenEditable));
                }
            }
        }

        public int CommandIndex
        {
            get => _commandIndex;
            set
            {
                if (_commandIndex != value)
                {
                    _commandIndex = value;
                    RaisePropertyChanged(nameof(CommandIndex));
                }
            }
        }

        public TokenType Type
        {
            get => _type;
            set
            {
                if (_type != value)
                {
                    _type = value;
                    RaisePropertyChanged(nameof(Type));
                }
            }
        }

        public string OriginalToken
        {
            get => _originalToken;
            private set
            {
                if (_originalToken != value)
                {
                    _originalToken = value;
                    RaisePropertyChanged(nameof(OriginalToken));
                }
            }
        }

        public bool IsTokenInEditMode
        {
            get => _inEditMode;
            set
            {
                if (!value)
                {
                    OriginalToken = _token;
                }
                _inEditMode = value;
            }
        }

        private void RaisePropertyChanged(string propertyName)
        {
            PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(propertyName));
        }
    }
}
