// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// Task 3, slice D (RED): LinuxSettings replaces ApplicationData.LocalSettings
// with a write-through JSON bag at $XDG_CONFIG_HOME/calculator-linux/.
using Calculator.Linux.Services;
using Xunit;

namespace Calculator.Linux.Tests.Services;

public sealed class SettingsTests : IDisposable
{
    private readonly string _dir = Path.Combine(Path.GetTempPath(), Path.GetRandomFileName());

    public void Dispose()
    {
        try
        {
            Directory.Delete(_dir, recursive: true);
        }
        catch
        {
        }
    }

    [Fact]
    public void Values_RoundTripAcrossInstances()
    {
        var settings = new LinuxSettings(_dir);
        settings.Set("UnitConverterPreferences", "prefs-blob");
        settings.Set("Count", 42L);
        settings.Set("Flag", true);
        settings.Set("Ratio", 1.25);
        settings.Set("Stamp", new DateTimeOffset(2026, 10, 10, 12, 0, 0, TimeSpan.Zero));

        var reopened = new LinuxSettings(_dir);
        Assert.Equal("prefs-blob", reopened.Get<string>("UnitConverterPreferences"));
        Assert.Equal(42L, reopened.Get<long>("Count"));
        Assert.True(reopened.Get<bool>("Flag"));
        Assert.Equal(1.25, reopened.Get<double>("Ratio"));
        Assert.Equal(new DateTimeOffset(2026, 10, 10, 12, 0, 0, TimeSpan.Zero), reopened.Get<DateTimeOffset>("Stamp"));
    }

    [Fact]
    public void MissingFile_StartsEmpty()
    {
        var settings = new LinuxSettings(_dir);
        Assert.False(settings.Contains("Anything"));
        Assert.Null(settings.Get<string>("Anything"));
    }

    [Fact]
    public void CorruptFile_StartsEmptyAndRecoversOnWrite()
    {
        Directory.CreateDirectory(_dir);
        File.WriteAllText(Path.Combine(_dir, "settings.json"), "{ not json");

        var settings = new LinuxSettings(_dir);
        Assert.False(settings.Contains("Anything"));

        settings.Set("K", "V");
        Assert.Equal("V", new LinuxSettings(_dir).Get<string>("K"));
    }

    [Fact]
    public void Remove_DeletesKey()
    {
        var settings = new LinuxSettings(_dir);
        settings.Set("K", "V");
        Assert.True(settings.Contains("K"));
        settings.Remove("K");
        Assert.False(settings.Contains("K"));
        Assert.False(new LinuxSettings(_dir).Contains("K"));
    }

    [Fact]
    public void DefaultDirectory_HonorsXdgConfigHome()
    {
        string xdg = Path.Combine(Path.GetTempPath(), Path.GetRandomFileName());
        try
        {
            Environment.SetEnvironmentVariable("XDG_CONFIG_HOME", xdg);
            var settings = new LinuxSettings();
            settings.Set("K", "V");
            Assert.True(File.Exists(Path.Combine(xdg, "calculator-linux", "settings.json")));
        }
        finally
        {
            Environment.SetEnvironmentVariable("XDG_CONFIG_HOME", null);
            try
            {
                Directory.Delete(xdg, recursive: true);
            }
            catch
            {
            }
        }
    }
}
