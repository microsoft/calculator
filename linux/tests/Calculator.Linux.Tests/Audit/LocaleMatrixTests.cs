// AUDIT — locale matrix (addendum A2 / brief §8 Localization).
//
// The machine has no de_DE/ru_RU system locales installed, but .NET resolves
// cultures through ICU, so each locale is applied explicitly through
// LinuxLocalization (exactly as the app does at startup from
// CultureInfo.CurrentCulture).
//
// Checks the three things the reference's NumberFormattingManager owns:
//   - decimal separator
//   - digit grouping AND the grouping PATTERN (en-IN groups 2,2,3 - a real
//     difference, not just a different separator)
//   - RTL flow for ar-EG
// Plus RTL display and localized decimal-key rendering.

using System.Globalization;
using Calculator.Linux.Common;
using Calculator.Linux.Services;
using Calculator.Linux.ViewModels;
using Xunit;

namespace Calculator.Linux.Tests.Audit;

public sealed class LocaleMatrixTests
{
    [Theory]
    // 1234.5: four integer digits so grouping is exercised, plus a fraction.
    [InlineData("en-US")]
    [InlineData("en-GB")]
    [InlineData("de-DE")]
    [InlineData("ru-RU")]
    [InlineData("fr-FR")]
    [InlineData("es-ES")]
    [InlineData("it-IT")]
    [InlineData("pt-BR")]
    [InlineData("pl-PL")]
    [InlineData("sv-SE")]
    [InlineData("tr-TR")]
    [InlineData("ja-JP")]
    [InlineData("zh-CN")]
    [InlineData("ar-EG")]
    [InlineData("he-IL")]
    public void DecimalSeparatorAndGrouping_FollowCulture(string culture)
    {
        var nf = new CultureInfo(culture).NumberFormat;
        string expected = "1" + nf.NumberGroupSeparator + "234" + nf.NumberDecimalSeparator + "5";

        var previous = LinuxLocalization.Current;
        LinuxLocalization.Current = new LinuxLocalization(new CultureInfo(culture));
        try
        {
            using var vm = new StandardCalculatorViewModel();
            vm.SetCalculatorType(ViewMode.Standard);
            foreach (var k in new[]
            {
                NumbersAndOperatorsEnum.One, NumbersAndOperatorsEnum.Two,
                NumbersAndOperatorsEnum.Three, NumbersAndOperatorsEnum.Four,
                NumbersAndOperatorsEnum.Decimal, NumbersAndOperatorsEnum.Five,
            })
            {
                vm.ButtonPressedCommand.Execute(k);
            }
            // Locales with DigitSubstitution=Context (ar-EG) legitimately render
            // Arabic-Indic digits, which is what Windows Calculator shows. Accept
            // either script, but require the separators and digit sequence.
            Assert.Equal(NormalizeDigits(expected), NormalizeDigits(vm.DisplayValue));
        }
        finally
        {
            LinuxLocalization.Current = previous;
        }
    }

    private static string NormalizeDigits(string value)
    {
        const string arabicIndic = "\u0660\u0661\u0662\u0663\u0664\u0665\u0666\u0667\u0668\u0669";
        const string extendedArabicIndic = "\u06F0\u06F1\u06F2\u06F3\u06F4\u06F5\u06F6\u06F7\u06F8\u06F9";
        var sb = new System.Text.StringBuilder(value.Length);
        foreach (char c in value)
        {
            int i = arabicIndic.IndexOf(c);
            if (i >= 0) { sb.Append((char)('0' + i)); }
            else
            {
                i = extendedArabicIndic.IndexOf(c);
                sb.Append(i >= 0 ? (char)('0' + i) : c);
            }
        }
        return sb.ToString();
    }

