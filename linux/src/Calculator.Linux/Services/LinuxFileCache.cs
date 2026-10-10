// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// File cache replacing ApplicationData.Current.LocalCacheFolder (WinRT):
// plain text files under $XDG_CACHE_HOME/calculator-linux/ (~/.cache
// fallback). Used by the currency loader for FX snapshots.
namespace Calculator.Linux.Services;

public sealed class LinuxFileCache
{
    private readonly string _directory;

    public LinuxFileCache(string? directory = null)
    {
        _directory = directory ?? DefaultDirectory();
    }

    public static string DefaultDirectory()
    {
        string? xdg = Environment.GetEnvironmentVariable("XDG_CACHE_HOME");
        string baseDir = !string.IsNullOrEmpty(xdg)
            ? xdg
            : Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.UserProfile), ".cache");
        return Path.Combine(baseDir, "calculator-linux");
    }

    public async Task WriteTextAsync(string fileName, string contents)
    {
        Directory.CreateDirectory(_directory);
        await File.WriteAllTextAsync(Path.Combine(_directory, fileName), contents);
    }

    public async Task<string?> ReadTextAsync(string fileName)
    {
        try
        {
            string path = Path.Combine(_directory, fileName);
            if (!File.Exists(path))
            {
                return null;
            }
            return await File.ReadAllTextAsync(path);
        }
        catch (Exception ex) when (ex is IOException or UnauthorizedAccessException)
        {
            return null;
        }
    }
}
