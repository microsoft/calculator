// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "CalculatorButtonPressedEventArgs.h"

using namespace CalculatorApp;
using namespace CalculatorApp::ViewModel;
using namespace CalculatorApp::ViewModel::Common;
using namespace Platform;

NumbersAndOperatorsEnum CalculatorButtonPressedEventArgs::GetOperationFromCommandParameter(_In_ Object ^ commandParameter)
{
    auto eventArgs = dynamic_cast<CalculatorButtonPressedEventArgs ^>(commandParameter);
    if (eventArgs != nullptr)
    {
        return eventArgs->Operation;
    }
    else
    {
        return safe_cast<NumbersAndOperatorsEnum>(commandParameter);
    }
}

String ^ CalculatorButtonPressedEventArgs::GetAuditoryFeedbackFromCommandParameter(_In_ Object ^ commandParameter)
{
    auto eventArgs = dynamic_cast<CalculatorButtonPressedEventArgs ^>(commandParameter);
    if (eventArgs != nullptr)
    {
        return eventArgs->AuditoryFeedback;
    }
    else
    {
        return nullptr;
    }
}
