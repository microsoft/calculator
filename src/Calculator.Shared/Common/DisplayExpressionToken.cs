// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.
using System.ComponentModel;

namespace CalculatorApp.Common
{
    public enum TokenType
    {
        Operator,
        Operand,
        Separator
    };

    [Windows.UI.Xaml.Data.Bindable]
    public sealed class DisplayExpressionToken : INotifyPropertyChanged
    {
        internal DisplayExpressionToken(string token, int tokenPosition, bool fEditable, TokenType type)
        {
            m_Token = token;
            m_TokenPosition = tokenPosition;
            m_IsTokenEditable = fEditable;
            m_Type = type;
            m_OriginalToken = token;
            m_InEditMode = false;
        }

        public event PropertyChangedEventHandler PropertyChanged;
        private void RaisePropertyChanged([System.Runtime.CompilerServices.CallerMemberName]string p = null) => PropertyChanged?.Invoke(this, new PropertyChangedEventArgs(p));
        private string m_Token;
        public string Token { get => m_Token; set { m_Token = value; RaisePropertyChanged("Token"); } }


        private int m_TokenPosition;
        public int TokenPosition { get => m_TokenPosition; set { m_TokenPosition = value; RaisePropertyChanged("TokenPosition"); } }


        private bool m_IsTokenEditable;
        public bool IsTokenEditable { get => m_IsTokenEditable; set { m_IsTokenEditable = value; RaisePropertyChanged("IsTokenEditable"); } }


        private int m_CommandIndex;
        public int CommandIndex { get => m_CommandIndex; set { m_CommandIndex = value; RaisePropertyChanged("CommandIndex"); } }


        private string m_OriginalToken;
        public string OriginalToken { get => m_OriginalToken; private set { m_OriginalToken = value; RaisePropertyChanged("OriginalToken"); } }

        bool IsTokenInEditMode
        {
            get
            {
                return m_InEditMode;
            }
            set
            {
                if (!value)
                {
                    m_OriginalToken = m_Token;
                }
                m_InEditMode = value;
            }
        }

        private TokenType m_Type;
        public TokenType Type { get => m_Type; set { m_Type = value; RaisePropertyChanged("Type"); } }

        bool m_InEditMode;
    };
}
