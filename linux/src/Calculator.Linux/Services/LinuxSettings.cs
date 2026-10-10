// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

// Linux replacement for ApplicationData.Current.LocalSettings: a write-through
// string-keyed bag persisted as JSON. Location follows the XDG Base Directory
// spec ($XDG_CONFIG_HOME/calculator-linux/settings.json, ~/.config fallback).
//
// Types: values round-trip via System.Text.Json, so string/long/bool/double/
// DateTimeOffset all work. A corrupt file is treated as empty (never throws)
// and is replaced on the next write — matching the "settings must never crash
// the app" posture of the Windows implementation.
using System.Text.Json;

namespace Calculator.Linux.Services;

public sealed class LinuxSettings
{
    private const string AppDirName = "calculator-linux";
    private const string FileName = "settings.json";

    private static readonly JsonSerializerOptions s_json = new() { WriteIndented = false };

    private readonly string _filePath;
    private readonly Dictionary<string, JsonElement> _values = new(StringComparer.Ordinal);
    private readonly object _lock = new();

    public LinuxSettings(string? directory = null)
    {
        string dir = directory ?? DefaultDirectory();
        _filePath = Path.Combine(dir, FileName);
        Load();
    }

    public static string DefaultDirectory()
    {
        string? xdg = Environment.GetEnvironmentVariable("XDG_CONFIG_HOME");
        string baseDir = !string.IsNullOrEmpty(xdg)
            ? xdg
            : Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.UserProfile), ".config");
        return Path.Combine(baseDir, AppDirName);
    }

    public bool Contains(string key)
    {
        lock (_lock)
        {
            return _values.ContainsKey(key);
        }
    }

    public void Set<T>(string key, T value)
    {
        lock (_lock)
        {
            _values[key] = JsonSerializer.SerializeToElement(value, s_json);
            Save();
        }
    }

    public T? Get<T>(string key)
    {
        lock (_lock)
        {
            if (!_values.TryGetValue(key, out JsonElement element))
            {
                return default;
            }
            try
            {
                return element.Deserialize<T>(s_json);
            }
            catch (JsonException)
            {
                return default;
            }
        }
    }

    public void Remove(string key)
    {
        lock (_lock)
        {
            if (_values.Remove(key))
            {
                Save();
            }
        }
    }

    private void Load()
    {
        try
        {
            if (!File.Exists(_filePath))
            {
                return;
            }
            using JsonDocument doc = JsonDocument.Parse(File.ReadAllText(_filePath));
            if (doc.RootElement.ValueKind != JsonValueKind.Object)
            {
                return;
            }
            foreach (JsonProperty prop in doc.RootElement.EnumerateObject())
            {
                _values[prop.Name] = prop.Value.Clone();
            }
        }
        catch (Exception ex) when (ex is IOException or UnauthorizedAccessException or JsonException)
        {
            // Corrupt/unreadable file: start empty. Never crash the app.
            _values.Clear();
        }
    }

    private void Save()
    {
        try
        {
            Directory.CreateDirectory(Path.GetDirectoryName(_filePath)!);
            using var stream = new MemoryStream();
            using (var writer = new Utf8JsonWriter(stream))
            {
                writer.WriteStartObject();
                foreach (var pair in _values)
                {
                    writer.WritePropertyName(pair.Key);
                    pair.Value.WriteTo(writer);
                }
                writer.WriteEndObject();
            }
            File.WriteAllBytes(_filePath, stream.ToArray());
        }
        catch (Exception ex) when (ex is IOException or UnauthorizedAccessException)
        {
            // Read-only home etc.: settings become session-only. Never crash.
        }
    }
}
