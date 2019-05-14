// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

namespace CalculatorApp
{
    namespace Common
    {
        public sealed class CalculatorButtonPressedEventArgs
        {
            public string AuditoryFeedback { get; private set; }
            public CalculatorApp.NumbersAndOperatorsEnum Operation { get; private set; }

            public CalculatorButtonPressedEventArgs(string feedback, CalculatorApp.NumbersAndOperatorsEnum operation)
            {
                AuditoryFeedback = feedback;
                Operation = operation;
            }

            static public NumbersAndOperatorsEnum GetOperationFromCommandParameter(object commandParameter)
            {
                if (commandParameter is CalculatorButtonPressedEventArgs eventArgs)
                {
                    return eventArgs.Operation;
                }
                else
                {
                    return (NumbersAndOperatorsEnum)commandParameter;
                }
            }

            static public string GetAuditoryFeedbackFromCommandParameter(object commandParameter)
            {
                if (commandParameter is CalculatorButtonPressedEventArgs eventArgs)
                {
                    return eventArgs.AuditoryFeedback;
                }
                else
                {
                    return null;
                }
            }
        };
    }
}
