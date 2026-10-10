// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// Linux replacement for WinRT DecimalFormatter / CurrencyFormatter.
// Covers exactly the surface the ViewModels use: FractionDigits,
// IsGrouped, IsDecimalPointAlwaysDisplayed, Format(double), FormatUInt,
// plus the currency-code variant (Currency + fraction digits per currency).
//
// Semantics notes vs WinRT:
//   - Grouping follows the culture's NumberGroupSizes via LinuxLocalization.
//   - Ties round half away from zero. (WinRT's currency path applies
//     RoundHalfDown; display-only differences on exact ties are documented
//     in PORTING-NOTES.md.)
//   - Currency fraction digits come from an embedded ISO 4217 minor-units
//     table (WinRT resolves them from OS data); unknown codes default to 2.
//   - Currency Format returns the NUMBER only. Upstream formats with the
//     currency code then strips it; producing the number directly is
//     equivalent and avoids locale-specific symbol surgery.
namespace Calculator.Linux.Services;

public sealed class LinuxNumberFormatter
{
    private readonly LinuxLocalization _loc;

    public LinuxNumberFormatter(LinuxLocalization loc)
    {
        _loc = loc;
    }

    public int FractionDigits { get; set; }
    public bool IsGrouped { get; set; } = true;
    public bool IsDecimalPointAlwaysDisplayed { get; set; }
    public string Currency { get; private set; } = string.Empty;

    public static LinuxNumberFormatter ForCurrency(string currencyCode, LinuxLocalization loc)
    {
        var formatter = new LinuxNumberFormatter(loc)
        {
            Currency = currencyCode,
            IsGrouped = true,
            FractionDigits = Iso4217MinorUnits.Lookup(currencyCode),
        };
        return formatter;
    }

    public string Format(double value)
    {
        if (double.IsNaN(value) || double.IsInfinity(value))
        {
            return value.ToString(System.Globalization.CultureInfo.InvariantCulture);
        }

        bool negative = value < 0 || (value == 0 && double.IsNegative(value));
        double rounded = Math.Round(Math.Abs(value), FractionDigits, MidpointRounding.AwayFromZero);

        long integerPart = (long)Math.Floor(rounded);
        double fraction = rounded - integerPart;

        string intText = IsGrouped ? GroupInteger(integerPart) : integerPart.ToString(System.Globalization.CultureInfo.InvariantCulture);

        string result;
        if (FractionDigits > 0)
        {
            long scale = (long)Math.Pow(10, FractionDigits);
            long fracDigits = (long)Math.Round(fraction * scale, MidpointRounding.AwayFromZero);
            // Carry from rounding (e.g. 0.999 with 2 digits -> 100).
            if (fracDigits >= scale)
            {
                integerPart += 1;
                intText = IsGrouped ? GroupInteger(integerPart) : integerPart.ToString(System.Globalization.CultureInfo.InvariantCulture);
                fracDigits = 0;
            }
            result = intText + _loc.DecimalSeparator + fracDigits.ToString(System.Globalization.CultureInfo.InvariantCulture).PadLeft(FractionDigits, '0');
        }
        else if (IsDecimalPointAlwaysDisplayed)
        {
            result = intText + _loc.DecimalSeparator;
        }
        else
        {
            result = intText;
        }

        if (negative)
        {
            result = "-" + result;
        }

        string localized = result;
        _loc.LocalizeDisplayValue(ref localized);
        // LocalizeDisplayValue only swaps digits; separators in `result` are
        // already locale-correct, but a non-en locale's decimal char equals
        // _loc.DecimalSeparator already, so this is a no-op for separators.
        return localized;
    }

    public string FormatUInt(uint value)
    {
        string text = IsGrouped ? GroupInteger(value) : value.ToString(System.Globalization.CultureInfo.InvariantCulture);
        _loc.LocalizeDisplayValue(ref text);
        return text;
    }

    private string GroupInteger(long value)
    {
        string digits = value.ToString(System.Globalization.CultureInfo.InvariantCulture);
        return InsertGroupSeparators(digits);
    }

    private string GroupInteger(uint value)
    {
        string digits = value.ToString(System.Globalization.CultureInfo.InvariantCulture);
        return InsertGroupSeparators(digits);
    }

    private string InsertGroupSeparators(string digits)
    {
        // Interpret LinuxLocalization.NumberGrouping ("3;0" shape):
        // repeating groups from the right; trailing ";0" repeats the last size.
        int[] sizes = ParseGrouping(_loc.NumberGrouping);
        if (sizes.Length == 0)
        {
            return digits;
        }

        var parts = new System.Collections.Generic.List<string>();
        int pos = digits.Length;
        int sizeIndex = 0;
        while (pos > 0)
        {
            int size = sizes[Math.Min(sizeIndex, sizes.Length - 1)];
            if (size <= 0)
            {
                break;
            }
            int start = Math.Max(0, pos - size);
            parts.Add(digits.Substring(start, pos - start));
            pos = start;
            if (sizeIndex < sizes.Length - 1)
            {
                sizeIndex++;
            }
        }
        if (pos > 0)
        {
            parts.Add(digits.Substring(0, pos));
        }
        parts.Reverse();
        return string.Join(_loc.GroupSeparator.ToString(), parts);
    }

    private static int[] ParseGrouping(string grouping)
    {
        var sizes = new System.Collections.Generic.List<int>();
        foreach (string part in grouping.Split(';'))
        {
            if (int.TryParse(part, out int size) && size > 0)
            {
                sizes.Add(size);
            }
            else if (part == "0")
            {
                break; // "0" = repeat previous size; sizes list already holds it last
            }
        }
        return sizes.ToArray();
    }
}