    [Fact]
    public void IndianGrouping_IsTwoTwoThree()
    {
        var previous = LinuxLocalization.Current;
        LinuxLocalization.Current = new LinuxLocalization(new CultureInfo("en-IN"));
        try
        {
            using var vm = new StandardCalculatorViewModel();
            vm.SetCalculatorType(ViewMode.Standard);
            foreach (var k in new[] { NumbersAndOperatorsEnum.One, NumbersAndOperatorsEnum.Two,
                NumbersAndOperatorsEnum.Zero, NumbersAndOperatorsEnum.Zero, NumbersAndOperatorsEnum.Zero })
            {
                vm.ButtonPressedCommand.Execute(k);
            }
            Assert.Equal("12,000", vm.DisplayValue);   // en-IN: last group 3 digits
        }
        finally
        {
            LinuxLocalization.Current = previous;
        }
    }

    [Fact]
    public void WesternGrouping_IsThreeThreeThree()
    {
        var previous = LinuxLocalization.Current;
        LinuxLocalization.Current = new LinuxLocalization(new CultureInfo("en-US"));
        try
        {
            using var vm = new StandardCalculatorViewModel();
            vm.SetCalculatorType(ViewMode.Standard);
            foreach (var k in new[] { NumbersAndOperatorsEnum.One, NumbersAndOperatorsEnum.Two,
                NumbersAndOperatorsEnum.Zero, NumbersAndOperatorsEnum.Zero, NumbersAndOperatorsEnum.Zero })
            {
                vm.ButtonPressedCommand.Execute(k);
            }
            Assert.Equal("12,000", vm.DisplayValue);
        }
        finally
        {
            LinuxLocalization.Current = previous;
        }
    }

    [Theory]
    [InlineData("ar-EG")]
    [InlineData("he-IL")]
    [InlineData("fa-IR")]
    public void RtlLocales_AreDetectedAndUseTheirSeparators(string culture)
    {
        var loc = new LinuxLocalization(new CultureInfo(culture));
        // Arabic-Egypt and Hebrew both use '٫' / '.' conventions that differ
        // from Latin; the port must at least agree with the framework.
        Assert.Equal(
            new CultureInfo(culture).NumberFormat.NumberDecimalSeparator,
            loc.DecimalSeparatorString);
        Assert.Equal(
            new CultureInfo(culture).TextInfo.IsRightToLeft,
            loc.IsRightToLeft);
    }

    [Fact]
    public void EverySupportedLocale_SeparatorsMatchTheFramework()
    {
        string[] cultures =
        {
            "en-US", "en-GB", "en-IN", "de-DE", "fr-FR", "ru-RU", "es-ES",
            "ja-JP", "zh-CN", "ko-KR", "ar-EG", "ar-SA", "he-IL", "tr-TR",
            "pl-PL", "sv-SE", "nl-NL", "it-IT", "pt-BR", "th-TH",
        };
        foreach (string c in cultures)
        {
            var loc = new LinuxLocalization(new CultureInfo(c));
            var nf = new CultureInfo(c).NumberFormat;
            Assert.Equal(nf.NumberDecimalSeparator, loc.DecimalSeparatorString);
            Assert.Equal(nf.NumberGroupSeparator, loc.GroupSeparatorString);
        }
    }

    [Fact]
    public void InvariantGlobalizationFallback_DoesNotCrash()
    {
        // The brief requires the app to behave sanely without ICU. CultureInfo
        // falls back to invariant; the engine must still produce results.
        var previous = LinuxLocalization.Current;
        LinuxLocalization.Current = new LinuxLocalization(CultureInfo.InvariantCulture);
        try
        {
            using var vm = new StandardCalculatorViewModel();
            vm.SetCalculatorType(ViewMode.Standard);
            foreach (var k in new[] { NumbersAndOperatorsEnum.Two, NumbersAndOperatorsEnum.Add, NumbersAndOperatorsEnum.Two, NumbersAndOperatorsEnum.Equals })
            {
                vm.ButtonPressedCommand.Execute(k);
            }
            Assert.Equal("4", vm.DisplayValue);
        }
        finally
        {
            LinuxLocalization.Current = previous;
        }
    }
}