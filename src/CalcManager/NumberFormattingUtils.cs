// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

using System;
using System.Globalization;
using System.Text;

namespace CalcManager.UnitConversionManager
{
    public static class NumberFormattingUtils
    {
        public static void TrimTrailingZeros(ref string number)
        {
            if (string.IsNullOrEmpty(number) || !number.Contains("."))
            {
                return;
            }

            int lastNonZero = -1;
            for (int i = number.Length - 1; i >= 0; i--)
            {
                if (number[i] != '0')
                {
                    lastNonZero = i;
                    break;
                }
            }

            if (lastNonZero != -1)
            {
                number = number.Substring(0, lastNonZero + 1);
            }

            if (number.EndsWith("."))
            {
                number = number.Substring(0, number.Length - 1);
            }
        }

        public static uint GetNumberDigits(string value)
        {
            TrimTrailingZeros(ref value);
            uint numberSignificantDigits = (uint)value.Length;
            if (value.Contains("."))
            {
                numberSignificantDigits--;
            }
            if (value.Contains("-"))
            {
                numberSignificantDigits--;
            }
            return numberSignificantDigits;
        }

        public static uint GetNumberDigitsWholeNumberPart(double value)
        {
            return value == 0 ? 1u : (uint)(1 + Math.Max(0.0, Math.Log10(Math.Abs(value))));
        }

        public static string RoundSignificantDigits(double num, uint numSignificant)
        {
            return num.ToString($"F{numSignificant}", CultureInfo.InvariantCulture);
        }

        public static string ToScientificNumber(double number)
        {
            string s = number.ToString("e6", CultureInfo.InvariantCulture);
            return System.Text.RegularExpressions.Regex.Replace(s, @"e([+-])0(\d\d)$", "e$1$2");
        }
    }
}
