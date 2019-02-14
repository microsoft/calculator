// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "CalcManager/CalculatorVector.h"
#include "CalcManager/ExpressionCommandInterface.h"

namespace CalculatorApp
{
    namespace ViewModel
    {
        [Windows::UI::Xaml::Data::Bindable]
        public ref class HistoryItemViewModel sealed : Windows::UI::Xaml::Data::ICustomPropertyProvider
        {

        internal:

            HistoryItemViewModel(Platform::String^ expression,
                Platform::String^ result, 
                _In_ std::shared_ptr<CalculatorVector <std::pair<std::wstring, int>>> const &spTokens,
                _In_ std::shared_ptr<CalculatorVector<std::shared_ptr<IExpressionCommand>>> const &spCommands);

            std::shared_ptr<CalculatorVector <std::pair<std::wstring, int>>> const& GetTokens()
            {
                return m_spTokens; 
            }

            std::shared_ptr<CalculatorVector<std::shared_ptr<IExpressionCommand>>> const& GetCommands()
            { 
                return m_spCommands; 
            }

        public:

            property Platform::String^ Expression
            {
                Platform::String^ get() { return m_expression; }
            }

            property Platform::String^ AccExpression
            {
                Platform::String^ get() { return m_accExpression; }
            }

            property Platform::String^ Result
            {
                Platform::String^ get() { return m_result; }
            }

            property Platform::String^ AccResult
            {
                Platform::String^ get() { return m_accResult; }
            }

            virtual Windows::UI::Xaml::Data::ICustomProperty^ GetCustomProperty(Platform::String^ name)
            {
                return nullptr;
            }

            virtual Windows::UI::Xaml::Data::ICustomProperty^ GetIndexedProperty(Platform::String^ name, Windows::UI::Xaml::Interop::TypeName type)
            {
                return nullptr;
            }

            virtual property Windows::UI::Xaml::Interop::TypeName Type
            {
                Windows::UI::Xaml::Interop::TypeName get()
                {
                    return this->GetType();
                }
            }

            virtual Platform::String^ GetStringRepresentation()
            {
                return m_accExpression + " " + m_accResult;
            }

        private:
            static Platform::String^ GetAccessibleExpressionFromTokens(
                _In_ std::shared_ptr<CalculatorVector <std::pair<std::wstring, int>>> const &spTokens,
                _In_ Platform::String^ fallbackExpression);

        private:
            Platform::String^ m_expression;
            Platform::String^ m_accExpression;
            Platform::String^ m_accResult;
            Platform::String^ m_result;
            std::shared_ptr<CalculatorVector <std::pair<std::wstring, int>>> m_spTokens;
            std::shared_ptr<CalculatorVector<std::shared_ptr<IExpressionCommand>>> m_spCommands;
        };
    }
}
