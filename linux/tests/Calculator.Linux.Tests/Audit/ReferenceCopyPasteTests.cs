// AUDIT — replay of the REFERENCE's own CopyPasteManager unit tests.
//
// Source: src/Calculator.Tests/CopyPasteManagerTests.cs @ d125246a (unmodified
// reference). Expectations transcribed verbatim; MSTest attributes replaced with
// xUnit. These exercise pure static parsing helpers, so they transfer directly to
// the port's Calculator.Linux.Common.CopyPasteManager.
//
// These matter for a Linux port specifically: pasted text on X11 comes from a
// different set of sources than UWP's clipboard, so the sanitiser and operand
// extraction have to behave exactly as upstream or Ctrl-V silently mis-parses.

using Calculator.Linux.Common;
using Xunit;

namespace Calculator.Linux.Tests.Audit;

public sealed class ReferenceCopyPasteTests
{
    private static void AssertListsAreEqual(IList<string> expected, IList<string> actual)
    {
        Assert.Equal(expected.Count, actual.Count);
        for (int i = 0; i < expected.Count; i++)
        {
            Assert.Equal(expected[i], actual[i]);
        }
    }

    // ---- reference: ValidateExtractOperands ----

    [Fact]
    public void ExtractOperands_ReferenceCases()
    {
        AssertListsAreEqual(
            new List<string> { "123456" },
            CopyPasteManager.ExtractOperands("123456", ViewMode.Standard));

        AssertListsAreEqual(
            new List<string> { "123^456" },
            CopyPasteManager.ExtractOperands("123^456", ViewMode.Standard));

        var twoOperands = new List<string> { "123", "456" };
        AssertListsAreEqual(CopyPasteManager.ExtractOperands("123+456", ViewMode.Standard), twoOperands);
        AssertListsAreEqual(CopyPasteManager.ExtractOperands("123-456", ViewMode.Standard), twoOperands);
        AssertListsAreEqual(CopyPasteManager.ExtractOperands("123*456", ViewMode.Standard), twoOperands);
        AssertListsAreEqual(CopyPasteManager.ExtractOperands("123/456", ViewMode.Standard), twoOperands);

        AssertListsAreEqual(
            new List<string> { "123e456" },
            CopyPasteManager.ExtractOperands("123e456", ViewMode.Standard));
        AssertListsAreEqual(
            new List<string> { "123e4567" },
            CopyPasteManager.ExtractOperands("123e4567", ViewMode.Standard));

        var twoOperandsParens = new List<string> { "((45)", "(-30))" };
        AssertListsAreEqual(
            CopyPasteManager.ExtractOperands("((45)+(-30))", ViewMode.Scientific),
            twoOperandsParens);
    }

    // ---- reference: ValidateExtractOperandsErrors ----

    [Fact]
    public void ExtractOperands_LimitAndExponentErrors()
    {
        string atLimit = string.Empty;
        for (uint i = 0; i < CopyPasteManager.MaxOperandCount; i++)
        {
            atLimit += "+1";
        }
        Assert.Equal(
            (uint)CopyPasteManager.ExtractOperands(atLimit, ViewMode.Standard).Count,
            (uint)100);

        atLimit += "+1";
        Assert.Equal(
            (uint)CopyPasteManager.ExtractOperands(atLimit, ViewMode.Standard).Count,
            (uint)0);

        Assert.Equal(
            (uint)CopyPasteManager.ExtractOperands("12e9999", ViewMode.Standard).Count,
            (uint)1);
        Assert.Equal(
            (uint)CopyPasteManager.ExtractOperands("12e10000", ViewMode.Standard).Count,
            (uint)0);
    }

    // ---- reference: ValidateSanitizeOperand ----

    [Fact]
    public void SanitizeOperand_ReferenceCases()
    {
        Assert.Equal(CopyPasteManager.SanitizeOperand("((1234"), "1234");
        Assert.Equal(CopyPasteManager.SanitizeOperand("1234))"), "1234");
        Assert.Equal(CopyPasteManager.SanitizeOperand("-1234"), "1234");
        Assert.Equal(CopyPasteManager.SanitizeOperand("+1234"), "1234");
        Assert.Equal(CopyPasteManager.SanitizeOperand("-(1234)"), "1234");
        Assert.Equal(CopyPasteManager.SanitizeOperand("+(1234)"), "1234");
        Assert.Equal(CopyPasteManager.SanitizeOperand("12-34"), "1234");
        Assert.Equal(CopyPasteManager.SanitizeOperand("((((1234))))"), "1234");
        Assert.Equal(CopyPasteManager.SanitizeOperand("1'2'3'4"), "1234");
        Assert.Equal(CopyPasteManager.SanitizeOperand("'''''1234''''"), "1234");
        Assert.Equal(CopyPasteManager.SanitizeOperand("1_2_3_4"), "1234");
        Assert.Equal(CopyPasteManager.SanitizeOperand("______1234___"), "1234");
    }

    // ---- reference: ValidatePrefixCurrencySymbols ----

    [Fact]
    public void RemoveUnwantedChars_StripsCurrencySymbols()
    {
        // Written as \uXXXX escapes exactly as the reference does, so the test
        // cannot be corrupted by file encoding.
        Assert.Equal("5", CopyPasteManager.RemoveUnwantedCharsFromString("\u00A5\u0035"));
        Assert.Equal("5", CopyPasteManager.RemoveUnwantedCharsFromString("\u00A4\u0035"));
        Assert.Equal("5", CopyPasteManager.RemoveUnwantedCharsFromString("\u20B5\u0035"));
        Assert.Equal("5", CopyPasteManager.RemoveUnwantedCharsFromString("\u0024\u0035"));
        Assert.Equal("5", CopyPasteManager.RemoveUnwantedCharsFromString("\u20A1\u0035"));
        Assert.Equal("5", CopyPasteManager.RemoveUnwantedCharsFromString("\u20A9\u0035"));
        Assert.Equal("5", CopyPasteManager.RemoveUnwantedCharsFromString("\u20AA\u0035"));
        Assert.Equal("5", CopyPasteManager.RemoveUnwantedCharsFromString("\u20A6\u0035"));
        Assert.Equal("5", CopyPasteManager.RemoveUnwantedCharsFromString("\u20B9\u0035"));
        Assert.Equal("5", CopyPasteManager.RemoveUnwantedCharsFromString("\u00A3\u0035"));
        Assert.Equal("5", CopyPasteManager.RemoveUnwantedCharsFromString("\u20AC\u0035"));
    }
}
