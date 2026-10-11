// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// Paste-validation parity (subset of upstream CopyPasteManager coverage):
// per-mode character sets, operand limits, error signaling.
using System.Globalization;
using Calculator.Linux.Common;
using Calculator.Linux.Services;
using Xunit;

namespace Calculator.Linux.Tests;

public sealed class PasteValidationTests : IDisposable
{
    private readonly LinuxLocalization _previous;

    public PasteValidationTests()
    {
        _previous = LinuxLocalization.Current;
        LinuxLocalization.Current = new LinuxLocalization(new CultureInfo("en-US"));
    }

    public void Dispose() => LinuxLocalization.Current = _previous;

    [Fact]
    public void Standard_AcceptsSimpleExpression()
    {
        string result = CopyPasteManager.ValidatePasteExpression(
            "2+2", ViewMode.Standard, NumberBase.DecBase, BitLength.BitLengthQWord);
        Assert.False(CopyPasteManager.IsErrorMessage(result));
    }

    [Fact]
    public void Standard_RejectsLetters()
    {
        string result = CopyPasteManager.ValidatePasteExpression(
            "abc", ViewMode.Standard, NumberBase.DecBase, BitLength.BitLengthQWord);
        Assert.True(CopyPasteManager.IsErrorMessage(result));
    }

    [Fact]
    public void Scientific_AcceptsParenthesesAndPower()
    {
        string result = CopyPasteManager.ValidatePasteExpression(
            "(2+3)^2", ViewMode.Scientific, NumberBase.DecBase, BitLength.BitLengthQWord);
        Assert.False(CopyPasteManager.IsErrorMessage(result));
    }

    [Fact]
    public void Programmer_AcceptsHex()
    {
        string result = CopyPasteManager.ValidatePasteExpression(
            "FF", ViewMode.Programmer, NumberBase.HexBase, BitLength.BitLengthQWord);
        Assert.False(CopyPasteManager.IsErrorMessage(result));
    }

    [Fact]
    public void Programmer_RejectsOverflow()
    {
        // 17 hex digits exceed QWORD.
        string result = CopyPasteManager.ValidatePasteExpression(
            "FFFFFFFFFFFFFFFFF", ViewMode.Programmer, NumberBase.HexBase, BitLength.BitLengthQWord);
        Assert.True(CopyPasteManager.IsErrorMessage(result));
    }

    [Fact]
    public void Converter_AcceptsDecimal()
    {
        string result = CopyPasteManager.ValidatePasteExpression(
            "12.5", ViewMode.Length, NumberBase.DecBase, BitLength.BitLengthQWord);
        Assert.False(CopyPasteManager.IsErrorMessage(result));
    }

    [Fact]
    public void OversizeInput_Rejected()
    {
        string result = CopyPasteManager.ValidatePasteExpression(
            new string('1', 600), ViewMode.Standard, NumberBase.DecBase, BitLength.BitLengthQWord);
        Assert.True(CopyPasteManager.IsErrorMessage(result));
    }
}
