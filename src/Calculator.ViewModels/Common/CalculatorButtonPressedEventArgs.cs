// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;

namespace CalculatorApp.ViewModel.Common
{
    public sealed class CalculatorButtonPressedEventArgs
    {
        public string AuditoryFeedback { get; }
        public NumbersAndOperatorsEnum Operation { get; }

        public CalculatorButtonPressedEventArgs(string auditoryFeedback, NumbersAndOperatorsEnum operation)
        {
            AuditoryFeedback = auditoryFeedback;
            Operation = operation;
        }

        public static NumbersAndOperatorsEnum GetOperationFromCommandParameter(object commandParameter)
        {
            if (commandParameter is CalculatorButtonPressedEventArgs eventArgs)
            {
                return eventArgs.Operation;
            }
            if (commandParameter is int operation)
            {
                return (NumbersAndOperatorsEnum)operation;
            }
            if (commandParameter is NumbersAndOperatorsEnum enumValue)
            {
                return enumValue;
            }
            throw new ArgumentException(
                $"Unsupported command parameter: {commandParameter?.GetType().ToString() ?? "null"}",
                nameof(commandParameter));
        }

        public static string GetAuditoryFeedbackFromCommandParameter(object commandParameter)
        {
            if (commandParameter is CalculatorButtonPressedEventArgs eventArgs)
            {
                return eventArgs.AuditoryFeedback;
            }
            return string.Empty;
        }
    }
}
