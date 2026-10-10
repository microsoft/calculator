// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// Task 3, slice A (RED): resource lookup + FormatMessage-style %1 substitution
// must work without WinRT ResourceLoader or kernel32.
using Calculator.Linux.Services;
using Xunit;

namespace Calculator.Linux.Tests.Services;

public sealed class ResourceTests
{
    [Fact]
    public void KnownKey_ReturnsEnglishText()
    {
        Assert.Equal("Standard", AppResources.GetString("StandardModeText"));
    }

    [Fact]
    public void MissingKey_ReturnsKeyItself()
    {
        // Mirrors upstream: GetResourceString falls back to the key.
        Assert.Equal("NoSuchKey_Xyz", AppResources.GetString("NoSuchKey_Xyz"));
    }

    [Fact]
    public void CEngineTable_HasDivideByZeroEntry()
    {
        // IDS_ERRORS_FIRST + 1 == 100 == "Invalid input" (see EngineStrings.h).
        Assert.Equal("Invalid input", AppResources.GetCEngineString("100"));
    }

    [Fact]
    public void MissingCEngineKey_ReturnsEmpty()
    {
        Assert.Equal(string.Empty, AppResources.GetCEngineString("NoSuchEngineKey"));
    }

    [Fact]
    public void Substitution_ReplacesNumberedPlaceholders()
    {
        string formatted = LocalizationStringUtil.GetLocalizedString("Hello %1, you have %2 messages", "Ada", "3");
        Assert.Equal("Hello Ada, you have 3 messages", formatted);
    }

    [Fact]
    public void Substitution_WithNoArgs_ReturnsMessage()
    {
        Assert.Equal("plain", LocalizationStringUtil.GetLocalizedString("plain"));
    }
}
