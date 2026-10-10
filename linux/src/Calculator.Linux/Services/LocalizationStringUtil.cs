// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// Linux port of LocalizationStringUtil. The upstream implementation shells
// out to kernel32 FormatMessage for %1..%5 substitution; that is Windows-only
// and overkill here — numbered inserts are plain positional replacement, so
// this is implemented in managed code with identical observable behavior for
// the %1..%9 patterns used across Resources.resw.
namespace Calculator.Linux.Services;

public static class LocalizationStringUtil
{
    public static string GetLocalizedString(string message) => message ?? string.Empty;

    public static string GetLocalizedString(string message, string param1) =>
        FormatMessageFromString(message, param1);

    public static string GetLocalizedString(string message, string param1, string param2) =>
        FormatMessageFromString(message, param1, param2);

    public static string GetLocalizedString(string message, string param1, string param2, string param3) =>
        FormatMessageFromString(message, param1, param2, param3);

    public static string GetLocalizedString(string message, string param1, string param2, string param3, string param4) =>
        FormatMessageFromString(message, param1, param2, param3, param4);

    public static string GetLocalizedString(string message, string param1, string param2, string param3, string param4, string param5) =>
        FormatMessageFromString(message, param1, param2, param3, param4, param5);

    private static string FormatMessageFromString(string message, params string[] args)
    {
        if (string.IsNullOrEmpty(message))
        {
            return string.Empty;
        }
        if (args.Length == 0)
        {
            return message;
        }

        // Replace %1..%9 (FormatMessage inserts). Higher numbers cannot occur
        // (max 5 args); multi-digit sequences like %10 are treated as %1 + "0",
        // matching FormatMessage's left-to-right single-digit parsing.
        var sb = new System.Text.StringBuilder(message.Length + 16);
        for (int i = 0; i < message.Length; i++)
        {
            char c = message[i];
            if (c == '%' && i + 1 < message.Length && message[i + 1] >= '1' && message[i + 1] <= '9')
            {
                int arg = message[i + 1] - '1';
                if (arg < args.Length)
                {
                    sb.Append(args[arg]);
                }
                i++;
            }
            else
            {
                sb.Append(c);
            }
        }
        return sb.ToString();
    }
}
