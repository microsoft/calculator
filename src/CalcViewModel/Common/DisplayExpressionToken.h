// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "Utils.h"

namespace CalculatorApp::ViewModel::Common
{
public
    enum class TokenType
    {
        Operator,
        Operand,
        Separator
    };

    [Windows::UI::Xaml::Data::Bindable] public ref class DisplayExpressionToken sealed : public Windows::UI::Xaml::Data::INotifyPropertyChanged
    {
        internal : DisplayExpressionToken(Platform::String ^ token, int tokenPosition, bool fEditable, TokenType type)
            : m_Token(token)
            , m_TokenPosition(tokenPosition)
            , m_IsTokenEditable(fEditable)
            , m_Type(type)
            , m_OriginalToken(token)
            , m_InEditMode(false)
        {
        }

    public:
        OBSERVABLE_OBJECT();
        OBSERVABLE_PROPERTY_RW(Platform::String ^, Token);
        OBSERVABLE_PROPERTY_RW(int, TokenPosition);
        OBSERVABLE_PROPERTY_RW(bool, IsTokenEditable);
        OBSERVABLE_PROPERTY_RW(int, CommandIndex);
        OBSERVABLE_PROPERTY_RW(TokenType, Type);
        OBSERVABLE_PROPERTY_R(Platform::String ^, OriginalToken);

        property bool IsTokenInEditMode
        {
            bool get()
            {
                return m_InEditMode;
            }
            void set(bool val)
            {
                if (!val)
                {
                    m_OriginalToken = ref new Platform::String(m_Token->Data());
                }
                m_InEditMode = val;
            }
        }

    private:
        bool m_InEditMode;
    };
}
