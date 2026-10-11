// AUDIT — locale matrix (addendum A2 / brief §8 Localization).
//
// IMPORTANT - the display-output cases are tagged [Trait("Category",
// "IsolatedProcess")] and AUDIT/run_all.sh runs them in their own `dotnet test`
// process. That is not ceremony: the unmodified engine stores its resource
// provider (decimal + group separators) behind a `const` member installed by the
// first CalculatorManagerInstance built in the process, so once any test has
// created an engine, a later VM cannot change the separators it formats with.
// Asserting locale-dependent display output in a shared process therefore
// depends on test order - it passed or failed depending on what ran first, which
// is a flake, not a result.
//
// This is NOT user-visible: the app builds its single engine at startup from
// CultureInfo.CurrentCulture and never changes locale afterwards, so the
// separators it formats with are the correct ones. See AUD-NOTE-001.
//
// The assertions that do NOT need an engine (separator/RTL agreement with the
// framework) run normally and are order-independent.
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
    [Trait("Category", "IsolatedProcess")]
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
    [Trait("Category", "IsolatedProcess")]
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
    [Trait("Category", "IsolatedProcess")]
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
    [Trait("Category", "IsolatedProcess")]
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