// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "CalculatorButtonUser.h"
#include "Utils.h"

namespace CalculatorApp::ViewModel
{
    namespace Common
    {
    public
        ref class CalculatorButtonPressedEventArgs sealed
        {
        public:
            PROPERTY_R(Platform::String ^, AuditoryFeedback);
            PROPERTY_R(CalculatorApp::ViewModel::Common::NumbersAndOperatorsEnum, Operation);

            CalculatorButtonPressedEventArgs(Platform::String ^ feedback, CalculatorApp::ViewModel::Common::NumbersAndOperatorsEnum operation)
                : m_AuditoryFeedback(feedback)
                , m_Operation(operation)
            {
            }

            static CalculatorApp::ViewModel::Common::NumbersAndOperatorsEnum GetOperationFromCommandParameter(_In_ Platform::Object ^ commandParameter);
            static Platform::String ^ GetAuditoryFeedbackFromCommandParameter(_In_ Platform::Object ^ commandParameter);
        };
    }
}
